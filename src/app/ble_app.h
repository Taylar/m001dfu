#ifndef			BLE_APP_H_
#define			BLE_APP_H_

#define			BLE_START_SIGN		0X7E


typedef struct 
{
	uint8_t	stratSign;
	uint8_t serialNum;
	uint8_t length;
	uint8_t id[2];
	uint8_t load[20];
}__attribute__((packed)) protocal_msg_t;

enum 
{
	DEVICE_COM_ACK        = 0X0001,
	DEVICE_REQUST_RTC     = 0x0002,
	DEVICE_INFO           = 0x0003,
	DEVICE_COMMIT_LINK    = 0x0004,
	DEVICE_CUR_STEP       = 0x0005,
	DEVICE_HIS_DATA_INFO  = 0x0006,
	DEVICE_UPDATA_COMMAND = 0x0007,
	DEVICE_REQ_TAKE_PIC   = 0x0008,
	DEVICE_SPORT_AIM      = 0x0009,

	APP_COM_ACK         = 0X8001,
	APP_REPLY_RTC       = 0X8002,
	APP_REQUST_LINK     = 0X8003,
	APP_REQUST_DEV_INFO = 0X8004,
	APP_SET_STEP_AIM    = 0X8005,
	APP_REQUST_CUR_STEP = 0X8006,
	APP_TAKE_PIC_SET    = 0X8007,
	APP_SET_MOVT        = 0X8008,
	APP_SET_BRO_NAME    = 0X8009,
	APP_REQ_HIS_DATA    = 0X800A,
	APP_FW_UPDATA       = 0X800B,
	APP_REQ_PEER        = 0X800C,
};

enum 
{
	HDATA_TOTAL_INFO = 0X01,
	HDATA_CATA_INFO	= 0X02,
	HDATA_CATA_DATA	= 0X03,
	HDATA_DEL_ALL	= 0X04,
	HDATA_DEL_TYPE	= 0X05,
	HDATA_DEL_CATA	= 0X06,
};


extern uint8_t modifyBleNameFlah;

extern protocal_msg_t bleSendMsg;
extern protocal_msg_t bleRecMsg;

void ModifyBleName(void);

void advertising_init(void);

void RequestPeer(void);

void advertising_start(bool erase_bonds);

void advertising_stop(void);

void ble_disconnect_req(void);

void Ble_CommandHandleSend(uint8_t *data, uint16_t length);

void Ble_OtaHandleSend(uint8_t *data, uint16_t length);

void Ble_ldtHandleSend(uint8_t *data, uint16_t length);


void BlePack(uint32_t command, protocal_msg_t *msg);

void BleProtocal(protocal_msg_t *msg);

void BleApp(uint32_t event);

uint8_t* HexToSprintf(uint8_t *data, uint16_t length);

#endif // BLE_APP_H_

