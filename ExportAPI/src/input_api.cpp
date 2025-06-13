#include <InteropDLL/InputApiWrapper.cpp>
#include <Mesen/Mesen.h>

void mesen_set_key_state(uint16_t scan_code, bool state)
{
    SetKeyState(scan_code, state);
}

void mesen_reset_key_state()
{
    ResetKeyState();
}
