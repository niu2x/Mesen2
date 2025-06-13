#include <Mesen/Mesen.h>
#include <InteropDLL/ConfigApiWrapper.cpp>

void mesen_set_output_to_stdout(bool enable) {
    SetEmulationFlag(EmulationFlags::OutputToStdout, enable);
}

ControllerConfig from(const MesenControllerConfig& src)
{
    ControllerConfig config;
    config.Type = (ControllerType)src.type;
    config.Keys = {
        .Mapping1 = {
            .A = src.key_mapping.A,
            .B = src.key_mapping.B,
            .Up = src.key_mapping.up,
            .Down = src.key_mapping.down,
            .Left = src.key_mapping.left,
            .Right = src.key_mapping.right,
            .Start = src.key_mapping.start,
            .Select = src.key_mapping.select,
        },
    };
    return config;
}

void mesen_set_NES_config(const MesenNesConfig* NES_config)
{
    auto config = GetNesConfig();
    memcpy(config.UserPalette, NES_config->user_palette, sizeof(NES_config->user_palette));

    config.Port1 = from(NES_config->port_1);
    config.Port2 = from(NES_config->port_2);

    SetNesConfig(config);
}