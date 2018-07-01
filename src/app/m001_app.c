#include "general.h"

#define		SPORTAIM_REMAIN_TIME		5


uint16_t	sysMode;
uint16_t	appMode;
uint16_t	phoneState;
uint16_t	bleMode;

uint16_t	batState;

uint8_t 	sportModeTime;
uint8_t 	appPeerModeTime;
uint8_t 	takePicModeTime;



void rtcisr(void)
{
    SetRtcEvent();
}
void rtcNull(void)
{

}

void DailyShortKeyS0Isr(void)
{
	SetKeyEvent(DAILYAPP_MSG_SHORT_KEYS0_ISR);
}

void DailyShortKeyS1Isr(void)
{
	SetKeyEvent(DAILYAPP_MSG_SHORT_KEYS1_ISR);
}

void DailyShortKeyS2Isr(void)
{
	SetKeyEvent(DAILYAPP_MSG_SHORT_KEYS2_ISR);
}

void DailyHoldKey3sS0Isr(void)
{
	SetKeyEvent(DAILYAPP_MSG_HOLD3S_KEYS0_ISR);
}

void DailyHoldKey3sS1Isr(void)
{
	SetKeyEvent(DAILYAPP_MSG_HOLD3S_KEYS1_ISR);
}

void DailyHoldKey3sS2Isr(void)
{
	SetKeyEvent(DAILYAPP_MSG_HOLD3S_KEYS2_ISR);
}

void DailyHoldKey6sS0Isr(void)
{
	SetKeyEvent(DAILYAPP_MSG_HOLD6S_KEYS0_ISR);
}

void DailyHoldKey6sS1Isr(void)
{
	SetKeyEvent(DAILYAPP_MSG_HOLD6S_KEYS1_ISR);
}

void DailyHoldKey6sS2Isr(void)
{
	SetKeyEvent(DAILYAPP_MSG_HOLD6S_KEYS2_ISR);
}




void  M001_AppInit(void)
{
	sysMode    = SYS_POWEROFF_MODE;
	appMode    = APP_NORMAL_MODE;
	phoneState = PHONE_STATE_NORMAL;
	bleMode    = BLE_SLEEP_MODE;
	batState   = BAT_FULL_STATE;
	sportModeTime = 0;

	SinglePortDriveInit();

	MovtAppInit();

	bspAccel.BspInterfaceEnable();
	bspAccel.Sleep();
    bspAccel.BspInterfaceDisable();
	

	  rtcApp.Init();
    rtcApp.Cb_SecIsrInit(rtcisr);
    rtcApp.Cb_HalfSecIsrInit(rtcNull);
    rtcApp.Cb_MinIsrInit(rtcNull);
    rtcApp.Cb_DayIsrInit(rtcNull);

	

    keyRegister.Init();
    keyRegister.ShortKeyIsrInit_S0(SYS_APP_HANDLE_DAILY, DailyShortKeyS0Isr);
    keyRegister.ShortKeyIsrInit_S1(SYS_APP_HANDLE_DAILY, DailyShortKeyS1Isr);
    keyRegister.ShortKeyIsrInit_S2(SYS_APP_HANDLE_DAILY, DailyShortKeyS2Isr);

    keyRegister.HoldKey3sIsrInit_S0(SYS_APP_HANDLE_DAILY, DailyHoldKey3sS0Isr);
    keyRegister.HoldKey3sIsrInit_S1(SYS_APP_HANDLE_DAILY, DailyHoldKey3sS1Isr);
    keyRegister.HoldKey3sIsrInit_S2(SYS_APP_HANDLE_DAILY, DailyHoldKey3sS2Isr);

    keyRegister.HoldKey6sIsrInit_S0(SYS_APP_HANDLE_DAILY, DailyHoldKey6sS0Isr);
    keyRegister.HoldKey6sIsrInit_S1(SYS_APP_HANDLE_DAILY, DailyHoldKey6sS1Isr);
    keyRegister.HoldKey6sIsrInit_S2(SYS_APP_HANDLE_DAILY, DailyHoldKey6sS2Isr);
    
	ExtflashAppInit();

    //SetSinglePort(RED_LED, LED_PORT_ACTIVE_STATE, 125, 125, 3);

}


void M001_3Hshortkey(void)
{
	if(sysMode == SYS_POWEROFF_MODE)
	{
		// if(bleMode == BLE_BROADCAST_MODE)
		// {
		// 	SetSinglePort(RED_LED, LED_PORT_ACTIVE_STATE, 125, 125, 8);
		// }
		// else if(bleMode == BLE_CONNECT_MODE)
		// {
		// 	SetSinglePort(GREEN_LED, LED_PORT_ACTIVE_STATE, 1000, 10, 1);
		// 	BlePack(DEVICE_REQ_TAKE_PIC, &bleSendMsg);
		// }
		// else
		// {
		// 	SetSinglePort(RED_LED, LED_PORT_ACTIVE_STATE, 125, 125, 1);
		// }
	}
	else if(sysMode == SYS_WORK_MODE)
	{
		sportModeTime = 1;
		DisplaySportComplete(dailyStepComplete);

		if(bleMode == BLE_BROADCAST_MODE)
		{
			SetSinglePort(RED_LED, LED_PORT_ACTIVE_STATE, 125, 125, 8);
		}

		if(bleMode == BLE_CONNECT_MODE)
		{
			SetSinglePort(GREEN_LED, LED_PORT_ACTIVE_STATE, 1000, 10, 1);
			BlePack(DEVICE_REQ_TAKE_PIC, &bleSendMsg);
		}

		if(appPeerModeTime && (phoneState == PHONE_STATE_PEER))
		{
			appPeerModeTime = 0;
			phoneState = APP_NORMAL_MODE;
			BlePack(DEVICE_COMMIT_LINK, &bleSendMsg);
		}

		if((phoneState == PHONE_STATE_TAKE_PICTURE) && takePicModeTime)
		{
			BlePack(DEVICE_REQ_TAKE_PIC, &bleSendMsg);
		}

	}
}


void M001_3Hlongkey(void)
{
	if(sysMode == SYS_POWEROFF_MODE)
	{
		if(batState == BAT_FULL_STATE)
		{
			SetSinglePort(RED_LED, LED_PORT_ACTIVE_STATE, 130, 370, 4);
			SetSinglePort(MOTO, MOTO_PORT_ACTIVE_STATE, 130, 130, 1);

			bleMode       = BLE_BROADCAST_MODE;
			sysMode       = SYS_WORK_MODE;
			phoneState    = APP_NORMAL_MODE;
			appMode       = APP_NORMAL_MODE;
			
			sportModeTime = 0;
			rtcApp.Start();
			accel.bsp->BspInterfaceEnable();
			bspAccel.SampleSet();
			accel.bsp->BspInterfaceDisable();
			advertising_start(false);
		}
	}
	else if(sysMode == SYS_WORK_MODE)
	{
		if(bleMode == BLE_SLEEP_MODE)
		{
			 SetSinglePort(RED_LED, LED_PORT_ACTIVE_STATE, 130, 370, 4);
			 bleMode = BLE_BROADCAST_MODE;
		}
		else
		{
			SetSinglePort(RED_LED, LED_PORT_ACTIVE_STATE, 130, 370, 1);
            if(bleMode == BLE_BROADCAST_MODE)
            {
            	advertising_stop();
            }
            else
            {
            	ble_disconnect_req();
            }
            bleMode = BLE_SLEEP_MODE;
		}
	}
}


void M001_3Hlongkey_6s(void)
{

}


void M001_KeyApp(uint32_t keyEvent)
{
	if(DAILYAPP_MSG_SHORT_KEYS0_ISR & keyEvent)
	{
    	M001_3Hshortkey();
	}


	if(DAILYAPP_MSG_SHORT_KEYS1_ISR & keyEvent)
	{
    	M001_3Hshortkey();
	}


	if(DAILYAPP_MSG_SHORT_KEYS2_ISR & keyEvent)
	{
    	NRF_LOG_INFO("key s2 press\n.");

	}


	if(DAILYAPP_MSG_HOLD3S_KEYS0_ISR & keyEvent)
	{
		M001_3Hlongkey();
	}


	if(DAILYAPP_MSG_HOLD3S_KEYS1_ISR & keyEvent)
	{
		M001_3Hlongkey();
	}


	if(DAILYAPP_MSG_HOLD3S_KEYS2_ISR & keyEvent)
	{

	}


	if(DAILYAPP_MSG_HOLD6S_KEYS0_ISR & keyEvent)
	{
		M001_3Hlongkey_6s();
	}


	if(DAILYAPP_MSG_HOLD6S_KEYS1_ISR & keyEvent)
	{

	}


	if(DAILYAPP_MSG_HOLD6S_KEYS2_ISR & keyEvent)
	{

	}


}


void M001_RtcApp(void)
{
    movt_task_msg_t msg;
    rtc_time_s  timeTemp;
    static uint8_t flag;
    uint16_t posTemp;

	if(sysMode != SYS_WORK_MODE)
		return;

	if(APP_DEEPSLEEP_MODE == appMode)
		return;

	rtcApp.SecPeriodProcess();

	if(phoneState != PHONE_STATE_ADJUST_MOVT && sportModeTime == 0)
    {
    	flag = !flag;
    	rtcApp.Read(&timeTemp);
    	posTemp = GetMovtCurPos();
    	msg.aim 		= ExchangeTimeforCount(timeTemp.hour, timeTemp.min, timeTemp.sec);
    	if(posTemp > msg.aim)
    	{
    		if(flag)
    		{
    			msg.id          = MOVT_MSG_MC_FORWARD;
		        MovtEventSet(msg);
    		}
    	}
    	else
    	{
	        msg.id          = MOVT_MSG_MC_SET_AIM;
	        MovtEventSet(msg);
    	}
    }
    if(sportModeTime)
    {
    	sportModeTime++;
    	if(sportModeTime >= (SPORTAIM_REMAIN_TIME))
    	{
    		sportModeTime = 0;
    	}
    }

    if(appPeerModeTime)
    {
    	appPeerModeTime++;
    	if(appPeerModeTime >= 60)
    	{
    		appPeerModeTime = 0;
    	}
    }

    if(takePicModeTime)
    {
    	takePicModeTime ++;
    	if(takePicModeTime > 22)
    	{
    		takePicModeTime = 0;
    		phoneState = PHONE_STATE_NORMAL;
    	}

    }


    DailyStepProcess();
}




uint16_t ExchangeTimeforCount(uint8_t Hour, uint8_t Min, uint8_t Sec)
{
    uint16_t z;
    uint8_t i;
    i = Hour;
    if(Hour > 11)
    {
        i = Hour - 12;
    }
    z = (uint16_t)i *3600;
    z += (uint16_t)Min *60;
    z += Sec ;
    return z;
}
