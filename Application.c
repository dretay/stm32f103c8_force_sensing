#include "Application.h"

extern ADC_HandleTypeDef hadc1;

#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
#define ADCCONVERTEDVALUES_BUFFER_SIZE ((uint8_t)    1)
volatile uint16_t   aADCxConvertedValues[ADCCONVERTEDVALUES_BUFFER_SIZE];

#define ADC_BUFFER_SIZE ((uint8_t)    1)
static uint16_t  adc_data[ADC_BUFFER_SIZE];
static uint16_t  adc_data_avg[ADC_BUFFER_SIZE];
static u16 readings_cnt;


static void init(void)
{	
	app_log_debug("hello, world!");	
	if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*) adc_data, ADC_BUFFER_SIZE) != HAL_OK) {
		Error_Handler();
	}

	gfxInit();

	app_log_debug("application started");
}
static void poll(void)
{

}

systemticks_t gfxSystemTicks(void)
{
	return SysTick->VAL;
}

systemticks_t gfxMillisecondsToTicks(delaytime_t ms)
{
	return ms;
}
static inline unsigned short adc_to_voltage(unsigned short reading) {
	static const unsigned long range_12_bits = 4096;
	static const unsigned long vdda = 3300;
	return ((reading * vdda) / range_12_bits);
} 
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {	
	uint8_t i = 0;	
	for (i = 0; i < ADC_BUFFER_SIZE; i++) {		
		adc_data_avg[i] = adc_data_avg[i] + ((adc_data[i] - adc_data_avg[i]) / ++readings_cnt);
	}
	if (readings_cnt == 10000)
	{
		for (i = 0; i < ADC_BUFFER_SIZE; i++) {	
			app_log_debug("%d mV", adc_to_voltage(adc_data_avg[i]));
			char output[10];
			snprintf(output, 10, "%d", adc_to_voltage(adc_data_avg[i]));
			font_t font;
			coord_t height, width, rx, ry, rcx, rcy;
			width = gdispGetWidth();
			font = gdispOpenFont("LARGENUMBERS");
			gdispClear(Black);
			gdispDrawStringBox(0, 10, width, 20, output, font, White, justifyCenter);

			adc_data_avg[i] = 0;		
		}
		readings_cnt = 0;			
	}
}


const struct application Application = { 
	.init = init,		
	.poll = poll,		
};
