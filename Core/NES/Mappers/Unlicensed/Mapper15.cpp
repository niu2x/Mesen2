#include "Mapper15.h"

void Mapper15::InitMapper(RomData& romData) {
    // To run these mapper-hacked games, emulators must not enforce CHR-RAM
    // write-protection even though an actual K-1029 PCB would
    if (romData.Info.DatabaseInfo.Crc == 0xB0A706D9) {
        enable_chr_ram_write_protection_ = false;
    }
}
