#include <iostream>
#include <array>
#include <random>

#include "lib/emulator.h"

int main() {
    std::random_device rd{};

    mfb_set_target_fps(60);
    em_c8::chip_8 emulator{rd(), "CHIP-8 Emulator"};

    std::array<uint8_t, 5> program {{2, 10, 0, 0, 0}};
    emulator.load(0x200, program.begin(), program.end());

    while(!emulator.should_close()) {
        emulator.update_window();
    }

    return 0;
}