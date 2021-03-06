//
// Created by gijsl on 5/1/2021.
//

#include <ltc6811_config_register.h>
#include <cstring>
#include "segment_board.h"

SegmentBoard::SegmentBoard() {
    registers.CFGR[0] = CFGR0;
    registers.CFGR[1] = CFGR1;
    registers.CFGR[2] = CFGR2;
    registers.CFGR[3] = CFGR3;
    registers.CFGR[4] = CFGR4;
    registers.CFGR[5] = CFGR5;

    registers.COMM[0] = 0x80; //default to mux state 0
    registers.COMM[1] = 0b11100000 + 0x09; //default to all LED's on
    registers.COMM[2] = 0xF0;
    registers.COMM[3] = 0x09;
    registers.COMM[4] = 0xF0;
    registers.COMM[5] = 0x09;
}

void SegmentBoard::set_leds(uint8_t state) {
    registers.COMM[1] = (state << 5) + 0x09;
}

void SegmentBoard::set_mux(uint8_t state) {
    mux_state = state;
    registers.COMM[0] = 0x80 + (real_mux_states[mux_state] & 0x0F);
}

void SegmentBoard::update_volts(uint8_t cmd, uint8_t *buffer) {
    if (cmd == COMMAND_RDCVA) {
        memcpy(registers.CVAR, buffer, 6);
        cell_volts[0] = (registers.CVAR[0] & 0xff) + (registers.CVAR[1] << 8) + 50;
        cell_volts[1] = (registers.CVAR[2] & 0xff) + (registers.CVAR[3] << 8);
        cell_volts[2] = (registers.CVAR[4] & 0xff) + (registers.CVAR[5] << 8);
    } else if (cmd == COMMAND_RDCVB) {
        memcpy(registers.CVBR, buffer, 6);
        cell_volts[3] = (registers.CVBR[0] & 0xff) + (registers.CVBR[1] << 8);
        cell_volts[4] = (registers.CVBR[2] & 0xff) + (registers.CVBR[3] << 8);
        cell_volts[5] = (registers.CVBR[4] & 0xff) + (registers.CVBR[5] << 8);
    } else if (cmd == COMMAND_RDCVC) {
        memcpy(registers.CVCR, buffer, 6);
        cell_volts[6] = (registers.CVCR[0] & 0xff) + (registers.CVCR[1] << 8);
        cell_volts[7] = (registers.CVCR[2] & 0xff) + (registers.CVCR[3] << 8);
        cell_volts[8] = (registers.CVCR[4] & 0xff) + (registers.CVCR[5] << 8);
    } else if (cmd == COMMAND_RDCVD) {
        memcpy(registers.CVDR, buffer, 6);
        cell_volts[9] = (registers.CVDR[0] & 0xff) + (registers.CVDR[1] << 8);
        cell_volts[10] = (registers.CVDR[2] & 0xff) + (registers.CVDR[3] << 8);
        cell_volts[11] = (registers.CVDR[4] & 0xff) + (registers.CVDR[5] << 8) + 60;
    }

    uint32_t temp = 0;
    for (unsigned short cell_volt : cell_volts) {
        temp += cell_volt;
    }
    segment_voltage = temp / 10;

    uint16_t lo = 0xffff;
    for (int i = 0; i < 12; i++) {
        if (cell_volts[i] < lo) {
            lo = cell_volts[i];
        }
    }
    lowest_voltage = lo;

    uint16_t hi = 0x0;
    for (int i = 0; i < 12; i++) {
        if (cell_volts[i] > hi) {
            hi = cell_volts[i];
        }
    }
    highest_voltage = hi;
}

void SegmentBoard::calculate_balance(uint16_t target_volt) {
    if (want_balance) {
        for (int i = 0; i < 12; i++) {
            if (cell_volts[i] > target_volt + balance_volt_target && cell_volts[i] > 30000) {
                set_balance_transistor(i, true);
            } else {
                set_balance_transistor(i, false);
            }
        }
    } else {
        for (int i = 0; i < 12; i++) {
            set_balance_transistor(i, false);
        }
    }
}

void SegmentBoard::set_balance_transistor(uint8_t cell_n, bool discharging) {
    if (cell_n == 0) {
        if (discharging) {
            registers.CFGR[4] |= 0b00000001;
        } else {
            registers.CFGR[4] &= 0b11111110;
        }
    } else if (cell_n == 1) {
        if (discharging) {
            registers.CFGR[4] |= 0b00000010;
        } else {
            registers.CFGR[4] &= 0b11111101;
        }
    } else if (cell_n == 2) {
        if (discharging) {
            registers.CFGR[4] |= 0b00000100;
        } else {
            registers.CFGR[4] &= 0b11111011;
        }
    } else if (cell_n == 3) {
        if (discharging) {
            registers.CFGR[4] |= 0b00001000;
        } else {
            registers.CFGR[4] &= 0b11110111;
        }
    } else if (cell_n == 4) {
        if (discharging) {
            registers.CFGR[4] |= 0b00010000;
        } else {
            registers.CFGR[4] &= 0b11101111;
        }
    } else if (cell_n == 5) {
        if (discharging) {
            registers.CFGR[4] |= 0b00100000;
        } else {
            registers.CFGR[4] &= 0b11011111;
        }
    } else if (cell_n == 6) {
        if (discharging) {
            registers.CFGR[4] |= 0b01000000;
        } else {
            registers.CFGR[4] &= 0b10111111;
        }
    } else if (cell_n == 7) {
        if (discharging) {
            registers.CFGR[4] |= 0b10000000;
        } else {
            registers.CFGR[4] &= 0b01111111;
        }
    } else if (cell_n == 8) {
        if (discharging) {
            registers.CFGR[5] |= 0b00000001;
        } else {
            registers.CFGR[5] &= 0b11111110;
        }
    } else if (cell_n == 9) {
        if (discharging) {
            registers.CFGR[5] |= 0b00000010;
        } else {
            registers.CFGR[5] &= 0b11111101;
        }
    } else if (cell_n == 10) {
        if (discharging) {
            registers.CFGR[5] |= 0b00000100;
        } else {
            registers.CFGR[5] &= 0b11111011;
        }
    } else if (cell_n == 11) {
        if (discharging) {
            registers.CFGR[5] |= 0b00001000;
        } else {
            registers.CFGR[5] &= 0b11110111;
        }
    }
}

float temp_conversion(uint16_t raw) {
    float raw_float = raw;
    return ((raw_float / 10.0f) - 400.0f) / 19.5f;
}

void SegmentBoard::update_temps(uint8_t *buffer) {
    cell_temps[mux_state] = temp_conversion(buffer[0] + (buffer[1] << 8));
    cell_temps[mux_state + 16] = temp_conversion(buffer[2] + (buffer[3] << 8));

    float total = 0;
    uint8_t n = 0;
    for (int i = 0; i < 24; i++) {
        if (cell_temps[i] < 100 && cell_temps[i] > 0) {
            total += cell_temps[i];
            n++;
        }
    }

    segment_temperature = total / n;
}

uint16_t SegmentBoard::lowest_cell_volt(void) {
    return lowest_voltage;
}

uint16_t SegmentBoard::highest_cell_volt(void) {
    return highest_voltage;
}

void SegmentBoard::set_allow_balance(bool do_balance) {
    want_balance = do_balance;

    if (!want_balance) {
        for (int i = 0; i < 12; i++) {
            set_balance_transistor(i, false);
        }
    }
}

void SegmentBoard::disable_balance() {
    want_balance = false;

    for (int i = 0; i < 12; i++) {
        set_balance_transistor(i, false);
    }
}

SegmentBoard::~SegmentBoard() = default;
