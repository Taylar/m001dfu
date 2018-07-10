#include "general.h"




// 分类数据的目录数量与数据长度返回协议
static const protocal_msg_t	PROT_PACK_TRANS_CATA_TOTAL_RET =
{
	0x23, 0x01, 0x0B, 0x13, 0x00, 0x08, 0xf0, 0x10,
};

// 分类数据的目录内容返回协议
static const protocal_msg_t	PROT_PACK_TRANS_CATA_RET =
{
	0x23, 0x01, 0x0c, 0x13, 0x00, 0x08, 0xf0, 0x11,
};

// 分类数据的数据包请求结果反馈
static const protocal_msg_t	PROT_PACK_TRANS_DATA_RET =
{
	0x23, 0x01, 0x04, 0x13, 0x00, 0x08, 0xf0, 0x12,
};

// 删除所有数据反馈
static const protocal_msg_t	PROT_PACK_TRANS_DELETE_ALL_RET =
{
	0x23, 0x01, 0x04, 0x13, 0x00, 0x80, 0xf0, 0x30,
};

// 删除某类数据反馈
static const protocal_msg_t	PROT_PACK_TRANS_DELETE_CLASS_RET =
{
	0x23, 0x01, 0x06, 0x13, 0x00, 0x80, 0xf0, 0x31,
};

// 删除目录反馈
static const protocal_msg_t	PROT_PACK_TRANS_DELETE_CATALOG_RET =
{
	0x23, 0x01, 0x0a, 0x13, 0x00, 0x80, 0xf0, 0x32,
};

// 数据包上传协议
static const protocal_msg_t	PROT_PACK_TRANS_DATA_UPLOAD =
{
	0x23, 0x01, 0x18, 0x12, 0x80, 0x01, 0xf0, 0x01,
};

#define			DAILY_STEP_NULLPACKET_MAX		12
#define			DAILY_STEP_PACKET_MAX			8
#define			DAILY_STEP_BYTE_LEN				2

#define			DAILY_DATA_UPLOAD_MAX_LEN		16


static uint16_t dailyStepNullpacketCnt;			//空包计数
static uint16_t dailySteppacketCnt;				//空包计数

static uint16_t stepDataTemp[DAILY_STEP_PACKET_MAX];


static uint16_t dailyStepSaveState;

static uint32_t dailyDataUnuploadLen;			//未上传数据长度
static uint32_t dailyDatauploadTotalLen;		//该目录的总长度
static uint16_t dailyDataCurUploadPackNum;		//当前上传的数据包序号

static uint16_t dailyDataCurUploadClassify;		//当前上传的数据分类
static uint16_t dailyDataCurCatalogUtc;			//当前上传的目录UTC

enum
{
	DATA_SAVE_IDLE		= 0,
	DATA_SAVE_SAVING,
};
// 函数功能:	清空数据缓存
// 输入参数：	无
// 返回参数：	无
static void ClearStepTemp(void)
{
	uint16_t i;
	for(i = 0; i < DAILY_STEP_PACKET_MAX; i++)
		stepDataTemp[i]	= 0;
}


// 函数功能:	初始化计步数据存储
// 输入参数：	无
// 返回参数：	无
void DailyDataSaveInit(void)
{
	dataManage.Init();
	dataManage.DataInit(STEPDATA_CLASSIFY, 2*DM_APP_SECTOR_LENGTH, 5*DM_APP_SECTOR_LENGTH);

	ClearStepTemp();
	dailyStepNullpacketCnt		= 0;
	dailySteppacketCnt			= 0;
	dailyStepSaveState			= DATA_SAVE_IDLE;
}

// 函数功能:	每整5分钟调用一次，自动判定是否需要存储
// 输入参数：	utc:	该5分钟数据的UTC时间
// 				step:	该5分钟内的计步数据。连续1小时（12小包）没有数据时不会进行存储。
// 返回参数：	无
void DailyStep5MinSave(uint32_t utc, uint16_t step)
{
	catalogInfo_s		catalogInfoTemp;


	if(step == 0)
	{
		if(DATA_SAVE_SAVING == dailyStepSaveState)
		{
			stepDataTemp[dailySteppacketCnt]	= step;

			dailyStepNullpacketCnt++;
			dailySteppacketCnt++;
			// 空包数据达到可以不存的程度
			if(DAILY_STEP_NULLPACKET_MAX == dailyStepNullpacketCnt)
			{
				// 将剩余的缓存数据存入
				dataManage.DataSave((uint8_t*)stepDataTemp, DAILY_STEP_BYTE_LEN * dailySteppacketCnt, STEPDATA_CLASSIFY);
				dataManage.DataSaveEnd(STEPDATA_CLASSIFY);
				dailyStepNullpacketCnt		= 0;
				dailySteppacketCnt			= 0;
				return;
			}
			// 达到存储的空包数
			if(DAILY_STEP_PACKET_MAX == dailySteppacketCnt)
			{
				dataManage.DataSave((uint8_t*)stepDataTemp, DAILY_STEP_BYTE_LEN * dailySteppacketCnt, STEPDATA_CLASSIFY);
				dailySteppacketCnt			= 0;
			}
		}
	}
	else
	{
		if(DATA_SAVE_IDLE == dailyStepSaveState)
		{
			catalogInfoTemp.utc				= utc;
			catalogInfoTemp.dataClassify	= STEPDATA_CLASSIFY;
			catalogInfoTemp.sampleUnit		= DATASAMPLE_UNIT_10S;
			catalogInfoTemp.sampleInterval	= 30;
			catalogInfoTemp.unitLength		= DAILY_STEP_BYTE_LEN;
			dataManage.CreateCatalog(&catalogInfoTemp);

			dailyStepSaveState				= DATA_SAVE_SAVING;	
		}

		stepDataTemp[dailySteppacketCnt]	= step;
		dailySteppacketCnt++;

		// 达到存储的空包数
		if(dailySteppacketCnt == DAILY_STEP_PACKET_MAX)
		{
			dataManage.DataSave((uint8_t*)stepDataTemp, DAILY_STEP_BYTE_LEN * dailySteppacketCnt, STEPDATA_CLASSIFY);
			dailySteppacketCnt			= 0;
		}
	}
}

// 函数功能:	将未存入的buf缓存数据,存入数据中，可在APP请求数据时调用。
// 输入参数：	无
// 返回参数：	无
void DailyStepSaveCache(void)
{
	if(DATA_SAVE_SAVING == dailyStepSaveState)
	{
		if(dailySteppacketCnt > 0)
		{
			dataManage.DataSave((uint8_t*)stepDataTemp, DAILY_STEP_BYTE_LEN * dailySteppacketCnt, STEPDATA_CLASSIFY);
			dailySteppacketCnt		= 0;
			dataManage.DataSaveEnd(STEPDATA_CLASSIFY);
			dailyStepSaveState		= DATA_SAVE_IDLE;
		}
	}
}


// 函数功能:	返回请求的数据分类总数据长度与数据目录数量，
// 输入参数：	
// 返回参数：	0x00: 操作成功
// 				0xff: 操作失败
uint16_t DailyDataRequestTotalInfoAck(uint32_t dataClassify)
{
	uint16_t catalogNum, result;
	uint32_t byteLen;

	// 将缓存存入flash
	switch(dataClassify)
	{
		case STEPDATA_CLASSIFY:
		DailyStepSaveCache();
		break;
	}

	result = dataManage.DataInfoRead(&catalogNum, &byteLen, dataClassify);
	

	if(result == 0xff)
	{
		return result;
	}
	else
	{
		// 返回长度与目录数量
		bleMsg.id			= BLE_MSG_SEND;
		bleMsg.u.protocal	= PROT_PACK_TRANS_CATA_TOTAL_RET;

		bleMsg.u.protocal.att.load.content.parameter[0]	= (uint8_t)(dataClassify >> 8);
		bleMsg.u.protocal.att.load.content.parameter[1]	= (uint8_t)(dataClassify);
		bleMsg.u.protocal.att.load.content.parameter[2]	= (uint8_t)(catalogNum >> 8);
		bleMsg.u.protocal.att.load.content.parameter[3]	= (uint8_t)(catalogNum);
		bleMsg.u.protocal.att.load.content.parameter[4]	= (uint8_t)(byteLen >> 24);
		bleMsg.u.protocal.att.load.content.parameter[5]	= (uint8_t)(byteLen >> 16);
		bleMsg.u.protocal.att.load.content.parameter[6]	= (uint8_t)(byteLen >> 8);
		bleMsg.u.protocal.att.load.content.parameter[7]	= (uint8_t)(byteLen);
		ble.EventSet(bleMsg);
	}
	return 0;
}



// 函数功能:	返回对应请求的目录详细信息给APP
// 输入参数：	
// 返回参数：	0x00: 操作成功
// 				0xff: 操作失败
uint16_t DailyStepCatalogInfoRead(uint32_t dataClassify, uint16_t catalogNum)
{
	uint16_t result;
	ble_task_msg_t      bleMsg;
	catalogInfo_s dataInfo;

	result = dataManage.DataCatalogRead(&dataInfo, dataClassify, catalogNum);

	if(result == 0xff)
		return 0xff;
	// 返回对应序号目录详细信息
	bleMsg.id			= BLE_MSG_SEND;
	bleMsg.u.protocal	= PROT_PACK_TRANS_CATA_RET;

	bleMsg.u.protocal.att.load.content.parameter[0]	= (uint8_t)(dataInfo.utc >> 24);
	bleMsg.u.protocal.att.load.content.parameter[1]	= (uint8_t)(dataInfo.utc >> 16);
	bleMsg.u.protocal.att.load.content.parameter[2]	= (uint8_t)(dataInfo.utc >> 8);
	bleMsg.u.protocal.att.load.content.parameter[3]	= (uint8_t)(dataInfo.utc);
	bleMsg.u.protocal.att.load.content.parameter[4]	= (uint8_t)(dataInfo.dataLength >> 8);
	bleMsg.u.protocal.att.load.content.parameter[5]	= (uint8_t)(dataInfo.dataLength);
	bleMsg.u.protocal.att.load.content.parameter[6]	= (uint8_t)((dataInfo.sampleUnit << 4) | (dataInfo.sampleInterval >> 12));
	bleMsg.u.protocal.att.load.content.parameter[7]	= (uint8_t)(dataInfo.sampleInterval);
	bleMsg.u.protocal.att.load.content.parameter[8]	= (uint8_t)(dataInfo.unitLength);
	ble.EventSet(bleMsg);
	return 0;
}


// 函数功能:	请求数据包，查询数据是否有效并反馈给APP，并进入发送数据的机制
// 输入参数：	
// 返回参数：	0x00: 操作成功
// 				0xff: 操作失败
uint16_t DailyStepRequestData(uint32_t dataClassify, uint32_t utc, uint16_t packNum)
{
	uint32_t lengthTemp;
	uint16_t result;
	ble_task_msg_t      bleMsg;

	result = dataManage.CatalogLenRead(&dailyDataUnuploadLen, utc, dataClassify);

	if(result == 0xff)
	{
		// 返回无效数据类型
		bleMsg.id			= BLE_MSG_SEND;
		bleMsg.u.protocal	= PROT_PACK_TRANS_DATA_RET;

		bleMsg.u.protocal.att.load.content.parameter[0]	= 1;
		ble.EventSet(bleMsg);
		return 0xff;
	}

	// 判断获取的数据包是否有效
	dailyDataCurUploadPackNum	= packNum;
	dailyDatauploadTotalLen		= packNum * DAILY_DATA_UPLOAD_MAX_LEN;
	dailyDataCurUploadClassify	= dataClassify;
	dailyDataCurCatalogUtc		= utc;
	// 序号超过数据长度
	if(dailyDatauploadTotalLen >= dailyDataUnuploadLen)
	{
		// 返回无效数据包序号
		bleMsg.id			= BLE_MSG_SEND;
		bleMsg.u.protocal	= PROT_PACK_TRANS_DATA_RET;

		bleMsg.u.protocal.att.load.content.parameter[0]	= 2;
		ble.EventSet(bleMsg);
		return 0xff;
	}

	bleMsg.id			= BLE_MSG_SEND;
	bleMsg.u.protocal	= PROT_PACK_TRANS_DATA_RET;

	bleMsg.u.protocal.att.load.content.parameter[0]	= 0;
	ble.EventSet(bleMsg);


	dailyDataUnuploadLen	= dailyDataUnuploadLen - dailyDatauploadTotalLen;

	if(dailyDataUnuploadLen < DAILY_DATA_UPLOAD_MAX_LEN)
	{
		lengthTemp	= dailyDataUnuploadLen;
	}
	else
	{
		lengthTemp	= DAILY_DATA_UPLOAD_MAX_LEN;
	}

	bleMsg.id			= BLE_MSG_SEND;
	bleMsg.u.protocal	= PROT_PACK_TRANS_DATA_UPLOAD;

	bleMsg.u.protocal.att.load.content.parameter[0]	= (uint8_t)(dailyDataCurUploadPackNum >> 8);
	bleMsg.u.protocal.att.load.content.parameter[1]	= (uint8_t)(dailyDataCurUploadPackNum);
	bleMsg.u.protocal.att.load.content.parameter[2]	= (uint8_t)(dailyDataCurUploadClassify >> 8);
	bleMsg.u.protocal.att.load.content.parameter[3]	= (uint8_t)(dailyDataCurUploadClassify);

	result = dataManage.DataRead((bleMsg.u.protocal.att.load.content.parameter + 4), 
								dailyDatauploadTotalLen, lengthTemp, dailyDataCurCatalogUtc, dailyDataCurUploadClassify);

	// 无效数据
	if(result == 0xff)
	{
		// 返回无效数据包序号
		bleMsg.id			= BLE_MSG_SEND;
		bleMsg.u.protocal	= PROT_PACK_TRANS_DATA_RET;

		bleMsg.u.protocal.att.load.content.parameter[0]	= 2;
		ble.EventSet(bleMsg);
		return 0xff;
	}

	// 发送数据
	ble.EventSet(bleMsg);

	dailyDataCurUploadPackNum++;
	dailyDatauploadTotalLen += lengthTemp;
	dailyDataUnuploadLen	-= lengthTemp;

	return 0x00;
}


// 函数功能:	数据上传处理，当MCU接收到BLE数据上传送达成功时调用该函数
// 输入参数：	
// 返回参数：	0x00: 操作成功
// 				0xff: 操作失败
uint16_t DailyDataUploadProcess(void)
{
	uint16_t result;
	ble_task_msg_t      bleMsg;
	uint32_t lengthTemp;

	// 数据上传完毕
	if(dailyDataUnuploadLen == 0)
		return 0;

	if(dailyDataUnuploadLen < DAILY_DATA_UPLOAD_MAX_LEN)
	{
		lengthTemp	= dailyDataUnuploadLen;
	}
	else
	{
		lengthTemp	= DAILY_DATA_UPLOAD_MAX_LEN;
	}

	bleMsg.id			= BLE_MSG_SEND;
	bleMsg.u.protocal	= PROT_PACK_TRANS_DATA_UPLOAD;

	bleMsg.u.protocal.att.load.content.parameter[0]	= (uint8_t)(dailyDataCurUploadPackNum >> 8);
	bleMsg.u.protocal.att.load.content.parameter[1]	= (uint8_t)(dailyDataCurUploadPackNum);
	bleMsg.u.protocal.att.load.content.parameter[2]	= (uint8_t)(dailyDataCurUploadClassify >> 8);
	bleMsg.u.protocal.att.load.content.parameter[3]	= (uint8_t)(dailyDataCurUploadClassify);

	result = dataManage.DataRead((bleMsg.u.protocal.att.load.content.parameter + 4), 
								dailyDatauploadTotalLen, lengthTemp, dailyDataCurCatalogUtc, dailyDataCurUploadClassify);

	// 无效数据
	if(result == 0xff)
	{
		// 返回无效数据包序号
		bleMsg.id			= BLE_MSG_SEND;
		bleMsg.u.protocal	= PROT_PACK_TRANS_DATA_RET;

		bleMsg.u.protocal.att.load.content.parameter[0]	= 2;
		ble.EventSet(bleMsg);
		return 0xff;
	}

	// 发送数据
	ble.EventSet(bleMsg);

	dailyDataCurUploadPackNum++;
	dailyDatauploadTotalLen += lengthTemp;
	dailyDataUnuploadLen	-= lengthTemp;

	return 0x00;
}

// 函数功能:	删除所有数据,并返回结果
// 输入参数：	
// 返回参数：	0x00: 操作成功
// 				0xff: 操作失败
uint16_t DailyDataTotalDelete(void)
{
	uint16_t result;
	ble_task_msg_t      bleMsg;

	result = dataManage.DeleteTotalData();

	bleMsg.id			= BLE_MSG_SEND;
	bleMsg.u.protocal	= PROT_PACK_TRANS_DELETE_ALL_RET;
	if(result == 0)
	{
		bleMsg.u.protocal.att.load.content.parameter[0]	= 0;
	}
	else
	{
		bleMsg.u.protocal.att.load.content.parameter[0]	= 1;
	}
	ble.EventSet(bleMsg);
	
	return result;
}


// 函数功能:	删除某类分类数据的所有数据,并返回结果
// 输入参数：	
// 返回参数：	0x00: 操作成功
// 				0xff: 操作失败
uint16_t DailyDataDeleteClassify(uint32_t dataClassify)
{
	uint16_t result;
	ble_task_msg_t      bleMsg;

	result = dataManage.DeleteClassifyData(dataClassify);

	bleMsg.id			= BLE_MSG_SEND;
	bleMsg.u.protocal	= PROT_PACK_TRANS_DELETE_CLASS_RET;
	bleMsg.u.protocal.att.load.content.parameter[0]	= (uint8_t)(dataClassify >> 8);
	bleMsg.u.protocal.att.load.content.parameter[1]	= (uint8_t)(dataClassify);

	if(result == 0)
	{
		bleMsg.u.protocal.att.load.content.parameter[2]	= 0;
	}
	else
	{
		bleMsg.u.protocal.att.load.content.parameter[2]	= 1;
	}
	ble.EventSet(bleMsg);
	return result;
}


// 函数功能:	删除指定目录,并返回结果
// 输入参数：	
// 返回参数：	0x00: 操作成功
// 				0xff: 操作失败
uint16_t DailyDataDeleteCatalog(uint32_t dataClassify, uint32_t utc)
{
	uint16_t result;
	ble_task_msg_t      bleMsg;

	result = dataManage.DeleteClassifyDataUtc(dataClassify, utc);

	bleMsg.id			= BLE_MSG_SEND;
	bleMsg.u.protocal	= PROT_PACK_TRANS_DELETE_CATALOG_RET;
	bleMsg.u.protocal.att.load.content.parameter[0]	= (uint8_t)(dataClassify >> 8);
	bleMsg.u.protocal.att.load.content.parameter[1]	= (uint8_t)(dataClassify);
	bleMsg.u.protocal.att.load.content.parameter[2]	= (uint8_t)(utc >> 24);
	bleMsg.u.protocal.att.load.content.parameter[3]	= (uint8_t)(utc >> 16);
	bleMsg.u.protocal.att.load.content.parameter[4]	= (uint8_t)(utc >> 8);
	bleMsg.u.protocal.att.load.content.parameter[5]	= (uint8_t)(utc);

	if(result == 0)
	{
		bleMsg.u.protocal.att.load.content.parameter[6]	= 0;
	}
	else
	{
		bleMsg.u.protocal.att.load.content.parameter[6]	= 1;
	}
	ble.EventSet(bleMsg);

	return result;
}



