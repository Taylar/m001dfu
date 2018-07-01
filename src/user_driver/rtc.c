#include "general.h"



func *BspRtcTimerIsr;

void BspRtcCbIsrInit(func *Cb)
{
	BspRtcTimerIsr = Cb;	
}

static uint8_t rtc_cnt = 0;
static void rtc_handler(nrf_drv_rtc_int_type_t int_type)
{
    if (int_type == NRF_DRV_RTC_INT_TICK)
    {
        rtc_cnt ++;
        if(rtc_cnt >= 8)
        {
            rtc_cnt = 0;
            BspRtcTimerIsr();
        }
        
    }
}


void BspRtcInit(void)
{
	rtc_config(rtc_handler);
}

void BspRtcTimerStart(void)
{
	rtc_enable();
}

void BspRtcTimerStop(void)
{
	rtc_disable();
}

const bsp_rtc_s bspRtc = 
{
	.BspInit		= BspRtcInit,
	.Start			= BspRtcTimerStart,
	.Stop			= BspRtcTimerStop,
	.CbIsrInit		= BspRtcCbIsrInit,
};


