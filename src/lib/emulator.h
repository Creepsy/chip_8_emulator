#pragma once

#include <cstdint>
#include <array>
#include <cstddef>
#include <istream>

namespace em_c8 {
    /*enum keys {
        KEY_ZERO,
        KEY_ONE,
        KEY_TWO,
        KEY_THREE,
        KEY_FOUR,
        KEY_FIVE,
        KEY_SIX,
        KEY_SEVEN,
        KEY_EIGHT,
        KEY_NINE,
        KEY_A,
        KEY_B,
        KEY_C,
        KEY_D,
        KEY_E,
        KEY_F
    };
    */
    

    class chip_8 {
        private:
            std::array<uint8_t, 4096> ram;
            std::array<uint8_t, 16> registers;
            std::array<uint16_t, 16> stack;
            std::array<uint8_t, 64 * 32> video_buffer;

            uint16_t i;
            uint8_t delay_timer;
            uint8_t sound_timer;
            uint16_t pc;
            uint8_t sp;

        public:
            chip_8();
            void set_pc(const uint16_t pc);
            void load(const size_t start_address, std::istream& input);
            void load(const size_t start_address, const uint8_t* begin, const uint8_t* end);
            void next_cycle();
            ~chip_8();
    };    
}
