#include <algorithm>
#include "SoftwareKeyManager.h"
#include "Utilities/FolderUtilities.h"
#include "Shared/Emulator.h"
#include "Shared/KeyDefinitions.h"

SoftwareKeyManager::SoftwareKeyManager(Emulator* emu)
{
    _emu = emu;

    ResetKeyState();

    _keyDefinitions = KeyDefinition::GetSharedKeyDefinitions();

    for(KeyDefinition &keyDef : _keyDefinitions) {
        _keyNames[keyDef.keyCode] = keyDef.name;
        _keyCodes[keyDef.name] = keyDef.keyCode;
    }

    _disableAllKeys = false;
}

SoftwareKeyManager::~SoftwareKeyManager()
{
}

void SoftwareKeyManager::RefreshState()
{
    //TODO: NOT IMPLEMENTED YET
    //Only needed to detect poll controller input
}

bool SoftwareKeyManager::IsKeyPressed(uint16_t key)
{
    if(_disableAllKeys || key == 0) {
        return false;
    }

    return _keyState[key] != 0;
}

optional<int16_t> SoftwareKeyManager::GetAxisPosition(uint16_t key)
{
    return std::nullopt;
}

bool SoftwareKeyManager::IsMouseButtonPressed(MouseButton button)
{
    return _keyState[SoftwareKeyManager::BaseMouseButtonIndex + (int)button];
}

vector<uint16_t> SoftwareKeyManager::GetPressedKeys()
{
    vector<uint16_t> pressedKeys;
    for(int i = 0; i < 0x10000; i++) {
        if(_keyState[i]) {
            pressedKeys.push_back(i);
        }
    }
    return pressedKeys;
}

string SoftwareKeyManager::GetKeyName(uint16_t key)
{
    auto keyDef = _keyNames.find(key);
    if(keyDef != _keyNames.end()) {
        return keyDef->second;
    }
    return "";
}

uint16_t SoftwareKeyManager::GetKeyCode(string keyName)
{
    auto keyDef = _keyCodes.find(keyName);
    if(keyDef != _keyCodes.end()) {
        return keyDef->second;
    }
    return 0;
}

void SoftwareKeyManager::UpdateDevices()
{
    //TODO: NOT IMPLEMENTED YET
    //Only needed to detect newly plugged in devices
}


bool SoftwareKeyManager::SetKeyState(uint16_t scanCode, bool state)
{
    if(_keyState[scanCode] != state) {
        _keyState[scanCode] = state;
        return true;
    }
    return false;
}

void SoftwareKeyManager::ResetKeyState()
{
    memset(_keyState, 0, sizeof(_keyState));
}

void SoftwareKeyManager::SetDisabled(bool disabled)
{
    _disableAllKeys = disabled;
}

void SoftwareKeyManager::SetForceFeedback(uint16_t magnitude, uint16_t magnitudeLeft)
{
}
