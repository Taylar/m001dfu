#include "am_mcu_apollo.h"
#include "am_util_delay.h"
#include "usertype.h"
#include "interrupt.h"
#include "bat.h"

#define			BAT_ADC_PIN			12

// accuracy 1.5V/32768      detectAdc = V_bat/3   
#define     BAT_ADC_MAX                     25486       //1.16V * 3 = 3.5V
#define     BAT_ADC_MIN                     13106       //0.6V * 3 = 1.8V

// 分压电阻
#define		RES_TOTAL						3
#define		RES_TEST						1

void (*BspBatVoltageIsrCb)(uint16_t voltage_mv);


void BspBatAdcIsrCb(uint16_t adcTemp)
{
	uint16_t	voltageTemp_mv;
	
    voltageTemp_mv  = (uint16_t)(((uint32_t)adcTemp) * 2289 * RES_TOTAL / 100000 / RES_TEST);
	BspBatVoltageIsrCb(voltageTemp_mv);
}
//****************************************************
//adc bsp  init
// /HW  adc of ambiq  init
static am_hal_adc_config_t AdcConfig = {
	//
	//! Select the ADC Clock source using one of the clock source macros.
	//
	.ui32Clock=AM_HAL_ADC_CLOCK_1_5MHZ,

	//
	//! Select the ADC trigger source using a trigger source macro.
	//
	.ui32TriggerConfig=AM_HAL_ADC_TRIGGER_SOFT,

	//
	//! Use a macro to select the ADC reference voltage.
	//
	.ui32Reference=AM_REG_ADC_CFG_REFSEL_INTERNAL,
//	.ui32Reference=AM_HAL_ADC_REF_VDD,
	//
	//! Use a macro to choose a maximum sample rate setting.
	//
	.ui32MaxSampleRate=AM_HAL_ADC_MODE_125KSPS,

	//
	//! Use a macro to choose the power mode for the ADC's idle state.
	//
	.ui32PowerMode=AM_HAL_ADC_LPMODE_NONE,

	//
	//! Use the Repeat macro to enable repeating samples using Timer3A
	//
	.ui32Repeat=AM_HAL_ADC_NO_REPEAT,

	//
	//! Power on the Temperature Sensor
	//
	.ui32POnTemp=AM_HAL_ADC_POFF_TEMP,

	//
	//! Set the ADC window limits using the window limit macro.
	//
	.ui32Window=AM_HAL_ADC_WINDOW(500,10),
};


void BspBatInit(void)
{
	am_hal_mcuctrl_bandgap_enable();
//    am_hal_gpio_pin_config(BAT_ADC_PIN,AM_HAL_PIN_12_ADC0);   
	am_hal_adc_config(&AdcConfig);
	am_hal_adc_slot_config(0, (AM_HAL_ADC_SLOT_AVG_1 |
								AM_REG_ADC_SL0CFG_CHSEL0_EXT0 |
								AM_HAL_ADC_SLOT_WINDOW_EN |
								AM_HAL_ADC_SLOT_ENABLE));
	am_hal_adc_slot_config(1, 0); // unused slot
	am_hal_adc_slot_config(2, 0); // unused slot
	am_hal_adc_slot_config(3, 0); // unused slot
	am_hal_adc_slot_config(4, 0); // unused slot
	am_hal_adc_slot_config(5, 0); // unused slot
	am_hal_adc_slot_config(6, 0); // unused slot
	am_hal_adc_slot_config(7, 0); // unused slot
	am_hal_adc_int_enable(AM_HAL_ADC_INTEN_WCINC |
							AM_HAL_ADC_INTEN_WCEXC |
							AM_HAL_ADC_INTEN_FIFOOVR2 |
							AM_HAL_ADC_INTEN_FIFOOVR1 |
							AM_HAL_ADC_INTEN_SCNCMP |
							AM_HAL_ADC_INTEN_CNVCMP);
	// am_hal_interrupt_disable(AM_HAL_INTERRUPT_ADC);
	//the user interrupt priority must less than 4
	am_hal_interrupt_priority_set(AM_HAL_INTERRUPT_ADC, AM_HAL_INTERRUPT_PRIORITY(3));	
}


// enable the adc clock
void BspEnableADC(void)
{
    am_hal_adc_enable();
	am_hal_gpio_pin_config(BAT_ADC_PIN,AM_HAL_PIN_12_ADC0); 
}


// disable the uart clock
void BspDisableADC(void)
{
    am_hal_gpio_pin_config(BAT_ADC_PIN,AM_HAL_PIN_DISABLE);
	am_hal_adc_disable();
}

void BspBatStartDetect(void)
{
	am_hal_adc_trigger();
}






void BspBatIsrCbInit(xBatCb *Cb)
{
	BspBatVoltageIsrCb = Cb;
}




