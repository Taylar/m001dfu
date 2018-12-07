#include "general.h"


// storage region 
#define		BROCAST_NAME_ID_ADDR			0xf8000				//识别码地址,字段长度为32字节
#define		BROCAST_NAME_INFO_ADDR			0xf8020				//蓝牙广播信息地址，有效数据为32个字节

#define		CUSTOM_SN_START_ADDR			1032192				//251 section
#define		FACTORY_SELFTEST_START_ADDR		1036288				//252 section
#define		FACTORY_SN_START_ADDR			1040384				//254 section
#define		BLE_MAC_START_ADDR				1044480				//255 section


#define		BROCAST_NAME_ID_LEN				12
static const uint8_t BROCAST_NAME_ID[BROCAST_NAME_ID_LEN] =
{
	"brocast name"
};
// idle tick
#define		EXTFLASH_READ_WRITE_IDELTICKS		20
#define		EXTFLASH_ERASE_IDELTICKS			60






uint16_t IdleWait(uint16_t WaitTick)
{
	uint16_t waitCnt, ret;
	ret 	= 0;
	waitCnt = 0;
	while(bspExtflash.IdleTest())
	{
		waitCnt++;
		if(waitCnt > WaitTick)
		{
			ret = 0xff;
			break;
		}
		nrf_delay_ms(10);		// ticks
		// 防止在其他任务调用时将其关闭
		bspExtflash.WakeUp();
	}
	return ret;
}


void ExtflashPowerOn(void)
{
	bspExtflash.PowerOn();
}

void ExtflashPowerOff(void)
{
	bspExtflash.PowerDown();
}

void ExtflashInit(void)
{
	bspExtflash.BspInit();
}

uint16_t ExtflashSelfTest(void)
{
	uint8_t  dataTemp[8], i;
	uint16_t ret = 0;
	bspExtflash.WakeUp();

	bspExtflash.Erase4KCmd(FACTORY_SELFTEST_START_ADDR);
	ret |= IdleWait(EXTFLASH_ERASE_IDELTICKS);

	bspExtflash.ReadCmd(dataTemp, FACTORY_SELFTEST_START_ADDR, 8);
	for(i = 0; i < 8; i++)
	{
		if(dataTemp[i] != 0xff)
			ret |= 0xff;
		dataTemp[i] = 0x01 << i;
	}

	bspExtflash.WriteCmd(dataTemp, FACTORY_SELFTEST_START_ADDR, 8);
	ret |= IdleWait(EXTFLASH_READ_WRITE_IDELTICKS);
	bspExtflash.ReadCmd(dataTemp, FACTORY_SELFTEST_START_ADDR, 8);
	for(i = 0; i < 8; i++)
	{
		if(dataTemp[i] != 0x01<<i)
			ret |= 0xff;
	}
	ret |= IdleWait(EXTFLASH_READ_WRITE_IDELTICKS);
	bspExtflash.Sleep();
	return ret;
}


void ExtflashWriteSN(extflash_task_msg_t *msg)
{
	bspExtflash.WakeUp();
	msg->result |= IdleWait(EXTFLASH_READ_WRITE_IDELTICKS);
	bspExtflash.Erase4KCmd(FACTORY_SN_START_ADDR);
	msg->result |= IdleWait(EXTFLASH_ERASE_IDELTICKS);
	bspExtflash.WriteCmd(msg->dataAddr, FACTORY_SN_START_ADDR, msg->length);
	msg->result |= IdleWait(EXTFLASH_READ_WRITE_IDELTICKS);
	bspExtflash.Sleep();
}


void ExtflashReadSN(extflash_task_msg_t *msg)
{
	bspExtflash.WakeUp();
	msg->result |= IdleWait(EXTFLASH_READ_WRITE_IDELTICKS);
	bspExtflash.ReadCmd(msg->dataAddr, FACTORY_SN_START_ADDR, msg->length);
	msg->result |= IdleWait(EXTFLASH_READ_WRITE_IDELTICKS);
	bspExtflash.Sleep();
}

void ExtflashWriteSN_Custom(extflash_task_msg_t *msg)
{
	bspExtflash.WakeUp();
	msg->result |= IdleWait(EXTFLASH_READ_WRITE_IDELTICKS);
	bspExtflash.Erase4KCmd(CUSTOM_SN_START_ADDR);
	msg->result |= IdleWait(EXTFLASH_ERASE_IDELTICKS);
	bspExtflash.WriteCmd(msg->dataAddr, CUSTOM_SN_START_ADDR, msg->length);
	msg->result |= IdleWait(EXTFLASH_READ_WRITE_IDELTICKS);
	bspExtflash.Sleep();
}


void ExtflashReadSN_Custom(extflash_task_msg_t *msg)
{
	bspExtflash.WakeUp();
	msg->result |= IdleWait(EXTFLASH_READ_WRITE_IDELTICKS);
	bspExtflash.ReadCmd(msg->dataAddr, CUSTOM_SN_START_ADDR, msg->length);
	msg->result |= IdleWait(EXTFLASH_READ_WRITE_IDELTICKS);
	bspExtflash.Sleep();
}


void ExtflashWriteBleMac(extflash_task_msg_t *msg)
{
	bspExtflash.WakeUp();
	bspExtflash.Erase4KCmd(BLE_MAC_START_ADDR);
	msg->result |= IdleWait(EXTFLASH_ERASE_IDELTICKS);
	bspExtflash.WriteCmd(msg->dataAddr, BLE_MAC_START_ADDR, msg->length);
	msg->result |= IdleWait(EXTFLASH_READ_WRITE_IDELTICKS);
	bspExtflash.Sleep();
}


void ExtflashReadBleMac(extflash_task_msg_t *msg)
{
	bspExtflash.WakeUp();
	msg->result |= IdleWait(EXTFLASH_READ_WRITE_IDELTICKS);
	bspExtflash.ReadCmd(msg->dataAddr, BLE_MAC_START_ADDR, msg->length);
	msg->result |= IdleWait(EXTFLASH_READ_WRITE_IDELTICKS);
	bspExtflash.Sleep();
}

void ExtflashWriteBleBrocastName(extflash_task_msg_t *msg)
{
	bspExtflash.WakeUp();

	// 擦除蓝牙广播信息块
	bspExtflash.Erase4KCmd(BROCAST_NAME_ID_ADDR);
	msg->result |= IdleWait(EXTFLASH_ERASE_IDELTICKS);
	// 写入识别字段
	bspExtflash.WriteCmd((uint8_t *)BROCAST_NAME_ID, BROCAST_NAME_ID_ADDR, BROCAST_NAME_ID_LEN);
	msg->result |= IdleWait(EXTFLASH_READ_WRITE_IDELTICKS);
	// 写入广播名
	bspExtflash.WriteCmd(msg->dataAddr, BROCAST_NAME_INFO_ADDR, msg->length);
	msg->result |= IdleWait(EXTFLASH_READ_WRITE_IDELTICKS);
	bspExtflash.Sleep();
}


void ExtflashReadBleBrocastName(extflash_task_msg_t *msg)
{
	uint8_t dataTemp[BROCAST_NAME_ID_LEN];
	uint16_t i;
	bspExtflash.WakeUp();
	msg->result |= IdleWait(EXTFLASH_READ_WRITE_IDELTICKS);

	bspExtflash.ReadCmd(dataTemp, BROCAST_NAME_ID_ADDR, BROCAST_NAME_ID_LEN);
	msg->result |= IdleWait(EXTFLASH_READ_WRITE_IDELTICKS);
	for(i = 0; i < BROCAST_NAME_ID_LEN; i++)
	{
		if(BROCAST_NAME_ID[i] != dataTemp[i])
		{
			msg->result	|= 0x01;
			break;
		}
	}
	if(msg->result == 0)
	{
		
		bspExtflash.ReadCmd(msg->dataAddr, BROCAST_NAME_INFO_ADDR, msg->length);
		msg->result |= IdleWait(EXTFLASH_READ_WRITE_IDELTICKS);
	}
	bspExtflash.Sleep();
}

void ExtflashWriteSysConfig(extflash_task_msg_t *msg)
{
	bspExtflash.WakeUp();
	bspExtflash.Erase4KCmd(SYS_CONFIG_START_ADD);
	msg->result |= IdleWait(EXTFLASH_ERASE_IDELTICKS);
	bspExtflash.WriteCmd(msg->dataAddr, SYS_CONFIG_START_ADD, msg->length);
	msg->result |= IdleWait(EXTFLASH_READ_WRITE_IDELTICKS);
	bspExtflash.Sleep();
}


void ExtflashReadSysConfig(extflash_task_msg_t *msg)
{
	bspExtflash.WakeUp();
	msg->result |= IdleWait(EXTFLASH_READ_WRITE_IDELTICKS);
	bspExtflash.ReadCmd(msg->dataAddr, SYS_CONFIG_START_ADD, msg->length);
	msg->result |= IdleWait(EXTFLASH_READ_WRITE_IDELTICKS);
	bspExtflash.Sleep();
}


void ExtflashReadMac(extflash_task_msg_t *msg)
{
	uint8_t i;
	uint8_t  *dataPoint;

	bspExtflash.WakeUp();
	dataPoint = bspExtflash.ReadMac();
	for(i = 0; i < 6; i++)
	{
		msg->dataAddr[i] = dataPoint[i];
	}
	bspExtflash.Sleep();
}


void ExtflashSleep(void)
{
	bspExtflash.Sleep();
}


void ExtflashWakeUp(void)
{
	bspExtflash.WakeUp();
}


void ExtflashErase_4K(extflash_task_msg_t *msg)
{
	uint32_t i, startSector, endSector;
	bspExtflash.WakeUp();
	startSector = msg->startAddr/4096;
	endSector   = msg->endAddr/4096;	
	for(i = startSector; i <= endSector; i++)
	{
		msg->result |= IdleWait(EXTFLASH_ERASE_IDELTICKS);
		bspExtflash.Erase4KCmd(startSector << 12);
	}
	msg->result |= IdleWait(EXTFLASH_ERASE_IDELTICKS);
	bspExtflash.Sleep();
}


void ExtflashWrite(extflash_task_msg_t *msg)
{

	uint32_t lengthTemp, lengthTemp2;
	bspExtflash.WakeUp();
	// 已写入长度
	lengthTemp = 0;

	// 循环写，每次最多写入256字节，不能跨页
	while(lengthTemp < msg->length)
	{
		lengthTemp2 = 256 - ((lengthTemp + msg->startAddr) % 256);
		// if exceed the length, 
		if((msg->startAddr + lengthTemp + lengthTemp2) > msg->endAddr)
		{
			lengthTemp2	= msg->endAddr - (msg->startAddr + lengthTemp) + 1;	//
		}
		msg->result |= IdleWait(EXTFLASH_READ_WRITE_IDELTICKS);
		bspExtflash.WriteCmd(msg->dataAddr + lengthTemp, lengthTemp + msg->startAddr, lengthTemp2);
		lengthTemp += lengthTemp2;
	}
	msg->result |= IdleWait(EXTFLASH_READ_WRITE_IDELTICKS);
	bspExtflash.Sleep();
}

void ExtflashWriteWithBuf(extflash_task_msg_t *msg)
{

	uint32_t lengthTemp, lengthTemp2;
	bspExtflash.WakeUp();
	// 已写入长度
	lengthTemp = 0;

	// 循环写，每次最多写入256字节，不能跨页
	while(lengthTemp < msg->length)
	{
		lengthTemp2 = 256 - ((lengthTemp + msg->startAddr) % 256);
		// if exceed the length, 
		if((msg->startAddr + lengthTemp + lengthTemp2) > msg->endAddr)
		{
			lengthTemp2	= msg->endAddr - (msg->startAddr + lengthTemp) + 1;	//
		}
		msg->result |= IdleWait(EXTFLASH_READ_WRITE_IDELTICKS);
		bspExtflash.WriteCmd(msg->data + lengthTemp, lengthTemp + msg->startAddr, lengthTemp2);
		lengthTemp += lengthTemp2;
	}
	msg->result |= IdleWait(EXTFLASH_READ_WRITE_IDELTICKS);
	bspExtflash.Sleep();
}


void ExtflashRead(extflash_task_msg_t *msg)
{
	uint32_t lengthTemp, lengthTemp2;
	bspExtflash.WakeUp();
	lengthTemp = 0;
	while(lengthTemp < msg->length)
	{
		lengthTemp2 = 4096 - ((lengthTemp + msg->startAddr) % 4096);
		// if exceed the length, 
		if((msg->startAddr + lengthTemp + lengthTemp2) > msg->endAddr)
		{
			lengthTemp2	= msg->endAddr - (msg->startAddr + lengthTemp) + 1;	//
		}
		msg->result |= IdleWait(EXTFLASH_READ_WRITE_IDELTICKS);
		bspExtflash.ReadCmd(msg->dataAddr + lengthTemp, lengthTemp + msg->startAddr, lengthTemp2);
		lengthTemp += lengthTemp2;
	}
	bspExtflash.Sleep();
}



void ExtflashAppInit(void)
{
	bspExtflash.PowerOn();
	bspExtflash.BspInit();
	// ExtflashSelfTest();
	bspExtflash.Sleep();
}




// extflash
const extflash_s extflash = 
{
	.PowerOn		= ExtflashPowerOn,
	.PowerOff		= ExtflashPowerOff,
	.Init			= ExtflashInit,
	.SelfTest		= ExtflashSelfTest,
	.WriteSN		= ExtflashWriteSN,
	.ReadSN			= ExtflashReadSN,
	.WriteBleMac	= ExtflashWriteBleMac,
	.ReadBleMac		= ExtflashReadBleMac,
	.ReadMac		= ExtflashReadMac,
	.Sleep			= ExtflashSleep,
	.WakeUp			= ExtflashWakeUp,
	.Erase_4K		= ExtflashErase_4K,
	.Write			= ExtflashWrite,
	.Read			= ExtflashRead,
};


