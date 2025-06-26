#include "SoftwareSoundManager.h"
#include "Core/Shared/Audio/SoundMixer.h"
#include "Core/Shared/EmuSettings.h"
#include "Core/Shared/Emulator.h"
#include "Core/Shared/MessageManager.h"
#include "Core/Shared/NotificationManager.h"

#define computeBytesLatency(sampleRate, latency, bytesPerSample)                                   \
    (int32_t)((float)(sampleRate * latency) / 1000.0f * bytesPerSample)

SoftwareSoundManager::SoftwareSoundManager(Emulator* emu) {
    _emu = emu;

    if (InitializeAudio(44100, false)) {
        _emu->GetSoundMixer()->RegisterAudioDevice(this);
    }
}

SoftwareSoundManager::~SoftwareSoundManager() { Release(); }

void SoftwareSoundManager::FillAudioBuffer(uint8_t* stream, int len) {
    ReadFromBuffer(stream, len);
}

void SoftwareSoundManager::Release() {
    Stop();
    if (_buffer) {
        delete[] _buffer;
        _buffer = nullptr;
        _bufferSize = 0;
    }
}

bool SoftwareSoundManager::InitializeAudio(uint32_t sampleRate, bool isStereo) {
    _sampleRate = sampleRate;
    _isStereo = isStereo;
    _previousLatency = _emu->GetSettings()->GetAudioConfig().AudioLatency;

    int bytesPerSample = 2 * (isStereo ? 2 : 1);
    int32_t requestedByteLatency
        = computeBytesLatency(sampleRate, _previousLatency, bytesPerSample);
    _bufferSize = (int32_t)std::ceil((double)requestedByteLatency * 2 / 0x10000) * 0x10000;
    _buffer = new uint8_t[_bufferSize];
    memset(_buffer, 0, _bufferSize);

    _writePosition = 0;
    _readPosition = 0;

    _needReset = false;

    return true;
}

string SoftwareSoundManager::GetAvailableDevices() { return "SoftwareAudioDevice"; }

void SoftwareSoundManager::SetAudioDevice(string deviceName) {
    if (deviceName.compare(_deviceName) != 0) {
        _deviceName = deviceName;
        _needReset = true;
    }
}

void SoftwareSoundManager::ReadFromBuffer(uint8_t* output, uint32_t len) {
    std::lock_guard lk(buffer_mutex_);

    if (_readPosition + len < _bufferSize) {
        memcpy(output, _buffer + _readPosition, len);
        _readPosition += len;
    } else {
        int remainingBytes = (_bufferSize - _readPosition);
        memcpy(output, _buffer + _readPosition, remainingBytes);
        memcpy(output + remainingBytes, _buffer, len - remainingBytes);
        _readPosition = len - remainingBytes;
    }

    if (_readPosition >= _writePosition && _readPosition - _writePosition < _bufferSize / 2) {
        _bufferUnderrunEventCount++;
    }
}

void SoftwareSoundManager::WriteToBuffer(uint8_t* input, uint32_t len) {
    if (_writePosition + len < _bufferSize) {
        memcpy(_buffer + _writePosition, input, len);
        _writePosition += len;
    } else {
        int remainingBytes = _bufferSize - _writePosition;
        memcpy(_buffer + _writePosition, input, remainingBytes);
        memcpy(_buffer, ((uint8_t*)input) + remainingBytes, len - remainingBytes);
        _writePosition = len - remainingBytes;
    }
}
void SoftwareSoundManager::PlayBuffer(int16_t* soundBuffer,
                                      uint32_t sampleCount,
                                      uint32_t sampleRate,
                                      bool isStereo) {
    std::lock_guard lk(buffer_mutex_);

    uint32_t bytesPerSample = 2 * (isStereo ? 2 : 1);
    uint32_t latency = _emu->GetSettings()->GetAudioConfig().AudioLatency;
    if (_sampleRate != sampleRate || _isStereo != isStereo || _needReset
        || _previousLatency != latency) {
        Release();
        InitializeAudio(sampleRate, isStereo);
    }

    WriteToBuffer((uint8_t*)soundBuffer, sampleCount * bytesPerSample);

    int32_t byteLatency = computeBytesLatency(sampleRate, latency, bytesPerSample);
    int32_t playWriteByteLatency = _writePosition - _readPosition;
    if (playWriteByteLatency < 0) {
        playWriteByteLatency = _bufferSize - _readPosition + _writePosition;
    }

    if (playWriteByteLatency > byteLatency) {
        AudioDeviceParam deviceConfig = { sampleRate, isStereo };
        _emu->GetNotificationManager()->SendNotification(ConsoleNotificationType::StartAudioDevice,
                                                         &deviceConfig);
    }
}

void SoftwareSoundManager::Pause() {
    _emu->GetNotificationManager()->SendNotification(ConsoleNotificationType::StopAudioDevice,
                                                     nullptr);
}

void SoftwareSoundManager::Stop() {
    std::lock_guard lk(buffer_mutex_);

    Pause();
    _readPosition = 0;
    _writePosition = 0;
    ResetStats();
}

void SoftwareSoundManager::ProcessEndOfFrame() {
    std::lock_guard lk(buffer_mutex_);

    ProcessLatency(_readPosition, _writePosition);

    uint32_t emulationSpeed = _emu->GetSettings()->GetEmulationSpeed();
    if (_averageLatency > 0 && emulationSpeed <= 100 && emulationSpeed > 0
        && std::abs(_averageLatency - _emu->GetSettings()->GetAudioConfig().AudioLatency) > 50) {
        // Latency is way off (over 50ms gap), stop audio & start again
        Stop();
    }
}
