#include "general.h"


// key status
#define     KEY_STANDBY             0X0000
#define     KEY_DELAY               0x0001
#define     KEY_HOLE                0x0002


#define		KEY_PERIOD_CLOCK_TIME_MS	50
// hold key time
#define     HOLD_3S                 (1000/KEY_PERIOD_CLOCK_TIME_MS *3)
#define     HOLD_6S                 (1000/KEY_PERIOD_CLOCK_TIME_MS *6)


// key pin
#ifndef		BSP_KEY_PIN
#define		BSP_KEY_PIN

#define		KEY_S0_PIN					14
#define		KEY_S1_PIN					13
#define		KEY_S2_PIN					17

#endif





// pin press
#define		KEY_PRESSING			0







static unsigned short   keyDelayFlag;      //0x0000: idle 	0x0001: keydelay     0x0002:keyhold
static unsigned short   validKeyNum;      //S0:0	S1:1	S2:2
static unsigned short   holdKeyCnt;



// callback key function
bsp_key_define_s		bspDefineKey[KEY_MAX_NUM];

APP_TIMER_DEF(keyHoldTmr);

static void null_isr(void)
{

}

// set key timer 31.25ms
void BspKeyTimerInit(void)
{
    uint32_t err_code;
	err_code = app_timer_create(&keyHoldTmr,
                                APP_TIMER_MODE_REPEATED,
                                BspKeyTimerIsr);
}


void BspKeyTimerStart(void)
{
    app_timer_start(keyHoldTmr, APP_TIMER_TICKS(KEY_PERIOD_CLOCK_TIME_MS), NULL);
}

void BspKeyTimerStop(void)
{
    app_timer_stop(keyHoldTmr);
}


void BspKeyInit(void)
{
	uint32_t  err_code;
    uint8_t i;

    bspDefineKey[KEY_S0].keyPin	= KEY_S0_PIN;
	bspDefineKey[KEY_S1].keyPin	= KEY_S1_PIN;
	bspDefineKey[KEY_S2].keyPin	= KEY_S2_PIN;

	for(i = 0; i < KEY_MAX_NUM; i++)
	{
		bspDefineKey[i].ShortKey	= null_isr;
		bspDefineKey[i].Hold3sKey	= null_isr;
		bspDefineKey[i].Hold6sKey	= null_isr;
	}

	if (!nrf_drv_gpiote_is_init())
    {
        err_code = nrf_drv_gpiote_init();
//        VERIFY_SUCCESS(err_code);
    }
    

    nrf_drv_gpiote_in_config_t config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
	
    config.pull = NRF_GPIO_PIN_PULLUP;

    err_code = nrf_drv_gpiote_in_init(KEY_S0_PIN, &config, (nrfx_gpiote_evt_handler_t)BspKeyS0_Isr);
    err_code = nrf_drv_gpiote_in_init(KEY_S1_PIN, &config, (nrfx_gpiote_evt_handler_t)BspKeyS1_Isr);
    err_code = nrf_drv_gpiote_in_init(KEY_S2_PIN, &config, (nrfx_gpiote_evt_handler_t)BspKeyS2_Isr);

    for(i = 0; i < KEY_MAX_NUM; i++)
    {
    	nrf_drv_gpiote_in_event_enable(bspDefineKey[i].keyPin, true);
    }

	BspKeyTimerInit();

	keyDelayFlag			= 0;
	validKeyNum				= 0;
	holdKeyCnt				= 0;

	
}


void BspKeyTimerIsr(void * p_context)
{
	if(keyDelayFlag == KEY_HOLE)
	{
		if(nrf_gpio_pin_read(bspDefineKey[validKeyNum].keyPin) == KEY_PRESSING)
		{
			holdKeyCnt++;
			if(holdKeyCnt == HOLD_3S)	//3s keyhold function
			{
				bspDefineKey[validKeyNum].Hold3sKey();
			}
			if(holdKeyCnt == HOLD_6S)	//6s keyhold function
			{
				BspKeyTimerStop();
				keyDelayFlag 	= KEY_STANDBY;
				bspDefineKey[validKeyNum].Hold6sKey();
			}
		}
		else
		{
			// short key fuction
			BspKeyTimerStop();
			keyDelayFlag 	= KEY_STANDBY;
			if(holdKeyCnt < HOLD_3S)
				bspDefineKey[validKeyNum].ShortKey();
		}
	}

	if(keyDelayFlag == KEY_DELAY)
	{
		holdKeyCnt 		= 1;
		keyDelayFlag 	= KEY_HOLE;
	}
}




void BspKeyS0_Isr(void)
{
	if(keyDelayFlag == KEY_STANDBY)
	{
		keyDelayFlag 	= KEY_DELAY;
		validKeyNum		= KEY_S0;
		BspKeyTimerStart();
	}
}


void BspKeyS1_Isr(void)
{
	if(keyDelayFlag == KEY_STANDBY)
	{
		keyDelayFlag 	= KEY_DELAY;
		validKeyNum		= KEY_S1;
		BspKeyTimerStart();
	}
}


void BspKeyS2_Isr(void)
{
	if(keyDelayFlag == KEY_STANDBY)
	{
		keyDelayFlag 	= KEY_DELAY;
		validKeyNum		= KEY_S2;
		BspKeyTimerStart();
	}
}


uint8_t ReadState_S0(void)
{
	return nrf_gpio_pin_read(KEY_S0_PIN);
}


uint8_t ReadState_S1(void)
{
	return nrf_gpio_pin_read(KEY_S1_PIN);
}


uint8_t ReadState_S2(void)
{
	return nrf_gpio_pin_read(KEY_S2_PIN);
}


const bsp_key_s			bspKey = 
{
	.key			= bspDefineKey,
	.Init			= BspKeyInit,
	.ReadState_S0	= ReadState_S0,
	.ReadState_S1	= ReadState_S1,
	.ReadState_S2	= ReadState_S2,
};

