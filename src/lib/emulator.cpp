#include "emulator.h"

using namespace em_c8;

chip_8::chip_8() {
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

chip_8::~chip_8() {
}