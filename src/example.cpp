#include <iostream>
#include <array>
#include <random>
#include <fstream>

#include "lib/emulator.h"

int main(int argc, char* argv[]) {
    if(argc != 2) {
        std::cerr << "Invalid arguments! Expected <file_name> as argument!" << std::endl;
        return -1;
    }

    std::ifstream rom{argv[1]};

    if(!rom.is_open()) {
        std::cerr << "Unable to open the ROM file!" << std::endl;
        return -1;
    }

    std::random_device rd{};

    mfb_set_target_fps(60);
    em_c8::chip_8 emulator{rd(), "CHIP-8 Emulator"};
    emulator.load(0x200, rom);
    rom.close();

    std::cout << "Loaded ROM successfully!" << std::endl;

    while(!emulator.should_close()) {
        emulator.next_cycle();
        emulator.update_window();
    }

    return 0;
}