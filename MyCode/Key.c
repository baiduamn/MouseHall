#include "stm32f1xx_hal.h"
#include "Key.h"
#include "main.h"
#include "paw3395.h"

#define DEBOUNCE_DELAY_MS 5

extern uint8_t led_flag;
extern uint8_t Key_cnt;
extern uint16_t DPI;

uint8_t Left_Key_Value = 0;
uint8_t Right_Key_Value = 0;
uint8_t Middel_Key_Value = 0;
uint8_t SideBack_Key_Value = 0;

typedef enum {
    Left_Key   = 1 << 0,  // 1  → 0b0001
    Right_Key  = 1 << 1,  // 2  → 0b0010
    Middle_Key = 1 << 2,  // 4  → 0b0100
    DPI_Key    = 1 << 3,   // 8  → 0b1000
	SideBack_Key = 1 <<4   //16 → 0b10000
} KEY;

uint8_t Key_Read(void)
{

    uint8_t raw_state = 0;
    if (HAL_GPIO_ReadPin(GPIOB, LeftKey_Pin) == GPIO_PIN_RESET)   raw_state |= Left_Key;
    if (HAL_GPIO_ReadPin(GPIOB, RightKey_Pin) == GPIO_PIN_RESET)  raw_state |= Right_Key;
    if (HAL_GPIO_ReadPin(GPIOB, MiddleKey_Pin) == GPIO_PIN_RESET) raw_state |= Middle_Key;
    if (HAL_GPIO_ReadPin(GPIOB, DPIKey_Pin) == GPIO_PIN_RESET)    raw_state |= DPI_Key;
    if (HAL_GPIO_ReadPin(GPIOB, SideBack_Pin) == GPIO_PIN_RESET)    raw_state |= SideBack_Key;



    return raw_state;
}

void Key_Task(void)
{
    static uint8_t Key_Old = 0;

    uint8_t Key_Value = Key_Read();  

    uint8_t Key_Changed = Key_Old ^ Key_Value;
    uint8_t Key_Down = Key_Value & Key_Changed;
//    uint8_t Key_UP   = (~Key_Value) & Key_Changed;

    // 更新持续状态（用于 HID 报告）
    Left_Key_Value   = (Key_Value & Left_Key)   ? 1 : 0;
    Right_Key_Value  = (Key_Value & Right_Key)  ? 1 : 0;
    Middel_Key_Value = (Key_Value & Middle_Key) ? 1 : 0;
    SideBack_Key_Value = (Key_Value & SideBack_Key) ? 1 : 0;

    // 处理 DPI 按键按下事件
    if (Key_Down & DPI_Key) {
        DPI += 500;
        if (DPI > 3000) DPI = 500;
        DPI_Config(DPI);
        led_flag = 1;
        Key_cnt++;
    }

    Key_Old = Key_Value;  // 保存消抖后的稳定状态
}
