#include "main.h"
#include "Hall.h"
#include "stm32f1xx_hal.h"
#include "myMouse.h"

extern ADC_HandleTypeDef hadc1;
 uint8_t SideForward_Key_Value = 0;
uint8_t Sideflag = 0;

extern composite_hid_report_t hid_report;


const uint16_t ZERO_OFFSET = 2175U;
			

uint32_t My_ADC_Read(void)
{
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK) {
        return HAL_ADC_GetValue(&hadc1);
    }
    return 0;
}

uint16_t Hall_Output(void)
{	
	uint16_t Output;
	uint16_t ADValue;
	ADValue = My_ADC_Read();					//??AD????
	if (ADValue <= ZERO_OFFSET){
		return	Output = 0;
	
	}
	else{
		uint16_t diff = ADValue - ZERO_OFFSET;
		Output = (diff * 4095U) / 25U;
		
//		if (Output <= 920U && Sideflag == 1){
//			Sideflag = 0;
//		}
//		if (Output >= 3280U && Sideflag == 0)
//		{
//			Sideflag = 1;
//			SideForward_Key_Value = 1;
//		}else{
//			SideForward_Key_Value = 0;
//		}
		
		if (Output > 4095)  Output = 4095U;
		return Output;
		}
}

void trigger(void)
{
	uint16_t adc_value;
	adc_value = Hall_Output();
	myGamepad_update(&hid_report, adc_value, adc_value);
}