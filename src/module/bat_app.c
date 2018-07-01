#include "usertype.h"
#include "sys_variable.h"
#include "bat.h"
#include "bat_app.h"

#define APP_BAT_FULL   3000
#define APP_BAT_LOW    2800

static uint16_t batVol;  // mV
static uint8_t  batLevel; // percent


void (*BatVolReadCb)(void);


 void BatVolReadIsrCbInit(void (*Cb)(void))
{
	BatVolReadCb = Cb;
}

 void BatMeasure_AdcIsr(uint16_t adcTemp)
{
	batVol		= adcTemp;
	if(BatVolReadCb != NULL)
		BatVolReadCb();
}

 void BatMeasureInit(void)
{
	bspBat.BspInit();
//	bspBat.Enable();
	bspBat.Disable();
	bspBat.InitIsrCb(BatMeasure_AdcIsr);

	BatVolReadCb	= NULL;
}

 void BatMeasureEnable(void)
{
	bspBat.Enable();
}

 void BatMeasureDisable(void)
{
	bspBat.Disable();
}

 void BatStartDetect(void)
{
	bspBat.StartDetect();
}

 void BatVolRead(uint16_t *dataTemp)
{
	*dataTemp = batVol;
}

 void BatLevelRead(uint8_t *dataTemp)
{
	if( batVol >= APP_BAT_FULL )
	{
		batLevel = 100;
	}
	else if(batVol < APP_BAT_LOW)
	{
		batLevel = 0;
	}
	else
	{
	    batLevel = 100 - (APP_BAT_FULL - batVol)*100/200;
	}		
		
    *dataTemp = batLevel;
}


//const bat_s bat = 
//{
//	.Init			= BatMeasureInit,
//	.Enable			= BatMeasureEnable,
//	.Disable		= BatMeasureDisable,
//	.StartDetect	= BatStartDetect,
//	.ReadVol		= BatVolRead,
//	.ReadLevel		= BatLevelRead,
//	.VolReadCbInit	= BatVolReadIsrCbInit,
//};
