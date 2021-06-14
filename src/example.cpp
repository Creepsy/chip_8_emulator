#include <iostream>
#include <array>
#include <random>

#include "lib/emulator.h"

int main() {
    std::random_device rd{};

    em_c8::chip_8 emulator{rd()};

    std::array<uint8_t, 5> program {{2, 10, 0, 0, 0}};

    emulator.load(0x200, program.begin(), program.end());

    std::cout << "project setup" << '\007' << std::endl;

    return 0;
}