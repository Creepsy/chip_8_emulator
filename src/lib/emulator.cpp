#include "emulator.h"

#include <stdexcept>
#include <thread>
//#include <iostream>
//#include <iomanip>

using namespace em_c8;

const std::array<chip_8::instruction, 16> chip_8::INSTRUCTION_TABLE = {{
    &chip_8::op_util, &chip_8::op_jmp, &chip_8::op_call, &chip_8::op_is_val,
    &chip_8::op_not_val, &chip_8::op_equals, &chip_8::op_put, &chip_8::op_add_val, &chip_8::op_arithmetric,
    &chip_8::op_not_equals, &chip_8::op_set_i, &chip_8::op_jmp_off, &chip_8::op_rand,
    &chip_8::op_draw_sprite, &chip_8::op_jmp_key, &chip_8::op_setters
}};

const std::array<uint8_t, 16> MFB_KEYMAPPINGS {{
    KB_KEY_0, KB_KEY_1, KB_KEY_2, KB_KEY_3,
    KB_KEY_4, KB_KEY_5, KB_KEY_6, KB_KEY_7,
    KB_KEY_8, KB_KEY_9, KB_KEY_A, KB_KEY_B,
    KB_KEY_C, KB_KEY_D, KB_KEY_E, KB_KEY_F,
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

chip_8::chip_8(const unsigned int seed, const std::string& title, const size_t fps)
 : rand{seed}, distrib{0, 255}, ram{}, registers{}, stack{}, video_buffer{}, i{0},
   delay_timer{0}, sound_timer{0}, pc{0x200}, sp{0}, window_update{true}, is_open{true}, window_buffer{}, next_timer_update{0} {
    size_t position = 0;
    for(const std::array<uint8_t, 5>& character : CHARACTERS) {
        this->load(position, character.begin(), character.end());
        position += character.size();
    }

    std::thread window_updater(&chip_8::update_window, this, title, fps);
    window_updater.detach();

    std::chrono::milliseconds curr = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    this->next_timer_update = curr + std::chrono::seconds{1} / 60;
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
    this->window_update = true;
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
    std::chrono::milliseconds curr = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    if(curr <= this->next_timer_update) {
        if(this->delay_timer > 0) this->delay_timer--;
        if(this->sound_timer > 0) this->sound_timer--;
        this->next_timer_update = curr + std::chrono::milliseconds{1000} / 60;
    }

    uint16_t op = (uint16_t)this->ram[this->pc] << 8 | this->ram[this->pc + 1];
    this->pc += 2;

    //std::cout << std::hex << op << " -> " << (op >> 12) << std::endl;

    (this->*INSTRUCTION_TABLE[op >> 12])(op);
}

bool chip_8::should_close() {
    return !this->is_open;
}

chip_8::~chip_8() {
}



//private
void chip_8::op_util(const uint16_t op) {
    if(op == 0x00e0) {
        this->video_buffer.fill(0);
        this->window_update = true;
    } else if(op == 0x00ee) {
        this->pc = this->stack[this->sp];
        this->sp--;
    } else {
        //not implemented
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
            {
                uint16_t tmp_res = (uint16_t)this->registers[target] + this->registers[second];
                result = tmp_res & 0xff;
                this->registers[0xf] = (tmp_res >> 8) & 1;
            }
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
    if((op & 0xf) == 0x0) {
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
    size_t x = (op >> 8) & 0xf;
    size_t y = (op >> 4) & 0xf;
    size_t sprite_size = op & 0xf;

    this->draw_sprite(this->registers[x], this->registers[y], sprite_size);
}

void chip_8::op_jmp_key(const uint16_t op) {
    uint8_t reg = (op >> 8) & 0xf;

    const uint8_t* keymap = mfb_get_key_buffer(this->window);
    if((op & 0xff) == 0x9e) {
        if(keymap[MFB_KEYMAPPINGS[this->registers[reg]]]) this->pc += 2;
    } else if((op & 0xff) == 0xa1) {
        if(!keymap[MFB_KEYMAPPINGS[this->registers[reg]]]) this->pc += 2;
    }
}

void chip_8::op_setters(const uint16_t op) {
    uint8_t reg = (op >> 8) & 0xf;

    switch(op & 0xff) {
        case 0x07:
            this->registers[reg] = this->delay_timer;
            break;
        case 0x0a:
            this->registers[reg] = this->await_key_press();
            break;
        case 0x15:
            this->delay_timer = this->registers[reg];
            break;
        case 0x18:
            this->sound_timer = this->registers[reg];
            break;
        case 0x1e:
            this->i += this->registers[reg];
            break;
        case 0x29:
            this->i = this->registers[reg] * 5;
            break;
        case 0x33:
            this->ram[this->i] = this->registers[reg] / 100;
            this->ram[this->i + 1] = this->registers[reg] % 100 / 10;
            this->ram[this->i + 2] = this->registers[reg] % 10;
            break;
        case 0x55:
            for(size_t r = 0; r <= reg; r++) {
                this->ram[this->i + r] = this->registers[r];
            }
            break;
        case 0x65:
            for(size_t r = 0; r <= reg; r++) {
                this->registers[r] = this->ram[this->i + r];
            }
            break;
        default:
            return;
    }
}

void chip_8::update_window_buffer() {
    for(size_t pixel = 0; pixel < this->window_buffer.size(); pixel++) {
        size_t x = pixel % 640;
        size_t y = (pixel - x) / 640;
        size_t vb_pos = x / 10 + y / 10 * 64;

        uint8_t value = this->video_buffer[vb_pos] * 255;
        this->window_buffer[pixel] = MFB_RGB(value, value, value);
    }
}

void chip_8::draw_sprite(const size_t x, const size_t y, const size_t sprite_size) {
    bool collision = false;

    for(size_t row = 0; row < sprite_size; row++) {
        for(size_t bit = 0; bit < 8; bit++) {
            size_t px = (x + 7 - bit) % 64;
            size_t py = (y + row) % 32;
            size_t vb_pos =  + px + py * 64;

            bool value = (this->ram[this->i + row] >> bit) & 1;

            collision |= this->video_buffer[vb_pos] & value;
            this->video_buffer[vb_pos] ^= value;
        }
    }

    this->window_update = true;
    this->registers[0xf] = collision;
}

uint8_t chip_8::await_key_press() {
    std::vector<uint8_t> pressed_keys;

    do {
        pressed_keys = this->get_pressed_keys();
    } while(pressed_keys.empty());

    return pressed_keys.back();
}

std::vector<uint8_t> chip_8::get_pressed_keys() {
    const uint8_t* keymap = mfb_get_key_buffer(this->window);
    std::vector<uint8_t> pressed_keys;

    for(size_t key = 0; key < 16; key++) {
        if(keymap[MFB_KEYMAPPINGS[key]]) pressed_keys.push_back(key);
    }

    return pressed_keys;
}

void chip_8::update_window(const std::string& title, const size_t fps) {
    mfb_set_target_fps(fps);
    this->window = mfb_open_ex(title.c_str(), 640, 320, 0);
    if(!window) throw std::runtime_error("Unable to create the emulator window!");
    while(true) {
        if(this->should_close()) return;

        if(this->window_update) {
            this->window_update = false;
            this->update_window_buffer();
        }

        int state = mfb_update_ex(window, this->window_buffer.begin(), 640, 320);
        if(state != STATE_OK) {
            this->is_open = false;
            this->window = nullptr;
            return;
        }

        mfb_wait_sync(window);
    }
}