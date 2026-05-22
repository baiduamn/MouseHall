#include "MyMouse.h"
#include "usbd_hid.h"
#include "main.h"

extern USBD_HandleTypeDef hUsbDeviceFS;
extern uint8_t SideForward_Key_Value;

void myMouse_init(composite_hid_report_t* report)
{
    if (!report) return;
    report->report_id = 1;
    report->data.mouse.buttons = 0;
    report->data.mouse.x = 0;
    report->data.mouse.y = 0;
    report->data.mouse.wheel = 0;
}

void myMouse_update(composite_hid_report_t* report)
{
    if (!report) return;

    extern uint8_t Left_Key_Value;
    extern uint8_t Right_Key_Value;
    extern uint8_t Middel_Key_Value;
    extern uint8_t SideBack_Key_Value;

    extern int16_t X_Speed, Y_Speed;
    extern int8_t wheel_num;

    report->report_id = 1;
    report->data.mouse.buttons = 0;
    if (Left_Key_Value)   report->data.mouse.buttons |= (1 << 0);
    if (Right_Key_Value)  report->data.mouse.buttons |= (1 << 1);
    if (Middel_Key_Value) report->data.mouse.buttons |= (1 << 2);
	if (SideBack_Key_Value) report->data.mouse.buttons |= (1 << 3);
//	if (SideForward_Key_Value) report->data.mouse.buttons |= (1 << 3);

    report->data.mouse.x = X_Speed;
    report->data.mouse.y = Y_Speed;


    report->data.mouse.wheel = wheel_num;


    USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)report, 7);
}

void myGamepad_update(composite_hid_report_t* report, uint16_t left_adc, uint16_t right_adc)
{
    if (!report) return;

    report->report_id = 2;
    report->data.gamepad.buttons = 0;
    report->data.gamepad.trigger_left  = (uint8_t)(left_adc >> 4);
    report->data.gamepad.trigger_right = (uint8_t)(right_adc >> 4);

    USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)report, 4);
}
