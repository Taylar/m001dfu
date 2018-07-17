#include "general.h"





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
static uint32_t dailyDataCurCatalogUtc;			//当前上传的目录UTC

uint8_t			dataUploadFlag;

uint8_t			dailyDataSendBug[20];

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
	dataUploadFlag				= DATA_UNUPLOADING;

	dataManage.StorageDataRecover();
}

// 函数功能:	每整5分钟调用一次，自动判定是否需要存储
// 输入参数：	utc:	开始的UTC时间
// 				step:	该时间段内的计步数据。连续1小时（12小包）没有数据时不会进行存储。
// 返回参数：	无
void DailyStepSave(uint32_t utc, uint16_t step)
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
		bleSendMsg.load[0] = HDATA_TOTAL_INFO;
		bleSendMsg.load[1] = (uint8_t)(dataClassify >> 8);
		bleSendMsg.load[2] = (uint8_t)(dataClassify);
		bleSendMsg.load[3] = (uint8_t)(catalogNum >> 8);
		bleSendMsg.load[4] = (uint8_t)(catalogNum);
		bleSendMsg.load[5] = (uint8_t)(byteLen >> 24);
		bleSendMsg.load[6] = (uint8_t)(byteLen >> 16);
		bleSendMsg.load[7] = (uint8_t)(byteLen >> 8);
		bleSendMsg.load[8] = (uint8_t)(byteLen);
		bleSendMsg.length  = 9;
		BlePack(DEVICE_HIS_DATA_INFO, &bleSendMsg);
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
	catalogInfo_s dataInfo;

	result = dataManage.DataCatalogRead(&dataInfo, dataClassify, catalogNum);

	if(result == 0xff)
		return 0xff;
	// 返回对应序号目录详细信息
	bleSendMsg.load[0] = HDATA_CATA_INFO;
	bleSendMsg.load[1] = (uint8_t)(dataInfo.utc >> 24);
	bleSendMsg.load[2] = (uint8_t)(dataInfo.utc >> 16);
	bleSendMsg.load[3] = (uint8_t)(dataInfo.utc >> 8);
	bleSendMsg.load[4] = (uint8_t)(dataInfo.utc);
	bleSendMsg.load[5] = (uint8_t)(dataInfo.dataLength >> 8);
	bleSendMsg.load[6] = (uint8_t)(dataInfo.dataLength);
	bleSendMsg.length  = 7;
	BlePack(DEVICE_HIS_DATA_INFO, &bleSendMsg);

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


	result = dataManage.CatalogLenRead(&dailyDataUnuploadLen, utc, dataClassify);

	if(result == 0xff)
	{
		// 返回无效数据类型
		bleSendMsg.load[0] = HDATA_CATA_DATA;
		bleSendMsg.load[1] = 0x01;
		bleSendMsg.length  = 2;
		BlePack(DEVICE_HIS_DATA_INFO, &bleSendMsg);
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
		bleSendMsg.load[0] = HDATA_CATA_DATA;
		bleSendMsg.load[1] = 0x03;
		bleSendMsg.length  = 2;
		BlePack(DEVICE_HIS_DATA_INFO, &bleSendMsg);
		return 0xff;
	}

	bleSendMsg.load[0] = HDATA_CATA_DATA;
	bleSendMsg.load[1] = 0;
	bleSendMsg.length  = 2;
	BlePack(DEVICE_HIS_DATA_INFO, &bleSendMsg);
	

	dailyDataUnuploadLen	= dailyDataUnuploadLen - dailyDatauploadTotalLen;

	if(dailyDataUnuploadLen < DAILY_DATA_UPLOAD_MAX_LEN)
	{
		lengthTemp	= dailyDataUnuploadLen;
	}
	else
	{
		lengthTemp	= DAILY_DATA_UPLOAD_MAX_LEN;
	}

	//bleSendMsg.load[0] = HDATA_CATA_INFO;
	dailyDataSendBug[0] = (uint8_t)(dailyDataCurUploadPackNum >> 8);
	dailyDataSendBug[1] = (uint8_t)(dailyDataCurUploadPackNum);
	dailyDataSendBug[2] = (uint8_t)(dailyDataCurUploadClassify >> 8);
	dailyDataSendBug[3] = (uint8_t)(dailyDataCurUploadClassify);
	

	result = dataManage.DataRead((dailyDataSendBug + 4), 
								dailyDatauploadTotalLen, lengthTemp, dailyDataCurCatalogUtc, dailyDataCurUploadClassify);

	// 无效数据
	if(result == 0xff)
	{
		// 返回无效数据包序号
		bleSendMsg.load[0] = HDATA_CATA_DATA;
		bleSendMsg.load[1] = 0x03;
		bleSendMsg.length  = 2;
		BlePack(DEVICE_HIS_DATA_INFO, &bleSendMsg);
		return 0xff;
	}

	// 发送数据
	Ble_ldtHandleSend(dailyDataSendBug, 20);

	dailyDataCurUploadPackNum++;
	dailyDatauploadTotalLen += lengthTemp;
	dailyDataUnuploadLen	-= lengthTemp;

	dataUploadFlag = DATA_UPLOADING;

	return 0x00;
}


// 函数功能:	数据上传处理，当MCU接收到BLE数据上传送达成功时调用该函数
// 输入参数：	
// 返回参数：	0x00: 操作成功
// 				0xff: 操作失败
uint16_t DailyDataUploadProcess(void)
{
	uint16_t result;

	uint32_t lengthTemp;

	if(dataUploadFlag == DATA_UNUPLOADING)
		return 0x00;

	// 数据上传完毕
	if(dailyDataUnuploadLen == 0)
	{
		dataUploadFlag = DATA_UNUPLOADING;
		return 0;
	}

	if(dailyDataUnuploadLen < DAILY_DATA_UPLOAD_MAX_LEN)
	{
		lengthTemp	= dailyDataUnuploadLen;
	}
	else
	{
		lengthTemp	= DAILY_DATA_UPLOAD_MAX_LEN;
	}

	bleSendMsg.load[0] = (uint8_t)(dailyDataCurUploadPackNum >> 8);
	bleSendMsg.load[1] = (uint8_t)(dailyDataCurUploadPackNum);
	bleSendMsg.load[2] = (uint8_t)(dailyDataCurUploadClassify >> 8);
	bleSendMsg.load[3] = (uint8_t)(dailyDataCurUploadClassify);
	bleSendMsg.length  = 20;

	result = dataManage.DataRead((bleSendMsg.load + 4), 
								dailyDatauploadTotalLen, lengthTemp, dailyDataCurCatalogUtc, dailyDataCurUploadClassify);

	// 无效数据
	if(result == 0xff)
	{
		// 返回无效数据包序号
		bleSendMsg.load[0] = HDATA_CATA_DATA;
		bleSendMsg.load[1] = 0x03;
		bleSendMsg.length  = 2;
		BlePack(DEVICE_HIS_DATA_INFO, &bleSendMsg);
		return 0xff;
	}

	// 发送数据
	Ble_ldtHandleSend(bleSendMsg.load, 20);

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
	

	result = dataManage.DeleteTotalData();

	bleSendMsg.load[0] = HDATA_DEL_ALL;
	bleSendMsg.length  = 2;
	
	if(result == 0)
	{
			bleSendMsg.load[1] = 0;
	}
	else
	{
		bleSendMsg.load[1] = 1;
	}
	BlePack(DEVICE_HIS_DATA_INFO, &bleSendMsg);
	
	return result;
}


// 函数功能:	删除某类分类数据的所有数据,并返回结果
// 输入参数：	
// 返回参数：	0x00: 操作成功
// 				0xff: 操作失败
uint16_t DailyDataDeleteClassify(uint32_t dataClassify)
{
	uint16_t result;


	result = dataManage.DeleteClassifyData(dataClassify);

	
	bleSendMsg.load[0] = HDATA_DEL_TYPE;
	bleSendMsg.load[1] = (uint8_t)(dataClassify >> 8);
	bleSendMsg.load[2] = (uint8_t)(dataClassify);
	
	bleSendMsg.length  = 4;


	if(result == 0)
	{
		bleSendMsg.load[3] = 0;
	}
	else
	{
		bleSendMsg.load[3] = 1;
	}
	BlePack(DEVICE_HIS_DATA_INFO, &bleSendMsg);
	return result;
}


// 函数功能:	删除指定目录,并返回结果
// 输入参数：	
// 返回参数：	0x00: 操作成功
// 				0xff: 操作失败
uint16_t DailyDataDeleteCatalog(uint32_t dataClassify, uint32_t utc)
{
	uint16_t result;

	result = dataManage.DeleteClassifyDataUtc(dataClassify, utc);

	bleSendMsg.load[0] = HDATA_DEL_TYPE;
	bleSendMsg.load[1] = (uint8_t)(dataClassify >> 8);
	bleSendMsg.load[2] = (uint8_t)(dataClassify);
	bleSendMsg.load[3] = (uint8_t)(utc >> 24);
	bleSendMsg.load[4] = (uint8_t)(utc >> 16);
	bleSendMsg.load[5] = (uint8_t)(utc >> 8);
	bleSendMsg.load[6] = (uint8_t)(utc);
	bleSendMsg.length  = 8;
	

	if(result == 0)
	{
		bleSendMsg.load[6]	= 0;
	}
	else
	{
		bleSendMsg.load[6]	= 1;
	}
	BlePack(DEVICE_HIS_DATA_INFO, &bleSendMsg);

	return result;
}



