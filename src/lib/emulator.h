#pragma once

#include <cstdint>
#include <array>
#include <cstddef>
#include <istream>
#include <random>

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
            std::mt19937 rand;
            std::uniform_int_distribution<uint8_t> distrib;

            std::array<uint8_t, 4096> ram;
            std::array<uint8_t, 16> registers;
            std::array<uint16_t, 16> stack;
            std::array<uint8_t, 64 * 32> video_buffer;

            uint16_t i;
            uint8_t delay_timer;
            uint8_t sound_timer;
            uint16_t pc;
            uint8_t sp;

            void op_util(const uint16_t op);
            void op_jmp(const uint16_t op);
            void op_call(const uint16_t op);
            void op_is_val(const uint16_t op);
            void op_not_val(const uint16_t op);
            void op_equals(const uint16_t op);
            void op_put(const uint16_t op);
            void op_add_val(const uint16_t op);
            void op_arithmetric(const uint16_t op);
            void op_not_equals(const uint16_t op);
            void op_set_i(const uint16_t op);
            void op_jmp_off(const uint16_t op);
            void op_rand(const uint16_t op);
            void op_draw_sprite(const uint16_t op);
            void op_jmp_key(const uint16_t op);
            void op_setters(const uint16_t op);

            typedef void(chip_8::*instruction)(const uint16_t op);
            const static std::array<instruction, 16> INSTRUCTION_TABLE;
        public:
            chip_8(const unsigned int seed);
            void set_pc(const uint16_t pc);
            void reset();
            void load(const size_t start_address, std::istream& input);
            void load(const size_t start_address, const uint8_t* begin, const uint8_t* end);
            void next_cycle();
            ~chip_8();
    };    
}
