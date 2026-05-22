#ifndef MYMOUSE_H
#define MYMOUSE_H

#include <stdint.h>

// 复合 HID 报告结构（带 Report ID，兼容 Keil ARMCC）
typedef __packed struct {
    uint8_t report_id;
    __packed union {
        __packed struct {
            uint8_t buttons;
            int16_t x;
            int16_t y;
            int8_t  wheel;
        } mouse;

        __packed struct {
            uint8_t buttons;
            uint8_t trigger_left;
            uint8_t trigger_right;
        } gamepad;
    } data;  
} composite_hid_report_t;

// 函数声明
void myMouse_init(composite_hid_report_t* report);
void myMouse_update(composite_hid_report_t* report);
void myGamepad_update(composite_hid_report_t* report, uint16_t left_adc, uint16_t right_adc);

#endif
