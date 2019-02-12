#include "general.h"

#define		SPORTAIM_REMAIN_TIME		5

uint16_t    powerMode;
uint16_t	sysMode;
uint16_t	appMode;
uint16_t	phoneState;
uint16_t	bleMode;

uint16_t	batState;

uint8_t 	sportModeTime;
uint8_t 	appPeerModeTime;
uint8_t 	takePicModeTime;

sysConfig_t sysConfig;

void rtcisr(void)
{
    SetRtcEvent();
}

void Hourisr(void)
{
    SetKeyEvent(DAILYAPP_MSG_HOUR_ISR);
}

void Dayisr(void)
{
    SetKeyEvent(DAILYAPP_MSG_DAY_ISR);
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



extern void DataManageTest(void);

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
    rtcApp.Cb_HourIsrInit(Hourisr);
    rtcApp.Cb_DayIsrInit(Dayisr);

	

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

	// data init
	modifyBleNameFlah = false;

	DailyDataSaveInit();
	DailySportInit();
	// DataManageTest();
    // SetSinglePort(MOTO, LED_PORT_ACTIVE_STATE, 125, 125, 1);
    
	if(SysConfigLoad() == 0)
		memcpy((char*)m001BraodcastName, (char*)sysConfig.brocastName, 16);
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
		if(phoneState != PHONE_STATE_PEER && (phoneState != PHONE_STATE_TAKE_PICTURE))
		{
			sportModeTime = 1;
			DisplaySportComplete(dailyStepComplete);
		}

		if(bleMode == BLE_BROADCAST_MODE)
		{
			SetSinglePort(RED_LED, LED_PORT_ACTIVE_STATE, 125, 125, 8);
		}

		if(bleMode == BLE_CONNECT_MODE)
		{
			SetSinglePort(GREEN_LED, LED_PORT_ACTIVE_STATE, 1000, 10, 1);
			// BlePack(DEVICE_REQ_TAKE_PIC, &bleSendMsg);
		}

		if(phoneState == PHONE_STATE_PEER)
		{
			appPeerModeTime = 0;
			phoneState = APP_NORMAL_MODE;
			BlePack(DEVICE_COMMIT_LINK, &bleSendMsg);
		}

		if(phoneState == PHONE_STATE_TAKE_PICTURE)
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
			SetSinglePort(RED_LED, LED_PORT_ACTIVE_STATE, 125, 125, 8);
			// SetSinglePort(MOTO, MOTO_PORT_ACTIVE_STATE, 130, 130, 1);

			bleMode       = BLE_BROADCAST_MODE;
			sysMode       = SYS_WORK_MODE;
			phoneState    = APP_NORMAL_MODE;
			appMode       = APP_NORMAL_MODE;
			
			sportModeTime = 0;
			rtcApp.Start();
			accel.bsp->BspInterfaceEnable();
			bspAccel.SampleSet();
			accel.bsp->BspInterfaceDisable();
			advertising_start(true);
		}
	}
	else if(sysMode == SYS_WORK_MODE)
	{
		if(bleMode == BLE_SLEEP_MODE)
		{
			SetSinglePort(RED_LED, LED_PORT_ACTIVE_STATE, 125, 125, 8);
			bleMode = BLE_BROADCAST_MODE;
			advertising_start(false);
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
            	// advertising_stop();
            }
            bleMode = BLE_SLEEP_MODE;
		}
	}
}


void M001_3Hlongkey_6s(void)
{
	// UpgradePreProcess();
	sd_nvic_SystemReset();
}


void M001_KeyApp(uint32_t keyEvent)
{
	if(DAILYAPP_MSG_SHORT_KEYS0_ISR & keyEvent)
	{
    	M001_3Hshortkey();
	}


	if(DAILYAPP_MSG_SHORT_KEYS1_ISR & keyEvent)
	{
    	// UpgradePreProcess();
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

	if(DAILYAPP_MSG_HOUR_ISR & keyEvent)
	{

		DailyStepSave((rtcApp.Read_GMT_Utc()/3600-1)*3600, DailyStepSaveRead());
		NRF_LOG_INFO("1hour:utc:%d,step:%d\n", (rtcApp.Read_GMT_Utc()/3600-1)*3600, DailyStepSaveRead());
		DailyStepSaveClear();
	}

	if(DAILYAPP_MSG_DAY_ISR & keyEvent)
	{
		DailySportClear();
	}


}


void M001_RtcApp(void)
{
    movt_task_msg_t msg;
    rtc_time_s  timeTemp;
    static uint8_t flag;
    uint16_t posTemp;
    uint32_t utcTemp;

	if(sysMode != SYS_WORK_MODE)
		return;

	if(APP_DEEPSLEEP_MODE == appMode)
		return;

	rtcApp.SecPeriodProcess();

	// 每天凌晨5分钟请求时间
	if(bleMode == BLE_CONNECT_MODE)
	{
		utcTemp = rtcApp.Read_Cur_Utc() % 86400;
		if((utcTemp == 300))
			BlePack(DEVICE_REQUST_RTC, &bleSendMsg);
	}

	if(phoneState != PHONE_STATE_ADJUST_MOVT && sportModeTime == 0)
    {
    	flag = !flag;
    	rtcApp.Read(&timeTemp);
    	posTemp = GetMovtCurPos();
    	msg.aim 		= ExchangeTimeforCount(timeTemp.hour, timeTemp.min, timeTemp.sec);
    	if(posTemp > msg.aim && ((posTemp - msg.aim) < 300) )
    	{
    		if(flag)
    		{
    			msg.id          = MOVT_MSG_MC_FORWARD;
		        MovtEventSet(msg);
    		}
    	}
    	else
    	{
	        msg.id          = MOVT_MSG_MC_SET_AIM_FORWARD;
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
    		phoneState = PHONE_STATE_NORMAL;
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

uint8_t CheckCode8(uint8_t *pData,  uint16_t length)
{
    uint8_t cc = 0;

    while (length) {
        cc ^= *pData;
        pData++;
        length--;
    }

    return cc;	
}

uint8_t SysConfigLoad(void)
{
	extflash_task_msg_t msgFlash;
	uint16_t check;


	msgFlash.dataAddr = (uint8_t*)(&sysConfig);
	msgFlash.length = sizeof(sysConfig_t);
	ExtflashReadSysConfig(&msgFlash);
	check = CheckCode8(sysConfig.movtLevelA, sizeof(sysConfig_t));
	if(check == sysConfig.checkCode)
		return 0;
	else
		return 0xff;
}


void SysConfigStore(void)
{
	extflash_task_msg_t msgFlash;



	msgFlash.dataAddr   = (uint8_t*)(&sysConfig);
	msgFlash.length     = sizeof(sysConfig_t);
	sysConfig.length    = sizeof(sysConfig_t) - 8;
	sysConfig.checkCode = CheckCode8(sysConfig.movtLevelA, sysConfig.length);
	ExtflashWriteSysConfig(&msgFlash);
}

void SysConfigReset(void)
{
	sysConfig.fwVersion = CONFIG_VER;
	sysConfig.length = sizeof(sysConfig_t) - 8;
	sysConfig.movtLevelA[0] = 0;
	sysConfig.movtLevelB[0] = 0;
	sysConfig.movtCurPos[0] = 0;
	sysConfig.totalStep = 0;
	sysConfig.aimStep = 10000;
	sysConfig.sysMode = SYS_POWEROFF_MODE;
	sysConfig.appMode = APP_NORMAL_MODE;
	sysConfig.bleMode = BLE_SLEEP_MODE;
	memcpy((char*)sysConfig.brocastName, (char*)m001BraodcastName, 16);
	RtcTimeRead(&sysConfig.time);
	sysConfig.checkCode = CheckCode8(sysConfig.movtLevelA, sysConfig.length);
	SysConfigStore();
}

void UpgradePreProcess(void)
{
	extflash_task_msg_t extflashMsg;
	movt_task_msg_t movtMsg;

	sysMode       = SYS_WORK_MODE;
	phoneState    = APP_NORMAL_MODE;
	appMode       = APP_NORMAL_MODE;
	
	sportModeTime = 0;

	RtcTimeRead(&sysConfig.time);

	movtMsg.id          = MOVT_MSG_MC_STOP;
    MovtEventSet(movtMsg);

    DailyStepSaveCache();

	sysConfig.fwVersion = CONFIG_VER;
	sysConfig.length = sizeof(sysConfig_t) - 8;
	sysConfig.movtLevelA[0] = GetLevelMovtM_A();;
	sysConfig.movtLevelB[0] = GetLevelMovtM_B();;
	sysConfig.movtCurPos[0] = GetMovtCurPos();
	sysConfig.totalStep = dailyTotalStep;
	sysConfig.aimStep = dailyStepAim;
	sysConfig.sysMode = SYS_UPDATA_MODE;
	sysConfig.appMode = APP_NORMAL_MODE;
	sysConfig.bleMode = BLE_BROADCAST_MODE;
	memcpy((char*)sysConfig.brocastName, (char*)m001BraodcastName, 16);
	sysConfig.checkCode = CheckCode8(sysConfig.movtLevelA, sysConfig.length);
	SysConfigStore();
}



void M001StateRecover(void)
{
	movt_task_msg_t movtMsg;

	if(SysConfigLoad() == 0xff)
    {
    	SysConfigReset();
    	SinglePortSetPolar(MOTO, MOTO_PORT_ACTIVE_STATE);
		nrf_delay_ms(125);
		SinglePortSetPolar(MOTO, !MOTO_PORT_ACTIVE_STATE);
    }
    else
    {
		if(sysConfig.sysMode == SYS_UPDATA_MODE)
		{
			sysConfig.sysMode = SYS_WORK_MODE;
			SysConfigStore();

			dailyTotalStep    = sysConfig.totalStep;
			dailyStepAim      = sysConfig.aimStep;
			sysMode           = sysConfig.sysMode;
			bleMode           = sysConfig.bleMode;

			SetSinglePort(RED_LED, LED_PORT_ACTIVE_STATE, 125, 125, 8);
			// SetSinglePort(MOTO, MOTO_PORT_ACTIVE_STATE, 130, 130, 1);

			bleMode       = BLE_BROADCAST_MODE;
			sysMode       = SYS_WORK_MODE;
			phoneState    = APP_NORMAL_MODE;
			appMode       = APP_NORMAL_MODE;
			
			sportModeTime = 0;
			RtcTimeWrite(&sysConfig.time);
			
			rtcApp.Start();
			accel.bsp->BspInterfaceEnable();
			bspAccel.SampleSet();
			accel.bsp->BspInterfaceDisable();
			advertising_start(true);


	    	SetLevelMovtM(sysConfig.movtLevelA[0]);
	    	movtMsg.id          = MOVT_MSG_MC_SET_CUR_AIM_FORWARD;
		    movtMsg.cur         = sysConfig.movtCurPos[0];
		    movtMsg.aim         = ExchangeTimeforCount(sysConfig.time.hour, sysConfig.time.min, sysConfig.time.sec);
		    // NRF_LOG_INFO("cur:%d, aim:%d", movtMsg.cur, movtMsg.aim);
		    MovtEventSet(movtMsg);
		}
    }
}

static uint16_t bleModeTemp;
void SwitchToSleepPowerMode(void)
{
	bleModeTemp = bleMode;
	if(bleMode == BLE_BROADCAST_MODE)
    {
    	advertising_stop();
    }
    else
    {
    	ble_disconnect_req();
    }
	bleMode = BLE_SLEEP_MODE;

	SetActionState(POWER_SLEEP_MODE);
	// SetSinglePort(RED_LED, LED_PORT_ACTIVE_STATE, 125, 125, 8);
}

void SwitchToWorkPowerMode(void)
{
	if(bleModeTemp == BLE_BROADCAST_MODE || BLE_CONNECT_MODE == bleModeTemp)
	{
		bleMode = BLE_BROADCAST_MODE;
		advertising_start(false);
	}
	SetActionState(POWER_WORK_MODE);
	// SetSinglePort(GREEN_LED, LED_PORT_ACTIVE_STATE, 125, 125, 8);
}