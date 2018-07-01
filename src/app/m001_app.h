#ifndef			M001_APP_H_
#define			M001_APP_H_

typedef enum 
{
	SYS_POWEROFF_MODE = 0,
	SYS_WORK_MODE,
	SYS_FACTORY_MODE,
}sys_mode_e;


typedef enum
{
	APP_NORMAL_MODE = 0,
	APP_SOPRTAIM_MODE,
	APP_UPGRADE_MODE,
	APP_SLEEP_MODE,
	APP_DEEPSLEEP_MODE,
}app_mode_e;

typedef enum
{
	BAT_FULL_STATE = 0,
	BAT_LOW_STATE,
}bat_state_e;


typedef enum
{
	PHONE_STATE_NORMAL          = 0,
    PHONE_STATE_PEER,
    PHONE_STATE_TAKE_PICTURE,
    PHONE_STATE_FIND_PHONE,
    PHONE_STATE_REJECT_CALL,
    PHONE_STATE_ADJUST_MOVT,
    PHONE_STATE_UPLOAD,
    PHONE_STATE_OTA,
    PHONE_STATE_AUTHOR,
	PHONE_STATE_DAILY_SPORT,
}phone_mode_e;

typedef enum 
{
	BLE_SLEEP_MODE = 0,
	BLE_BROADCAST_MODE,
	BLE_CONNECT_MODE,
	BLE_PEER_MODE,
}ble_mode_e;



extern uint16_t	sysMode;
extern uint16_t	appMode;
extern uint16_t	phoneState;
extern uint16_t	bleMode;
extern uint16_t	batState;


extern uint8_t 	sportModeTime;
extern uint8_t 	appPeerModeTime;
extern uint8_t 	takePicModeTime;



void  M001_AppInit(void);

uint16_t ExchangeTimeforCount(uint8_t Hour, uint8_t Min, uint8_t Sec);

void M001_KeyApp(uint32_t keyEvent);

void M001_RtcApp(void);


#endif   // M001_APP_H_
