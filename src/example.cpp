#include <iostream>
#include <array>

#include "lib/emulator.h"

int main() {
    em_c8::chip_8 emulator{};

    std::array<uint8_t, 5> program {{2, 10, 0, 0, 0}};

    emulator.load(0x200, program.begin(), program.end());

    std::cout << "project setup" << std::endl;
    return 0;
}