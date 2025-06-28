#include "SoftwareSoundManager.h"
#include "Core/Shared/Audio/SoundMixer.h"
#include "Core/Shared/EmuSettings.h"
#include "Core/Shared/Emulator.h"
#include "Core/Shared/MessageManager.h"
#include "Core/Shared/NotificationManager.h"
#include "Core/Shared/RewindManager.h"

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
        data_count_ = 0;
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

    write_position_ = 0;
    read_position_ = 0;
    data_count_ = 0;

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

    uint32_t origin_len = len;

    if (data_count_ < len) {
        _bufferUnderrunEventCount++;
        len = data_count_;
    }
    data_count_ -= len;

    if (read_position_ + len < _bufferSize) {
        memcpy(output, _buffer + read_position_, len);
        read_position_ += len;
    } else {
        int remainingBytes = (_bufferSize - read_position_);
        memcpy(output, _buffer + read_position_, remainingBytes);
        memcpy(output + remainingBytes, _buffer, len - remainingBytes);
        read_position_ = len - remainingBytes;
    }

    if (origin_len > len) {
        memset(output + len, 0, origin_len - len);
    }
}

void SoftwareSoundManager::WriteToBuffer(uint8_t* input, uint32_t len) {
    if (write_position_ + len < _bufferSize) {
        memcpy(_buffer + write_position_, input, len);
        write_position_ += len;
    } else {
        int remainingBytes = _bufferSize - write_position_;
        memcpy(_buffer + write_position_, input, remainingBytes);
        memcpy(_buffer, ((uint8_t*)input) + remainingBytes, len - remainingBytes);
        write_position_ = len - remainingBytes;
    }

    data_count_ += len;
    if (data_count_ > _bufferSize) {
        data_count_ = _bufferSize;
        read_position_ = write_position_;
    }
}
void SoftwareSoundManager::PlayBuffer(int16_t* soundBuffer,
                                      uint32_t sampleCount,
                                      uint32_t sampleRate,
                                      bool isStereo) {

    std::lock_guard lk(buffer_mutex_);

    if (_emu->GetRewindManager()->IsRewinding()) {
        read_position_ = write_position_ = data_count_ = 0;
        return;
    }

    uint32_t bytesPerSample = 2 * (isStereo ? 2 : 1);
    uint32_t latency = _emu->GetSettings()->GetAudioConfig().AudioLatency;
    if (_sampleRate != sampleRate || _isStereo != isStereo || _needReset
        || _previousLatency != latency) {
        Release();
        InitializeAudio(sampleRate, isStereo);
    }

    WriteToBuffer((uint8_t*)soundBuffer, sampleCount * bytesPerSample);

    int32_t byteLatency = computeBytesLatency(sampleRate, latency, bytesPerSample);
    int32_t playWriteByteLatency = write_position_ - read_position_;
    if (playWriteByteLatency < 0) {
        playWriteByteLatency = _bufferSize - read_position_ + write_position_;
    }

    if (playWriteByteLatency > byteLatency) {
        AudioDeviceParam deviceConfig = { (uint32_t)byteLatency, sampleRate, isStereo };
        if (!audio_playing_) {
            audio_playing_ = true;
            _emu->GetNotificationManager()->SendNotification(
                ConsoleNotificationType::StartAudioDevice, &deviceConfig);
        }
    }
}

void SoftwareSoundManager::Pause() {
    if (audio_playing_) {
        audio_playing_ = false;
        _emu->GetNotificationManager()->SendNotification(ConsoleNotificationType::StopAudioDevice,
                                                         nullptr);
    }
}

void SoftwareSoundManager::Stop() {
    std::lock_guard lk(buffer_mutex_);

    Pause();
    read_position_ = 0;
    write_position_ = 0;
    ResetStats();
}

void SoftwareSoundManager::ProcessEndOfFrame() {
    std::lock_guard lk(buffer_mutex_);

    ProcessLatency(read_position_, write_position_);

    uint32_t emulationSpeed = _emu->GetSettings()->GetEmulationSpeed();
    if (_averageLatency > 0 && emulationSpeed <= 100 && emulationSpeed > 0
        && std::abs(_averageLatency - _emu->GetSettings()->GetAudioConfig().AudioLatency) > 50) {
        // Latency is way off (over 50ms gap), stop audio & start again
        Stop();
    }
}
