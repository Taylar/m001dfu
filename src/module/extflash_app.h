#ifndef			EXTFLASH_APP_H
#define			EXTFLASH_APP_H

#define			EXTFLASH_TASK_QUEUE_SIZE			32

// Note: 252--255 section was used for test and SN / MAC
typedef enum _extflash_task_msg_id{
	EXTFLASH_MSG_POWERON	= 0x0001,
	EXTFLASH_MSG_POWERDOWN,
	EXTFLASH_MSG_BSP_INIT,
	EXTFLASH_MSG_SELFTEST,
	EXTFLASH_MSG_WRITE_SN,
	EXTFLASH_MSG_READ_SN,
	EXTFLASH_MSG_WRITE_BLEMAC,
	EXTFLASH_MSG_READ_BLEMAC,
	EXTFLASH_MSG_READMAC,
	EXTFLASH_MSG_SLEEP,
	EXTFLASH_MSG_WAKEUP,
	EXTFLASH_MSG_4K_ERASE,
	EXTFLASH_MSG_READ,
	EXTFLASH_MSG_WRITE,
	EXTFLASH_MSG_WRITE_WITH_BUF,
	EXTFLASH_MSG_WRITE_BLE_BROCAST,
	EXTFLASH_MSG_READ_BLE_BROCAST,
	EXTFLASH_MSG_WRITE_SN_CUSTOM,
	EXTFLASH_MSG_READ_SN_CUSTOM,
} extflash_task_msg_id;


#define			PROCESSTYPE_EVENT		0
#define			PROCESSTYPE_CALL		1

#define			FLASH_SECTOR_LENGTH		4096

typedef	struct
{
	uint16_t	id;
	uint16_t	result;
	uint8_t*	dataAddr;
	uint8_t		data[16];
	uint32_t	length;
	uint32_t	startAddr;
	uint32_t	endAddr;
	void		(*Cb)(void *parametre);
}extflash_task_msg_t;

typedef struct
{
	void		(*PowerOn)(void);
	void		(*PowerOff)(void);
	void		(*Init)(void);
	uint16_t	(*SelfTest)(void);
	void		(*WriteSN)(extflash_task_msg_t *msg);
	void		(*ReadSN)(extflash_task_msg_t *msg);
	void		(*WriteBleMac)(extflash_task_msg_t *msg);
	void		(*ReadBleMac)(extflash_task_msg_t *msg);
	void		(*ReadMac)(extflash_task_msg_t *msg);
	void		(*Sleep)(void);
	void		(*WakeUp)(void);
	void		(*Erase_4K)(extflash_task_msg_t *msg);
	void		(*Write)(extflash_task_msg_t *msg);
	void		(*Read)(extflash_task_msg_t *msg);
	uint16_t	(*EventSet)(extflash_task_msg_t msg);
	func *		CreateTask;
} extflash_s;



extern const extflash_s extflash;


void ExtflashPowerOn(void);

void ExtflashPowerOff(void);

void ExtflashInit(void);

uint16_t ExtflashSelfTest(void);

void ExtflashWriteSN(extflash_task_msg_t *msg);

void ExtflashReadSN(extflash_task_msg_t *msg);

void ExtflashWriteBleMac(extflash_task_msg_t *msg);

void ExtflashReadBleMac(extflash_task_msg_t *msg);

void ExtflashWriteSysConfig(extflash_task_msg_t *msg);

void ExtflashReadSysConfig(extflash_task_msg_t *msg);

void ExtflashReadMac(extflash_task_msg_t *msg);

void ExtflashSleep(void);

void ExtflashWakeUp(void);

void ExtflashErase_4K(extflash_task_msg_t *msg);

void ExtflashWrite(extflash_task_msg_t *msg);

void ExtflashWriteWithBuf(extflash_task_msg_t *msg);

void ExtflashRead(extflash_task_msg_t *msg);

void ExtflashAppInit(void);


// **************************************************************************
// **********************外部flash分配***************************************
// **************************************************************************

// 数据区域分配
// 外部flash存储空间分配
// 目录区域分配
#define		DM_APP_CATALOG_AREA_START_ADD			0x00000
#define		DM_APP_CATALOG_AREA_LENGTH				0x4000

// 数据区域分配
#define		DM_APP_DATA_AREA_START_ADD				(DM_APP_CATALOG_AREA_START_ADD + DM_APP_CATALOG_AREA_LENGTH)
#define		DM_APP_DATA_AREA_LENGTH					0xA000


// 用户数据分配

#define 	SYS_CONFIG_START_ADD					0Xf000


#endif		//	EXTFLASH_APP_H
