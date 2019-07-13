#ifndef		GENERAL_H_
#define		GENERAL_H_

#include <stdint.h>
#include <string.h>



// bsp
#include "boards.h"

// mcu driver
#include "nrf_drv_spi.h"
#include "nrf_drv_gpiote.h"
#include "nrfx.h"
#include "app_util_platform.h"
#include "nrf_delay.h"

// mcu lib
#include "app_timer.h"
#include "app_button.h"

// user driver
#include "movt.h"
#include "rtc.h"
#include "singleportdrive.h"
#include "accel_sensor.h"
#include "key.h"
#include "extflash.h"
// module
#include "movt_app.h"
#include "rtc_app.h"
#include "singleportdrive_app.h"
#include "extflash_app.h"
#include "data_manage_app.h"
// osal
#include "osal.h"

// app
#include "m001_app.h"
#include "key_app.h"
#include "ble_app.h"
#include "key_app.h"
#include "actionrecongnition.h"
#include "accel_app.h"
#include "daily_sport.h"
#include "lis3dh.h"
#include "display.h"
#include "daily_datasave.h"


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


// uart debug macro
// #define 	SUPPORT_UART_PRINTF 

// ************************rtt**********************************
// rtt debug macro
// #define		SUPPORT_RTT_PRINTF

// #ifndef		SUPPORT_RTT_PRINTF

// #undef		NRF_LOG_ENABLED
// #define		NRF_LOG_ENABLED

// #endif 		// SUPPORT_RTT_PRINTF
// ************************rtt**********************************


#define 	UserDubug     NRF_LOG_INFO
// #define 	UserDubug(...)

#define		SYS_APP_HANDLE_NUM  1

// ************************version**********************************
#define		FW_VERSION			0X0026
#define		BLE_PRO_VER			0X0001


extern uint16_t	sysAppHandle;

extern char m001BraodcastName[16];

typedef enum
{
	SYS_APP_HANDLE_DAILY		= 0,
} sys_app_handle_t;


#define CONFIG_VER		0
#define MOVT_NUM		1

#pragma pack (1)
typedef struct 
{
	uint32_t 	fwVersion;
	uint16_t	length;
	uint16_t	checkCode;
	uint8_t		movtLevelA[MOVT_NUM];
	uint8_t		movtLevelB[MOVT_NUM];
	uint32_t	movtCurPos[MOVT_NUM];
	uint32_t	totalStep;
	uint32_t	aimStep;
	uint16_t	sysMode;
    uint16_t	appMode;
    uint16_t	bleMode;
	uint8_t		brocastName[16];
	rtc_time_s  time;
}sysConfig_t;
#pragma pack ()

extern sysConfig_t sysConfig;

#endif	// GENERAL_H_
