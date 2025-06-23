#pragma once
#include <unordered_map>
#include <vector>
#include <thread>
#include "Utilities/AutoResetEvent.h"
#include "Shared/Interfaces/IKeyManager.h"
#include "Shared/KeyDefinitions.h"

class Emulator;

class SoftwareKeyManager : public IKeyManager
{
private:
    Emulator* _emu;

    vector<KeyDefinition> _keyDefinitions;
    bool _keyState[0x205];
    std::unordered_map<uint16_t, string> _keyNames;
    std::unordered_map<string, uint16_t> _keyCodes;

    bool _disableAllKeys;

public:
    SoftwareKeyManager(Emulator* emu);
    virtual ~SoftwareKeyManager();

    void RefreshState() override;
    bool IsKeyPressed(uint16_t key) override;
    optional<int16_t> GetAxisPosition(uint16_t key) override;
    bool IsMouseButtonPressed(MouseButton button) override;
    std::vector<uint16_t> GetPressedKeys() override;
    string GetKeyName(uint16_t key) override;
    uint16_t GetKeyCode(string keyName) override;

    void UpdateDevices() override;
    bool SetKeyState(uint16_t scanCode, bool state) override;
    void ResetKeyState() override;

    void SetDisabled(bool disabled) override;

    void SetForceFeedback(uint16_t magnitude, uint16_t magnitudeLeft) override;
};
