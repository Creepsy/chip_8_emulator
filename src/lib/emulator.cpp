#include "emulator.h"

using namespace em_c8;

const std::array<std::array<uint8_t, 5>, 16> CHARACTERS {{
    {0xf0, 0x90, 0x90, 0x90, 0xf0},
    {0x20, 0x60, 0x20, 0x20, 0x70},
    {0xf0, 0x10, 0xf0, 0x80, 0xf0},
    {0xf0, 0x10, 0xf0, 0x10, 0xf0},
    {0x90, 0x90, 0xf0, 0x10, 0x10},
    {0xf0, 0x80, 0xf0, 0x10, 0xf0},
    {0xf0, 0x80, 0xf0, 0x90, 0xf0},
    {0xf0, 0x10, 0x20, 0x40, 0x40},
    {0xf0, 0x90, 0xf0, 0x90, 0xf0},
    {0xf0, 0x90, 0xf0, 0x10, 0xf0},
    {0xf0, 0x90, 0xf0, 0x90, 0x90},
    {0xe0, 0x90, 0xe0, 0x90, 0xe0},
    {0xf0, 0x80, 0x80, 0x80, 0xf0},
    {0xe0, 0x90, 0x90, 0x90, 0xe0},
    {0xf0, 0x80, 0xf0, 0x80, 0xf0},
    {0xf0, 0x80, 0xf0, 0x80, 0x80}
}};

chip_8::chip_8() : ram{}, registers{}, stack{}, video_buffer{}, i{0}, delay_timer{0}, sound_timer{0}, pc{0x200}, sp{0} {
    size_t position = 0;
    for(const std::array<uint8_t, 5>& character : CHARACTERS) {
        this->load(position, character.begin(), character.end());
        position += character.size();
    }
}

void chip_8::set_pc(const uint16_t pc) {
    if(pc >= 0x200) this->pc = pc;
}

void chip_8::load(const size_t start_address, std::istream& input) {
    uint8_t op = input.get();
    size_t offset = 0;

    while(input.good()) {
        this->ram[start_address + offset] = op;
        offset++;

        op = input.get();
    }
}

void chip_8::load(const size_t start_address, const uint8_t* begin, const uint8_t* end) {
    if(!begin || !end) return;

    size_t offset = 0;
    while(begin != end) {
        this->ram[start_address + offset] = *begin;
        begin++;
        offset++;
    }
}

void chip_8::next_cycle() {

}

chip_8::~chip_8() {
}