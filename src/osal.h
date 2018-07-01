#ifndef		_OSAL_H_
#define		_OSAL_H




// osal task 
// rtc event
extern uint32_t rtcCntEvent;


// movt event
#define		MOVT_FORWARD_FINISH_EVENT		0x0001
#define		MOVT_RESVERSE_FINISH_EVENT		0x0002

extern uint32_t movtEvent;

// ble event 
#define		BLE_ADV_EVENT			0x0001
#define		BLE_CONNECT_EVENT		0x0002
#define		BLE_DISCONNECT_EVENT	0x0004
#define		BLE_COMMAND_EVENT			0x0008
#define		BLE_UPGRADE_EVENT		0x0010
#define		BLE_HIS_DATA_EVENT		0x0020


#define     DAILYAPP_MSG_SHORT_KEYS0_ISR		0x0001
#define     DAILYAPP_MSG_SHORT_KEYS1_ISR		0x0002
#define     DAILYAPP_MSG_SHORT_KEYS2_ISR		0x0004
#define     DAILYAPP_MSG_HOLD3S_KEYS0_ISR		0x0008
#define     DAILYAPP_MSG_HOLD3S_KEYS1_ISR		0x0010
#define     DAILYAPP_MSG_HOLD3S_KEYS2_ISR		0x0020
#define     DAILYAPP_MSG_HOLD6S_KEYS0_ISR		0x0040
#define     DAILYAPP_MSG_HOLD6S_KEYS1_ISR		0x0080
#define     DAILYAPP_MSG_HOLD6S_KEYS2_ISR		0x0100
// key event
#define		KEY_S0_

void NullFunc(void);

void SetRtcEvent(void);

void SetMovtEvent(uint32_t event);

void SetBleEvent(uint32_t event);

void SetKeyEvent(uint32_t event);


void OsalRun(void);

#endif
