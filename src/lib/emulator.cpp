#include "emulator.h"

using namespace em_c8;

const std::array<chip_8::instruction, 16> chip_8::INSTRUCTION_TABLE = {{
    &chip_8::op_util, &chip_8::op_jmp, &chip_8::op_call, &chip_8::op_is_val,
    &chip_8::op_not_val, &chip_8::op_equals, &chip_8::op_put, &chip_8::op_add_val, &chip_8::op_arithmetric,
    &chip_8::op_not_equals, &chip_8::op_set_i, &chip_8::op_jmp_off, &chip_8::op_rand,
    &chip_8::op_draw_sprite, &chip_8::op_jmp_key, &chip_8::op_setters
}};

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

chip_8::chip_8(const unsigned int seed)
 : rand{seed}, distrib{0, 255}, ram{}, registers{}, stack{}, video_buffer{}, i{0}, delay_timer{0}, sound_timer{0}, pc{0x200}, sp{0} {
    size_t position = 0;
    for(const std::array<uint8_t, 5>& character : CHARACTERS) {
        this->load(position, character.begin(), character.end());
        position += character.size();
    }
}

void chip_8::set_pc(const uint16_t pc) {
    if(pc >= 0x200) this->pc = pc;
}

void chip_8::reset() {
    this->registers.fill(0);
    this->stack.fill(0);
    this->video_buffer.fill(0);
    this->i = 0;
    this->delay_timer = 0;
    this->sound_timer = 0;
    this->pc = 0x200;
    this->sp = 0;
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
    uint16_t op = (uint16_t)this->ram[this->pc] << 8 | this->ram[this->pc + 1];
    this->pc += 2;

    (this->*INSTRUCTION_TABLE[op >> 12])(op);
}

chip_8::~chip_8() {
}



//private
void chip_8::op_util(const uint16_t op) {
    if(op == 0x00e0) {
        this->video_buffer.fill(0);
    } else if(op == 0x00ee) {
        //TODO
    } else {
        //TODO
    }
}

void chip_8::op_jmp(const uint16_t op) {
    this->pc = op & 0xfff;
}

void chip_8::op_call(const uint16_t op) {
    this->sp++;
    this->stack[sp] = this->pc;
    this->pc = op & 0xfff;
}

void chip_8::op_is_val(const uint16_t op) {
    uint8_t reg = (op >> 8) & 0xf;
    uint8_t val = op & 0xff;

    if(this->registers[reg] == val) this->pc += 2;
}

void chip_8::op_not_val(const uint16_t op) {
    uint8_t reg = (op >> 8) & 0xf;
    uint8_t val = op & 0xff;

    if(this->registers[reg] != val) this->pc += 2;
}

void chip_8::op_equals(const uint16_t op) {
    if(op & 0xf == 0x0) {
        uint8_t first = (op >> 8) & 0xf;
        uint8_t second = (op >> 4) & 0xf;

        if(this->registers[first] == this->registers[second]) this->pc += 2;
    }
}

void chip_8::op_put(const uint16_t op) {
    uint8_t reg = (op >> 8) & 0xf;
    uint8_t val = op & 0xff;

    this->registers[reg] = val;
}

void chip_8::op_add_val(const uint16_t op) {
    uint8_t reg = (op >> 8) & 0xf;
    uint8_t val = op & 0xff;

    this->registers[reg] += val;
}

void chip_8::op_arithmetric(const uint16_t op) {
    uint8_t target = (op >> 8) & 0xf;
    uint8_t second = (op >> 4) & 0xf;

    uint8_t result;

    switch(op & 0xF) {
        case 0x0:
            result = this->registers[second];
            break;
        case 0x1:
            result = this->registers[target] | this->registers[second];
            break;
        case 0x2:
            result = this->registers[target] & this->registers[second];
            break;
        case 0x3:
            result = this->registers[target] ^ this->registers[second];
            break;
        case 0x4:
            uint16_t tmp_res = (uint16_t)this->registers[target] + this->registers[second];
            result = tmp_res & 0xff;
            this->registers[0xf] = (tmp_res >> 8) & 1;
            break;
        case 0x5:
            this->registers[0xf] = this->registers[target] > this->registers[second];
            result = this->registers[target] - this->registers[second];
            break;
        case 0x6:
            this->registers[0xf] = this->registers[target] >> 7;
            result = this->registers[target] / 2;
            break;
        case 0x7:
            this->registers[0xf] = this->registers[second] > this->registers[target];
            result = this->registers[second] - this->registers[target];
            break;
        case 0xe:
            this->registers[0xf] = this->registers[target] >> 7;
            result = this->registers[target] * 2;
            break;
        default:
            return;
    }

    this->registers[target] = result;

}

void chip_8::op_not_equals(const uint16_t op) {
    if(op & 0xf == 0x0) {
        uint8_t first = (op >> 8) & 0xf;
        uint8_t second = (op >> 4) & 0xf;

        if(this->registers[first] != this->registers[second]) this->pc += 2;
    }
}

void chip_8::op_set_i(const uint16_t op) {
    this->i = op & 0xfff;
}

void chip_8::op_jmp_off(const uint16_t op) {
    this->pc = (op & 0xfff) + this->registers[0];
}

void chip_8::op_rand(const uint16_t op) {
    uint8_t reg = (op >> 8) & 0xf;
    uint8_t mask = op & 0xff;

    this->registers[reg] = this->distrib(this->rand) & mask;
}

void chip_8::op_draw_sprite(const uint16_t op) {
    //TODO
}

void chip_8::op_jmp_key(const uint16_t op) {
    uint8_t reg = (op >> 8) & 0xf;
    if(op & 0xff == 0x9e) {

    } else if(op & 0xff == 0xa1) {

    }
    //TODO
}

void chip_8::op_setters(const uint16_t op) {
    uint8_t reg = (op >> 8) & 0xf;

    switch(op & 0xff) {
        case 0x07:
            break;
        case 0x0a:
            break;
        case 0x15:
            break;
        case 0x18:
            break;
        case 0x1e:
            break;
        case 0x29:
            break;
        case 0x33:
            break;
        case 0x55:
            break;
        case 0x65:
            break;
    }
    //TODO
}