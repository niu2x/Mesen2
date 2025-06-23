#include "WsController.h"
#include "Shared/Emulator.h"

void WsController::InternalSetStateFromInput() 
{
    vector<KeyMapping>& keyMappings = _console->IsVerticalMode() ? _verticalMappings : _keyMappings;
    for(KeyMapping& keyMapping : keyMappings) {
        SetPressedState(Buttons::A, keyMapping.A);
        SetPressedState(Buttons::B, keyMapping.B);
        SetPressedState(Buttons::Sound, keyMapping.GenericKey1);
        SetPressedState(Buttons::Start, keyMapping.Start);
        SetPressedState(Buttons::Up, keyMapping.Up);
        SetPressedState(Buttons::Down, keyMapping.Down);
        SetPressedState(Buttons::Left, keyMapping.Left);
        SetPressedState(Buttons::Right, keyMapping.Right);

        SetPressedState(Buttons::Up2, keyMapping.U);
        SetPressedState(Buttons::Down2, keyMapping.D);
        SetPressedState(Buttons::Left2, keyMapping.L);
        SetPressedState(Buttons::Right2, keyMapping.R);

        uint8_t turboFreq = 1 << (4 - _turboSpeed);
        bool turboOn = (uint8_t)(_emu->GetFrameCount() % turboFreq) < turboFreq / 2;
        if(turboOn) {
            SetPressedState(Buttons::A, keyMapping.TurboA);
            SetPressedState(Buttons::B, keyMapping.TurboB);
        }
    }
}






