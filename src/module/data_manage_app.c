#include "general.h"


static const catalogInfo_u catalogWrite0Data = 
{
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
};


// 数据区域分配管理
typedef struct 
{
	uint32_t	startAddr;			// 数据区域开始地址
	uint32_t	validStartAddr;		// 可进行分配开始地址
	uint32_t	validLength;		// 剩余可分配长度
}data_allocation_s;

static data_allocation_s	catalogAreaManage;
static data_allocation_s	dataAreaManage;

// 数据信息
typedef struct
{
	uint8_t		init;						// 数据是否初始化了		1：已初始化 	0:未初始化
	uint8_t		occupy;						// 是否已新建目录正在存储，且未进行封存。1：占用 0：未占用
	uint8_t		catalogCycle;				// 目录是否已经循环存储。1：循环 0：未循环
	uint8_t		dataCycle;					// 数据是否已经循环存储。1：循环 0：未循环


	uint32_t	catalogAreaStartAddr;		// 目录在外部flash分配区域的开始地址
	uint32_t	catalogAreaEndAddr;			// 目录在外部flash分配区域的结束地址,包含该地址
	uint32_t	catalogWritableStartAddr;	// 目录在外部flash可写开始地址
	uint32_t	catalogWritableLength;		// 目录在外部flash可写长度

	uint32_t	catalogValidStartAddr;		// 目录在外部flash的有效数据开始地址
	uint32_t	catalogValidEndAddr;		// 目录在外部flash的有效数据开始地址,不包含该地址

	uint32_t	dataAreaStartAddr;			// 数据在外部flash分配区域的开始地址
	uint32_t	dataAreaEndAddr;			// 数据在外部flash分配区域的结束地址,包含该地址
	uint32_t	dataWritableStartAddr;		// 数据在外部flash可写开始地址
	uint32_t	dataWritableLength;			// 数据在外部flash可写长度

	uint32_t	dataValidStartAddr;			// 数据在外部flash的有效数据开始地址
	uint32_t	dataValidEndAddr;			// 数据在外部flash的有效数据结束地址,不包含该地址


	catalogInfo_s	catalog;				// 最新开辟的目录信息
}classify_data_info_s;



static classify_data_info_s		dataClassifyInfo[DM_APP_DATA_CLASSIFY_NUM];





typedef struct 
{
	uint8_t			valid;							// 总信息是否有效
	uint16_t		totalCatalog;					// 目录数量
	uint16_t		dataClassify;					// 数据分类

	catalogInfo_s	catalogInfo[CATALOG_TOTAL_MAX_NUM];		// 总数据缓存
	uint32_t		catalogAddr[CATALOG_TOTAL_MAX_NUM];		// 目录所在外部flash位置
}catalog_total_info_s;

static catalog_total_info_s catalogTotalInfo;




// 函数功能:	初始化数据管理模块数据
// 输入参数：	无
// 返回参数：	无
void DataManageInit(void)
{
	uint16_t i;

	catalogAreaManage.startAddr					= DM_APP_CATALOG_AREA_START_ADD;
	catalogAreaManage.validStartAddr			= catalogAreaManage.startAddr;
	catalogAreaManage.validLength				= DM_APP_CATALOG_AREA_LENGTH;

	dataAreaManage.startAddr					= DM_APP_DATA_AREA_START_ADD;
	dataAreaManage.validStartAddr				= dataAreaManage.startAddr;
	dataAreaManage.validLength					= DM_APP_DATA_AREA_LENGTH;

	// 总信息数据初始化，未读取总信息不可进行数据读取
	catalogTotalInfo.valid					= 0;

	for(i = 0; i < DM_APP_DATA_CLASSIFY_NUM; i++)
	{
		dataClassifyInfo[i].init			= 0;
	}
}



// 函数功能:	初始化分类数据的目录与数据区域
// 输入参数：	dataClassify:	数据类型
// 				catalogSize:	目录分配长度，为DM_APP_SECTOR_LENGTH整数倍
// 				dataSize:		数据分配长度，为DM_APP_SECTOR_LENGTH整数倍
// 返回参数：	0x00: 初始化成功
// 				0xff: 区域分配失败
uint16_t ClassifyDataInit(uint32_t dataClassify, uint32_t catalogSize, uint32_t dataSize)
{
	// 未定义数据
	if(dataClassify >= DM_APP_DATA_CLASSIFY_NUM)
		return 0xff;

	// 输入数据不合法
	if((catalogSize % DM_APP_SECTOR_LENGTH) || (dataSize % DM_APP_SECTOR_LENGTH))
		return 0xff;

	// 该类数据已初始化或该类数据分配长度超出
	if( (dataClassifyInfo[dataClassify].init == 1) ||
		(dataSize > dataAreaManage.validLength) ||
		(catalogSize > catalogAreaManage.validLength))
		return 0xff;

	// 目录区域分配 
	dataClassifyInfo[dataClassify].init							= 1;
	dataClassifyInfo[dataClassify].occupy						= 0;
	dataClassifyInfo[dataClassify].catalogCycle					= 0;
	dataClassifyInfo[dataClassify].dataCycle					= 0;

	dataClassifyInfo[dataClassify].catalogAreaStartAddr			= catalogAreaManage.validStartAddr;
	dataClassifyInfo[dataClassify].catalogAreaEndAddr			= dataClassifyInfo[dataClassify].catalogAreaStartAddr + catalogSize - 1;
	dataClassifyInfo[dataClassify].catalogWritableStartAddr		= dataClassifyInfo[dataClassify].catalogAreaStartAddr;
	dataClassifyInfo[dataClassify].catalogWritableLength		= 0;
	dataClassifyInfo[dataClassify].catalogValidStartAddr		= dataClassifyInfo[dataClassify].catalogAreaStartAddr;
	dataClassifyInfo[dataClassify].catalogValidEndAddr			= dataClassifyInfo[dataClassify].catalogAreaStartAddr;

	// 目录分配有效区域更新
	catalogAreaManage.validStartAddr			= catalogAreaManage.validStartAddr + catalogSize;
	catalogAreaManage.validLength				= catalogAreaManage.validLength - catalogSize;

	// 数据区域分配
	dataClassifyInfo[dataClassify].dataAreaStartAddr			= dataAreaManage.validStartAddr;
	dataClassifyInfo[dataClassify].dataAreaEndAddr				= dataClassifyInfo[dataClassify].dataAreaStartAddr + dataSize - 1;
	dataClassifyInfo[dataClassify].dataWritableStartAddr		= dataClassifyInfo[dataClassify].dataAreaStartAddr;
	dataClassifyInfo[dataClassify].dataWritableLength			= 0;
	dataClassifyInfo[dataClassify].dataValidStartAddr			= dataClassifyInfo[dataClassify].dataAreaStartAddr;
	dataClassifyInfo[dataClassify].dataValidEndAddr				= dataClassifyInfo[dataClassify].dataAreaStartAddr;

	// 数据分配有效区域更新
	dataAreaManage.validStartAddr				= dataAreaManage.validStartAddr + dataSize;
	dataAreaManage.validLength					= dataAreaManage.validLength - dataSize;

	return 0x00;
}

// 函数功能:	开辟新目录，并分配外部存储的起始地址，将UTC、数据分类、采样单位、
// 				采样间隔、等写入。若过程中有其他同类数据正在存储则会报错。保存后
// 				未开启新的目录禁止进行存储。
// 输入参数：	dataInfo:	目录信息
// 返回参数：	0x00: 初始化成功
// 				0xff: 目录分配失败
uint16_t CreateCatalog(catalogInfo_s *dataInfo)
{
	catalogInfo_u 			dataInfoTemp;
	extflash_task_msg_t		extflashMsg;
	uint32_t				classifyTemp;
	uint32_t				eraseStartAddr;
	uint32_t				eraseEndAddr;
	uint16_t				result;
	uint16_t				i;


	classifyTemp = dataInfo->dataClassify;
	// 检查数据是否合法
	if(classifyTemp >= DM_APP_DATA_CLASSIFY_NUM)
		return 0xff;

	// 检查是否分配区域了
	if(dataClassifyInfo[classifyTemp].init == 0)
		return 0xff;

	// 检查是否已有新建未完成的目录
	if(dataClassifyInfo[classifyTemp].occupy == 1)
		return 0xff;

	for(i = 0; i < CATALOG_INFO_LENGTH; i++)
	{
		dataInfoTemp.databuf[i] = 0xff;
	}


	dataInfoTemp.u.utc				= dataInfo->utc;
	dataInfoTemp.u.startAddr		= dataClassifyInfo[classifyTemp].dataWritableStartAddr;
	dataInfoTemp.u.dataLength		= 0xffffffff;			//  长度暂不写入，
	dataInfoTemp.u.dataClassify		= classifyTemp;
	dataInfoTemp.u.sampleUnit		= dataInfo->sampleUnit;
	dataInfoTemp.u.sampleInterval	= dataInfo->sampleInterval;
	dataInfoTemp.u.unitLength		= dataInfo->unitLength;


	dataClassifyInfo[classifyTemp].catalog				= dataInfoTemp.u;
	dataClassifyInfo[classifyTemp].catalog.dataLength	= 0;


	// 将目录信息存入外部flash

	// 检查是否需要进行擦除
	if(dataClassifyInfo[classifyTemp].catalogWritableLength < CATALOG_INFO_LENGTH)
	{


		// 检查下一个有效区域是否超出目录范围
		if((dataClassifyInfo[classifyTemp].catalogWritableStartAddr + CATALOG_INFO_LENGTH)
			> dataClassifyInfo[classifyTemp].catalogAreaEndAddr)
		{
			// 跨首尾后，不论尾部剩余多少数据，均进行舍弃。重新赋值可写位置为开始地址
			dataClassifyInfo[classifyTemp].catalogWritableStartAddr = 
					dataClassifyInfo[classifyTemp].catalogAreaStartAddr;


			// 擦除区域
			extflashMsg.startAddr	= dataClassifyInfo[classifyTemp].catalogWritableStartAddr;
			extflashMsg.length		= DM_APP_SECTOR_LENGTH;
			extflashMsg.endAddr		= extflashMsg.startAddr + extflashMsg.length - 1;
			extflashMsg.id			= EXTFLASH_MSG_4K_ERASE;
			extflashMsg.Cb			= NULL;
			extflashMsg.result		= 0;
			extflash.Erase_4K(&extflashMsg);
			result					= extflashMsg.result;
			if(result)
				return 0xff;

			dataClassifyInfo[classifyTemp].catalogCycle				= 1;

			// 跨首尾后，不论尾部剩余多少数据，均进行舍弃。重新赋值可写长度
			dataClassifyInfo[classifyTemp].catalogWritableLength 	= DM_APP_SECTOR_LENGTH;
		}
		else
		{
			// 擦除区域
			extflashMsg.startAddr	= dataClassifyInfo[classifyTemp].catalogWritableStartAddr + dataClassifyInfo[classifyTemp].catalogWritableLength;
			extflashMsg.length		= DM_APP_SECTOR_LENGTH;
			extflashMsg.endAddr		= extflashMsg.startAddr + extflashMsg.length - 1;
			extflashMsg.id			= EXTFLASH_MSG_4K_ERASE;
			extflashMsg.Cb			= NULL;
			extflashMsg.result		= 0;
			extflash.Erase_4K(&extflashMsg);
			result					= extflashMsg.result;

			if(result)
				return 0xff;

			dataClassifyInfo[classifyTemp].catalogWritableLength	+= DM_APP_SECTOR_LENGTH;
		}


		// 进行循环擦除时需要将目录的开始有效地址进行更新
		if(dataClassifyInfo[classifyTemp].catalogCycle == 1)
		{
			// 擦除的开始与结束地址
			eraseStartAddr			= extflashMsg.startAddr - (extflashMsg.startAddr % DM_APP_SECTOR_LENGTH);
			eraseEndAddr			= eraseStartAddr + DM_APP_SECTOR_LENGTH - 1;

			// 目录的有效开始位置是否在本次擦写的区域内
			// 目录数据需片对齐，否则将会出错
			if((dataClassifyInfo[classifyTemp].catalogValidStartAddr >= eraseStartAddr) && 
				(dataClassifyInfo[classifyTemp].catalogValidStartAddr <= eraseEndAddr))
			{
				dataClassifyInfo[classifyTemp].catalogValidStartAddr = eraseEndAddr + 1;
				if(dataClassifyInfo[classifyTemp].catalogValidStartAddr > dataClassifyInfo[classifyTemp].catalogAreaEndAddr)
				{
					dataClassifyInfo[classifyTemp].catalogValidStartAddr = dataClassifyInfo[classifyTemp].catalogAreaStartAddr;
				}
			}

			// 目录的有效结束位置是否在本次擦写的区域内
			// 目录数据需片对齐，否则将会出错
			if((dataClassifyInfo[classifyTemp].catalogAreaStartAddr >= eraseStartAddr) && 
				(dataClassifyInfo[classifyTemp].catalogValidEndAddr <= eraseEndAddr))
			{
				dataClassifyInfo[classifyTemp].catalogValidEndAddr = eraseEndAddr + 1;
				if(dataClassifyInfo[classifyTemp].catalogValidEndAddr > dataClassifyInfo[classifyTemp].catalogAreaEndAddr)
				{
					dataClassifyInfo[classifyTemp].catalogValidEndAddr = dataClassifyInfo[classifyTemp].catalogAreaStartAddr;
				}
			}
		}
	}

	// 写入目录信息至外部flash
	extflashMsg.startAddr	= dataClassifyInfo[classifyTemp].catalogWritableStartAddr;
	extflashMsg.dataAddr	= (uint8_t*)(dataInfoTemp.databuf);
	extflashMsg.length		= CATALOG_INFO_LENGTH;
	extflashMsg.endAddr		= extflashMsg.startAddr + extflashMsg.length - 1;
	extflashMsg.id			= EXTFLASH_MSG_WRITE;
	extflashMsg.Cb			= NULL;

	dataClassifyInfo[classifyTemp].catalogWritableLength	-= CATALOG_INFO_LENGTH;



	extflash.Write(&extflashMsg);

	dataClassifyInfo[classifyTemp].occupy		= 1;

	return 0x00;
}

// 函数功能:	保存目录长度，并更新有效目录的结束位置，解除目录占用
// 输入参数：	dataClassify:	分类数据
// 返回参数：	0x00: 操作成功
// 				0xff: 操作失败
uint16_t DataSaveEnd(uint32_t dataClassify)
{
	catalogInfo_u 			dataInfoTemp;
	extflash_task_msg_t		extflashMsg;
	uint16_t				i;

	// 检查数据是否合法
	if(dataClassify >= DM_APP_DATA_CLASSIFY_NUM)
		return 0xff;

	// 检查是否分配区域了
	if(dataClassifyInfo[dataClassify].init == 0)
		return 0xff;

	// 检查是否未新建目录
	if(dataClassifyInfo[dataClassify].occupy == 0)
		return 0xff;

	for(i = 0; i < CATALOG_INFO_LENGTH; i++)
	{
		dataInfoTemp.databuf[i] = 0xff;
	}

	dataInfoTemp.u				= dataClassifyInfo[dataClassify].catalog;

	// 将长度存入
	extflashMsg.startAddr	= dataClassifyInfo[dataClassify].catalogWritableStartAddr;
	extflashMsg.dataAddr	= (uint8_t*)(dataInfoTemp.databuf);
	extflashMsg.length		= CATALOG_INFO_LENGTH;
	extflashMsg.endAddr		= extflashMsg.startAddr + extflashMsg.length - 1;
	extflashMsg.id			= EXTFLASH_MSG_WRITE;
	extflashMsg.Cb			= NULL;


	extflash.Write(&extflashMsg);

	// 将可写位置往后移动
	dataClassifyInfo[dataClassify].catalogWritableStartAddr	+= CATALOG_INFO_LENGTH;

	// 目录有效结束地址数据位置更新
	dataClassifyInfo[dataClassify].catalogValidEndAddr		= dataClassifyInfo[dataClassify].catalogWritableStartAddr;
	if(dataClassifyInfo[dataClassify].catalogValidEndAddr > dataClassifyInfo[dataClassify].catalogAreaEndAddr)
		dataClassifyInfo[dataClassify].catalogValidEndAddr		= dataClassifyInfo[dataClassify].catalogAreaStartAddr;

	// 数据有效结束地址更新
	dataClassifyInfo[dataClassify].dataValidEndAddr		= dataClassifyInfo[dataClassify].dataWritableStartAddr;
	if(dataClassifyInfo[dataClassify].dataValidEndAddr > dataClassifyInfo[dataClassify].dataAreaEndAddr)
		dataClassifyInfo[dataClassify].dataValidEndAddr		= dataClassifyInfo[dataClassify].dataAreaStartAddr;

	// 解除目录占用
	dataClassifyInfo[dataClassify].occupy		= 0;

	return 0x00;
}



// 函数功能:	根据目录信息转换当前的UTC时间
// 输入参数：	catalogTemp:	目录信息
// 返回参数：	0x00: 操作成功
// 				0xff: 操作失败
static uint32_t UtcTranslation(catalogInfo_s *catalogTemp)
{
	uint32_t utcTemp;
	uint32_t unitNum;

	unitNum		= catalogTemp->dataLength/catalogTemp->unitLength;
	utcTemp		= 0;
	switch(catalogTemp->sampleUnit)
	{
		case DATASAMPLE_UNIT_1US:
		utcTemp		= (unitNum * catalogTemp->sampleInterval) / 1000000;
		break;

		case DATASAMPLE_UNIT_1MS:
		utcTemp		= (unitNum * catalogTemp->sampleInterval) / 1000;
		break;

		case DATASAMPLE_UNIT_1S:
		utcTemp		= (unitNum * catalogTemp->sampleInterval);
		break;

		case DATASAMPLE_UNIT_10US:
		utcTemp		= (unitNum * catalogTemp->sampleInterval) / 100000;
		break;

		case DATASAMPLE_UNIT_10MS:
		utcTemp		= (unitNum * catalogTemp->sampleInterval) / 100;
		break;

		case DATASAMPLE_UNIT_10S:
		utcTemp		= (unitNum * catalogTemp->sampleInterval) * 10;
		break;
	}


	utcTemp += catalogTemp->utc;

	return utcTemp;
}


// 函数功能:	根据分类数据进行数据存储
// 输入参数：	data:			原始数据指针
// 				length:			数据长度，不能超过DM_APP_SECTOR_LENGTH
// 				dataClassify:	分类数据
// 返回参数：	0x00: 操作成功
// 				0xff: 操作失败
uint16_t ClassifyDataSave(uint8_t *data, uint32_t length, uint32_t dataClassify)
{
	extflash_task_msg_t		extflashMsg;
	uint32_t				eraseStartAddr;
	uint32_t				eraseEndAddr;


	catalogInfo_u			catalogTemp;
	uint32_t				dataValidStartAddrTemp;
	uint32_t				dataValidEndAddrTemp;
	uint32_t				readAddrTemp;
	uint16_t				lengthTemp;
	uint16_t				result;

	// 检查数据是否合法
	if(dataClassify >= DM_APP_DATA_CLASSIFY_NUM)
		return 0xff;

	// 单次写入的数据不能超过DM_APP_SECTOR_LENGTH
	if(length > DM_APP_SECTOR_LENGTH)
		return 0xff;

	// 检查是否分配区域了
	if(dataClassifyInfo[dataClassify].init == 0)
		return 0xff;

	// 检查是否未新建目录
	if(dataClassifyInfo[dataClassify].occupy == 0)
		return 0xff;

	lengthTemp				= 0;


	// 检测本次数据是否会超出最大存储量
	if((dataClassifyInfo[dataClassify].catalog.dataLength + length)
		> DATA_MAX_LENGTH)
	{
		// 进行目录封存
		DataSaveEnd(dataClassify);

		// 重新计算utc并新建目录
		dataClassifyInfo[dataClassify].catalog.utc = UtcTranslation(&(dataClassifyInfo[dataClassify].catalog));

		// 重新开辟新目录
		CreateCatalog(&(dataClassifyInfo[dataClassify].catalog));
	}


	// 检查是否需要进行擦除
	if(dataClassifyInfo[dataClassify].dataWritableLength < length)
	{
		// 检查下一个有效区域是否超出目录范围
		if((dataClassifyInfo[dataClassify].dataWritableStartAddr + length)
			> dataClassifyInfo[dataClassify].dataAreaEndAddr)
		{
			// 跨首尾后，将尾部剩余部分写入
			if(dataClassifyInfo[dataClassify].dataWritableLength > 0)
			{
				// 写入最小部分
				extflashMsg.startAddr	= dataClassifyInfo[dataClassify].dataWritableStartAddr;
				extflashMsg.dataAddr	= (uint8_t*)(data);
				extflashMsg.length		= dataClassifyInfo[dataClassify].dataWritableLength;
				extflashMsg.endAddr		= extflashMsg.startAddr + extflashMsg.length - 1;
				extflashMsg.id			= EXTFLASH_MSG_WRITE;
				extflashMsg.Cb			= NULL;

				lengthTemp				= extflashMsg.length;


				dataClassifyInfo[dataClassify].dataWritableLength		-= extflashMsg.length;

				dataClassifyInfo[dataClassify].dataWritableStartAddr	+= extflashMsg.length;

				extflash.Write(&extflashMsg);

				// 将写入的长度加入已经写入的长度中。
				dataClassifyInfo[dataClassify].catalog.dataLength	+= lengthTemp;

				// 将写入的长度减去
				length												-= lengthTemp;
			}

			// 跨首尾后，剩余未写地址小于数据最小单位，尾部进行舍弃
			dataClassifyInfo[dataClassify].dataWritableStartAddr = 
					dataClassifyInfo[dataClassify].dataAreaStartAddr;


			// 擦除区域
			extflashMsg.startAddr	= dataClassifyInfo[dataClassify].dataWritableStartAddr;
			extflashMsg.length		= DM_APP_SECTOR_LENGTH;
			extflashMsg.endAddr		= extflashMsg.startAddr + extflashMsg.length - 1;
			extflashMsg.id			= EXTFLASH_MSG_4K_ERASE;
			extflashMsg.Cb			= NULL;
			extflashMsg.result		= 0;
			extflash.Erase_4K(&extflashMsg);
			result					= extflashMsg.result;

			if(result)
				return 0xff;

			dataClassifyInfo[dataClassify].dataCycle				= 1;

			// 跨首尾后，剩余未写地址小于数据最小单位，尾部进行舍弃
			dataClassifyInfo[dataClassify].dataWritableLength	= DM_APP_SECTOR_LENGTH;
		}
		else
		{


			// 擦除区域
			extflashMsg.startAddr	= dataClassifyInfo[dataClassify].dataWritableStartAddr + dataClassifyInfo[dataClassify].dataWritableLength;
			extflashMsg.length		= DM_APP_SECTOR_LENGTH;
			extflashMsg.endAddr		= extflashMsg.startAddr + extflashMsg.length - 1;
			extflashMsg.id			= EXTFLASH_MSG_4K_ERASE;
			extflashMsg.Cb			= NULL;
			extflashMsg.result		= 0;
			extflash.Erase_4K(&extflashMsg);
			result					= extflashMsg.result;

			if(result)
				return 0xff;

			dataClassifyInfo[dataClassify].dataWritableLength	+= DM_APP_SECTOR_LENGTH;
		}

		// 当循环擦除数据时需要修复目录，与该区域数据相关联的目录需被擦除
		if(dataClassifyInfo[dataClassify].dataCycle == 1)
		{
			// 数据的开始与结束地址
			eraseStartAddr			= extflashMsg.startAddr - (extflashMsg.startAddr % DM_APP_SECTOR_LENGTH);
			eraseEndAddr			= eraseStartAddr + DM_APP_SECTOR_LENGTH - 1;

			// 数据的有效开始位置是否在本次擦写的区域内
			if((dataClassifyInfo[dataClassify].dataValidStartAddr >= eraseStartAddr) && 
				(dataClassifyInfo[dataClassify].dataValidStartAddr <= eraseEndAddr))
			{


				readAddrTemp			= dataClassifyInfo[dataClassify].catalogValidStartAddr;
				// 读取位置与有效的结束位置不一致时，进行持续读取
				while(readAddrTemp != dataClassifyInfo[dataClassify].catalogValidEndAddr)
				{
					extflashMsg.startAddr	= readAddrTemp;
					extflashMsg.length		= CATALOG_INFO_LENGTH;
					extflashMsg.endAddr		= extflashMsg.startAddr + extflashMsg.length - 1;
					extflashMsg.id			= EXTFLASH_MSG_READ;
					extflashMsg.dataAddr	= (uint8_t *)(catalogTemp.databuf);
					extflashMsg.Cb			= NULL;
					extflash.Read(&extflashMsg);

					// 判断数据是否有效
					if((catalogTemp.u.dataClassify == dataClassify) && (catalogTemp.u.dataLength != 0x00) && (catalogTemp.u.dataLength != 0xffffffff))
					{

						dataValidStartAddrTemp		= catalogTemp.u.startAddr;
						dataValidEndAddrTemp		= dataValidStartAddrTemp + catalogTemp.u.dataLength;

						// 数据已进行循环存储
						if(dataValidEndAddrTemp > dataClassifyInfo[dataClassify].dataAreaEndAddr)
							dataValidEndAddrTemp	= dataValidEndAddrTemp - dataClassifyInfo[dataClassify].dataAreaEndAddr
														+ dataClassifyInfo[dataClassify].dataAreaStartAddr;


						// 判断该目录存放数据的开始地址是否该次擦写的地址范围内
						if(((dataValidStartAddrTemp >= eraseStartAddr)
							&& (dataValidStartAddrTemp <= eraseEndAddr)))
						{
							// 写0该目录
							extflashMsg.startAddr	= readAddrTemp;
							extflashMsg.length		= CATALOG_INFO_LENGTH;
							extflashMsg.endAddr		= extflashMsg.startAddr + extflashMsg.length - 1;
							extflashMsg.id			= EXTFLASH_MSG_WRITE;
							extflashMsg.dataAddr	= (uint8_t *)(catalogWrite0Data.databuf);
							extflashMsg.Cb			= NULL;
							extflash.Write(&extflashMsg);


							// 更新有效数据开始地址，由于数据是根据目录索引故不对齐不会出现问题
							dataClassifyInfo[dataClassify].dataValidStartAddr = eraseEndAddr + 1;
							if(dataClassifyInfo[dataClassify].dataValidStartAddr > dataClassifyInfo[dataClassify].dataAreaEndAddr)
							{
								dataClassifyInfo[dataClassify].dataValidStartAddr = dataClassifyInfo[dataClassify].dataAreaStartAddr;
							}

							// 判断数据的有效地址是否也在该次擦写的范围内
							if((dataClassifyInfo[dataClassify].dataValidEndAddr >= eraseStartAddr) && 
								(dataClassifyInfo[dataClassify].dataValidEndAddr <= eraseEndAddr))
							{
								// 更新有效数据结束地址，由于数据是根据目录索引故不对齐不会出现问题
								dataClassifyInfo[dataClassify].catalogValidEndAddr = eraseEndAddr + 1;
								if(dataClassifyInfo[dataClassify].catalogValidEndAddr > dataClassifyInfo[dataClassify].catalogAreaEndAddr)
								{
									dataClassifyInfo[dataClassify].catalogValidEndAddr = dataClassifyInfo[dataClassify].catalogAreaStartAddr;
								}
							}

							if(readAddrTemp == dataClassifyInfo[dataClassify].catalogValidStartAddr)
							{
								// 更新目录有效地址开始地址
								dataClassifyInfo[dataClassify].catalogValidStartAddr		+= CATALOG_INFO_LENGTH;
								if(dataClassifyInfo[dataClassify].catalogValidStartAddr > dataClassifyInfo[dataClassify].catalogAreaEndAddr)
								{
									dataClassifyInfo[dataClassify].catalogValidStartAddr	= dataClassifyInfo[dataClassify].catalogAreaStartAddr;
								}
							}
						}
						else
						{
							// 在范围外，直接跳出，不需要再次检查
							break;
						}

					}
					else
					{
						if(readAddrTemp == dataClassifyInfo[dataClassify].catalogValidStartAddr)
						{
							// 更新目录有效地址开始地址
							dataClassifyInfo[dataClassify].catalogValidStartAddr		+= CATALOG_INFO_LENGTH;
							if(dataClassifyInfo[dataClassify].catalogValidStartAddr > dataClassifyInfo[dataClassify].catalogAreaEndAddr)
							{
								dataClassifyInfo[dataClassify].catalogValidStartAddr	= dataClassifyInfo[dataClassify].catalogAreaStartAddr;
							}
						}
					}


					readAddrTemp += CATALOG_INFO_LENGTH;
					// 超过存储区域从开始地址继续
					if(readAddrTemp > dataClassifyInfo[dataClassify].catalogAreaEndAddr)
						readAddrTemp = dataClassifyInfo[dataClassify].catalogAreaStartAddr;

				}
			}
		}

	}

	

	// 写入数据至外部flash
	extflashMsg.startAddr	= dataClassifyInfo[dataClassify].dataWritableStartAddr;
	extflashMsg.dataAddr	= (uint8_t*)(data + lengthTemp);
	extflashMsg.length		= length;
	extflashMsg.endAddr		= extflashMsg.startAddr + extflashMsg.length - 1;
	extflashMsg.id			= EXTFLASH_MSG_WRITE;
	extflashMsg.Cb			= NULL;

	// 更新数据区域可写的开始地址与长度
	dataClassifyInfo[dataClassify].dataWritableLength		-= length;
	dataClassifyInfo[dataClassify].dataWritableStartAddr	+= length;

	// 更新已经存储的数据长度
	dataClassifyInfo[dataClassify].catalog.dataLength		+= length;

	extflash.Write(&extflashMsg);

	return 0x00;
}

// 函数功能:	根据分类数据进行数据存储
// 输入参数：	catalogTotal:	目录总数指针，处理结束后更新目录总数，最长目录数为16
// 				dataLength:		数据总数指针，处理结束后更新数据长度
// 				dataClassify:	分类数据
// 返回参数：	0x00: 操作成功
// 				0xff: 操作失败
uint16_t ClassifyDataInfoRead(uint16_t *catalogTotal, uint32_t *dataLength, uint32_t dataClassify)
{
	extflash_task_msg_t		extflashMsg;
	catalogInfo_u			catalogTemp;
	uint16_t				catalogTotalNumTemp;
	uint32_t				dataLengthTemp;
	uint32_t				readAddrTemp;
	uint32_t				dataValidStartAddrTemp;
	uint32_t				dataValidEndAddrTemp;

	// 检查数据是否合法
	if(dataClassify >= DM_APP_DATA_CLASSIFY_NUM)
		return 0xff;

	// 检查是否分配区域了
	if(dataClassifyInfo[dataClassify].init == 0)
		return 0xff;

	catalogTotalNumTemp		= 0;
	dataLengthTemp			= 0;


	readAddrTemp			= dataClassifyInfo[dataClassify].catalogValidStartAddr;

	// 读取位置与有效的结束位置不一致时，进行持续读取
	while(readAddrTemp != dataClassifyInfo[dataClassify].catalogValidEndAddr)
	{
		extflashMsg.startAddr	= readAddrTemp;
		extflashMsg.length		= CATALOG_INFO_LENGTH;
		extflashMsg.endAddr		= extflashMsg.startAddr + extflashMsg.length - 1;
		extflashMsg.id			= EXTFLASH_MSG_READ;
		extflashMsg.dataAddr	= (uint8_t *)(catalogTemp.databuf);
		extflashMsg.Cb			= NULL;
		extflash.Read(&extflashMsg);

		// 判断数据是否有效
		if((catalogTemp.u.dataClassify == dataClassify) && (catalogTemp.u.dataLength != 0x00) && (catalogTemp.u.dataLength != 0xffffffff))
		{

			// 判断该目录存放数据的地址是否在有效的数据范围内
			dataValidStartAddrTemp		= catalogTemp.u.startAddr;
			dataValidEndAddrTemp		= dataValidStartAddrTemp + catalogTemp.u.dataLength;

			// 数据已进行循环存储
			if(dataValidEndAddrTemp > dataClassifyInfo[dataClassify].dataAreaEndAddr)
				dataValidEndAddrTemp	= dataValidEndAddrTemp - dataClassifyInfo[dataClassify].dataAreaEndAddr - 1
											+ dataClassifyInfo[dataClassify].dataAreaStartAddr;
			

			// 写0该目录
			extflashMsg.startAddr	= readAddrTemp;
			extflashMsg.length		= CATALOG_INFO_LENGTH;
			extflashMsg.endAddr		= extflashMsg.startAddr + extflashMsg.length - 1;
			extflashMsg.id			= EXTFLASH_MSG_WRITE;
			extflashMsg.dataAddr	= (uint8_t *)(catalogWrite0Data.databuf);
			extflashMsg.Cb			= NULL;

			// 有效数据是否正向
			if(dataClassifyInfo[dataClassify].dataValidEndAddr >= dataClassifyInfo[dataClassify].dataValidStartAddr)
			{
				// 是否在有效数据范围外
				if((dataValidStartAddrTemp < dataClassifyInfo[dataClassify].dataValidStartAddr) ||
					(dataValidStartAddrTemp >= dataClassifyInfo[dataClassify].dataValidEndAddr) ||
					(dataValidEndAddrTemp < dataClassifyInfo[dataClassify].dataValidStartAddr) ||
					(dataValidEndAddrTemp >= dataClassifyInfo[dataClassify].dataValidEndAddr))
				{
					extflash.Write(&extflashMsg);
					if(readAddrTemp == dataClassifyInfo[dataClassify].catalogValidStartAddr)
					{
						// 更新目录有效地址开始地址
						dataClassifyInfo[dataClassify].catalogValidStartAddr		+= CATALOG_INFO_LENGTH;
						if(dataClassifyInfo[dataClassify].catalogValidStartAddr > dataClassifyInfo[dataClassify].catalogAreaEndAddr)
						{
							dataClassifyInfo[dataClassify].catalogValidStartAddr	= dataClassifyInfo[dataClassify].catalogAreaStartAddr;
						}
					}
				}
			}
			else
			{
				// 是否在有效数据范围外
				if(!((dataValidStartAddrTemp < dataClassifyInfo[dataClassify].dataValidEndAddr) ||
					(dataValidStartAddrTemp >= dataClassifyInfo[dataClassify].dataValidStartAddr) ||
					(dataValidEndAddrTemp < dataClassifyInfo[dataClassify].dataValidEndAddr) ||
					(dataValidEndAddrTemp >= dataClassifyInfo[dataClassify].dataValidStartAddr)))
				{
					extflash.Write(&extflashMsg);
					if(readAddrTemp == dataClassifyInfo[dataClassify].catalogValidStartAddr)
					{
						// 更新目录有效地址开始地址
						dataClassifyInfo[dataClassify].catalogValidStartAddr		+= CATALOG_INFO_LENGTH;
						if(dataClassifyInfo[dataClassify].catalogValidStartAddr > dataClassifyInfo[dataClassify].catalogAreaEndAddr)
						{
							dataClassifyInfo[dataClassify].catalogValidStartAddr	= dataClassifyInfo[dataClassify].catalogAreaStartAddr;
						}
					}
				}
			}

			
			catalogTotalInfo.catalogInfo[catalogTotalNumTemp]	= catalogTemp.u;
			catalogTotalInfo.catalogAddr[catalogTotalNumTemp]	= readAddrTemp;

			catalogTotalNumTemp++;
			dataLengthTemp		+= catalogTemp.u.dataLength;



			if(catalogTotalNumTemp >= CATALOG_TOTAL_MAX_NUM)
				break;
		}
		else
		{
			if(readAddrTemp == dataClassifyInfo[dataClassify].catalogValidStartAddr)
			{
				// 更新目录有效地址开始地址
				dataClassifyInfo[dataClassify].catalogValidStartAddr		+= CATALOG_INFO_LENGTH;
				if(dataClassifyInfo[dataClassify].catalogValidStartAddr > dataClassifyInfo[dataClassify].catalogAreaEndAddr)
				{
					dataClassifyInfo[dataClassify].catalogValidStartAddr	= dataClassifyInfo[dataClassify].catalogAreaStartAddr;
				}
			}
		}


		readAddrTemp += CATALOG_INFO_LENGTH;
		// 超过存储区域从开始地址继续
		if(readAddrTemp > dataClassifyInfo[dataClassify].catalogAreaEndAddr)
			readAddrTemp = dataClassifyInfo[dataClassify].catalogAreaStartAddr;

	}


	*catalogTotal		= catalogTotalNumTemp;
	*dataLength			= dataLengthTemp;

	catalogTotalInfo.dataClassify		= dataClassify;
	catalogTotalInfo.totalCatalog		= catalogTotalNumTemp;
	if(catalogTotalNumTemp == 0)
		catalogTotalInfo.valid				= 0;
	else
		catalogTotalInfo.valid				= 1;


	return 0x00;
}


// 函数功能:	根据目录序号读取目录信息
// 输入参数：	dataInfo:		目录信息指针
// 				dataClassify:	分类数据
// 				catalogNum:		目录序号，不能超过总数量，从0开始
// 返回参数：	0x00: 操作成功
// 				0xff: 操作失败
uint16_t ClassifyDataCatalogRead(catalogInfo_s *dataInfo, uint32_t dataClassify, uint16_t catalogNum)
{

	// 数据为加载与总信息缓存
	if(catalogTotalInfo.valid == 0)
		return 0xff;

	// 数据分类与缓存数据不同 
	if(dataClassify != catalogTotalInfo.dataClassify)
		return 0xff;

	// 请求目录序号超出范围
	if(catalogNum >= catalogTotalInfo.totalCatalog)
		return 0xff;

	*dataInfo				= catalogTotalInfo.catalogInfo[catalogNum];

	return 0;
}



// 函数功能:	根据UTC读取数据
// 输入参数：	data:			数据指针
// 				addr:			数据开始相对位置
// 				length:			读取数据长度
// 				dataUtc:		目录UTC指定
// 				dataClassify:	数据分类指定
// 返回参数：	0x00: 操作成功
// 				0xff: 操作失败
uint16_t  ClassifyDataRead(uint8_t *data, uint32_t addr, uint32_t length, uint32_t dataUtc, uint32_t dataClassify)
{
	uint16_t	i;
	uint16_t	hasReadLength;
	uint32_t	addrTemp;
	extflash_task_msg_t		extflashMsg;

	// 数据为加载与总信息缓存
	if(catalogTotalInfo.valid == 0)
		return 0xff;

	// 数据分类与缓存数据不同 
	if(dataClassify != catalogTotalInfo.dataClassify)
		return 0xff;

	hasReadLength		= 0;
	// 查找目录相同的数据
	for(i = 0; i < catalogTotalInfo.totalCatalog; i++)
	{
		if(catalogTotalInfo.catalogInfo[i].utc == dataUtc)
		{
			if((length + addr) <= catalogTotalInfo.catalogInfo[i].dataLength)
			{
				// 
				addrTemp = catalogTotalInfo.catalogInfo[i].startAddr + addr;

				// 数据是否跨结束与开始地址
				if((addrTemp + length) >= dataClassifyInfo[dataClassify].dataAreaEndAddr)
				{
					// 若有数据在尾部，先把尾部的数据读取完
					if(addrTemp < dataClassifyInfo[dataClassify].dataAreaEndAddr)
					{
						extflashMsg.startAddr	= addrTemp;
						extflashMsg.length		= dataClassifyInfo[dataClassify].dataAreaEndAddr - addrTemp + 1;
						extflashMsg.endAddr		= extflashMsg.startAddr + extflashMsg.length - 1;
						extflashMsg.id			= EXTFLASH_MSG_READ;
						extflashMsg.dataAddr	= (uint8_t *)(data);
						extflashMsg.Cb			= NULL;
						extflash.Read(&extflashMsg);

						hasReadLength			= extflashMsg.length;
					}
					// 跨首尾的数据结束地址,不包含该地址
					addrTemp				= addrTemp + length - dataClassifyInfo[dataClassify].dataAreaEndAddr 
												+ dataClassifyInfo[dataClassify].dataAreaStartAddr - 1;

					extflashMsg.startAddr	= addrTemp - (length - hasReadLength);
					extflashMsg.length		= length - hasReadLength;
					extflashMsg.endAddr		= extflashMsg.startAddr + extflashMsg.length - 1;
					extflashMsg.id			= EXTFLASH_MSG_READ;
					extflashMsg.dataAddr	= (uint8_t *)(data + hasReadLength);
					extflashMsg.Cb			= NULL;
					extflash.Read(&extflashMsg);
				}
				else
				{
					extflashMsg.startAddr	= addrTemp;
					extflashMsg.length		= length;
					extflashMsg.endAddr		= extflashMsg.startAddr + extflashMsg.length - 1;
					extflashMsg.id			= EXTFLASH_MSG_READ;
					extflashMsg.dataAddr	= (uint8_t *)(data);
					extflashMsg.Cb			= NULL;
					extflash.Read(&extflashMsg);
				}
				break;
			}
			else
			{
				// 读取数据超出范围
				return 0xff;
			}
		}
	}
	if(i >= catalogTotalInfo.totalCatalog)
		return 0xff;

	return 0x00;
}

// 函数功能:	根据UTC读取数据
// 输入参数：	data:			数据指针
// 				addr:			数据开始相对位置
// 				length:			读取数据长度
// 				dataUtc:		目录UTC指定
// 				dataClassify:	数据分类指定
// 返回参数：	0x00: 操作成功
// 				0xff: 操作失败
uint16_t ReadCatalogDataLen(uint32_t *length, uint32_t dataUtc, uint32_t dataClassify)
{
	uint16_t i;

	// 数据未加载于总信息缓存
	if(catalogTotalInfo.valid == 0)
		return 0xff;

	// 数据分类与缓存数据不同 
	if(dataClassify != catalogTotalInfo.dataClassify)
		return 0xff;

	for(i = 0; i < catalogTotalInfo.totalCatalog; i++)
	{
		if(catalogTotalInfo.catalogInfo[i].utc == dataUtc)
		{
			*length			= catalogTotalInfo.catalogInfo[i].dataLength;
			return 0x00;
		}

	}
	// 未查找到相关目录
	return 0xff;
}


// 函数功能:	删除所有数据，包括未存储的
// 输入参数：	无
// 返回参数：	0x00: 操作成功
// 				0xff: 操作失败
uint16_t DeleteTotalData(void)
{
	uint16_t i;

	for(i = 0; i < DM_APP_DATA_CLASSIFY_NUM; i++)
	{
		if(dataClassifyInfo[i].init == 1)
		{
			// 目录信息初始化
			dataClassifyInfo[i].occupy						= 0;
			dataClassifyInfo[i].catalogCycle				= 0;
			dataClassifyInfo[i].dataCycle					= 0;

			dataClassifyInfo[i].catalogWritableStartAddr	= dataClassifyInfo[i].catalogAreaStartAddr;
			dataClassifyInfo[i].catalogWritableLength		= 0;
			dataClassifyInfo[i].catalogValidStartAddr		= dataClassifyInfo[i].catalogAreaStartAddr;
			dataClassifyInfo[i].catalogValidEndAddr			= dataClassifyInfo[i].catalogAreaStartAddr;


			// 数据区域分配
			dataClassifyInfo[i].dataWritableStartAddr		= dataClassifyInfo[i].dataAreaStartAddr;
			dataClassifyInfo[i].dataWritableLength			= 0;
			dataClassifyInfo[i].dataValidStartAddr			= dataClassifyInfo[i].dataAreaStartAddr;
			dataClassifyInfo[i].dataValidEndAddr			= dataClassifyInfo[i].dataAreaStartAddr;
		}
	}

	catalogTotalInfo.valid					= 0;
	return 0;
}



// 函数功能:	删除某类数据
// 输入参数：	dataClassify：	分类数据
// 返回参数：	0x00: 操作成功
// 				0xff: 操作失败
uint16_t DeleteClassifyData(uint32_t dataClassify)
{

	if(dataClassifyInfo[dataClassify].init == 1)
	{
		dataClassifyInfo[dataClassify].occupy						= 0;
		dataClassifyInfo[dataClassify].catalogCycle					= 0;
		dataClassifyInfo[dataClassify].dataCycle					= 0;

		dataClassifyInfo[dataClassify].catalogWritableStartAddr		= dataClassifyInfo[dataClassify].catalogAreaStartAddr;
		dataClassifyInfo[dataClassify].catalogWritableLength		= 0;
		dataClassifyInfo[dataClassify].catalogValidStartAddr		= dataClassifyInfo[dataClassify].catalogAreaStartAddr;
		dataClassifyInfo[dataClassify].catalogValidEndAddr			= dataClassifyInfo[dataClassify].catalogAreaStartAddr;


		// 数据区域分配
		dataClassifyInfo[dataClassify].dataWritableStartAddr		= dataClassifyInfo[dataClassify].dataAreaStartAddr;
		dataClassifyInfo[dataClassify].dataWritableLength			= 0;
		dataClassifyInfo[dataClassify].dataValidStartAddr			= dataClassifyInfo[dataClassify].dataAreaStartAddr;
		dataClassifyInfo[dataClassify].dataValidEndAddr				= dataClassifyInfo[dataClassify].dataAreaStartAddr;
	}
	else
	{
		return 0xff;
	}

	// 清除该数据缓存
	if(catalogTotalInfo.dataClassify == dataClassify)
		catalogTotalInfo.valid					= 0;

	return 0;
}

// 函数功能:	删除指定目录
// 输入参数：	dataClassify:	分类数据
// 				utc:			目录UTC
// 返回参数：	0x00: 操作成功
// 				0xff: 操作失败
uint16_t  DeleteClassifyDataUtc(uint32_t dataClassify, uint32_t utc)
{
	uint16_t i;
	extflash_task_msg_t		extflashMsg;

	// 缓存是否放置了该类数据
	if((catalogTotalInfo.dataClassify == dataClassify) && (catalogTotalInfo.valid == 1))
	{
		for(i = 0; i < catalogTotalInfo.totalCatalog; i++)
		{
			if(catalogTotalInfo.catalogInfo[i].utc == utc)
			{
				extflashMsg.startAddr	= catalogTotalInfo.catalogAddr[i];
				extflashMsg.length		= CATALOG_INFO_LENGTH;
				extflashMsg.endAddr		= extflashMsg.startAddr + extflashMsg.length - 1;
				extflashMsg.id			= EXTFLASH_MSG_WRITE;
				extflashMsg.dataAddr	= (uint8_t *)(catalogWrite0Data.databuf);
				extflashMsg.Cb			= NULL;
				extflash.Write(&extflashMsg);

				return 0;
			}
		}
	}

	// 缓存中为有相同的目录
	return 0xff;
}


// 函数功能:	由于不正常原因掉电，数据恢复前需要先分配区域，按照分配的区域进行数据恢复
// 				数据存储区域若掉电前和掉电后分配区域有不会则会造成恢复数据错误，升级可做
// 				正常的数据恢复，不调用该函数。
// 输入参数：	none
// 返回参数：	0x00: 操作成功
// 				0xff: 操作失败
uint16_t StorageDataRecover(void)
{
	uint16_t i;
	uint32_t addrTemp;
	uint32_t earlyCatalogAddr;
	uint32_t lastCatalogAddr;
	uint32_t earlyDataAddr;
	uint32_t lastDataAddr;
	extflash_task_msg_t		extflashMsg;

	catalogInfo_s	earlyCatalog;
	catalogInfo_s	lastCatalog;
	catalogInfo_u 	catalogTemp;



	

	// 区域未进行分配
	if(catalogAreaManage.validLength == DM_APP_CATALOG_AREA_LENGTH)
		return 0xff;

	for(i = 0; i < DM_APP_DATA_CLASSIFY_NUM; i++)
	{
		lastCatalog.utc		= 0;
		earlyCatalog.utc	= 0xffffffff;
		if(dataClassifyInfo[i].init == 1)
		{
			addrTemp		= dataClassifyInfo[i].catalogAreaStartAddr;
			while(addrTemp < dataClassifyInfo[i].catalogAreaEndAddr)
			{
				extflashMsg.startAddr	= addrTemp;
				extflashMsg.length		= CATALOG_INFO_LENGTH;
				extflashMsg.endAddr		= extflashMsg.startAddr + extflashMsg.length - 1;
				extflashMsg.id			= EXTFLASH_MSG_READ;
				extflashMsg.dataAddr	= (uint8_t *)(catalogTemp.databuf);
				extflashMsg.Cb			= NULL;
				extflash.Read(&extflashMsg);

				// 判断存储的数据是否有效
				if((catalogTemp.u.dataClassify == i) && ((catalogTemp.u.dataLength != 0) || (catalogTemp.u.dataLength == 0xffffffff)))
				{
					if(lastCatalog.utc < catalogTemp.u.utc)
					{
						lastCatalog		= catalogTemp.u;
						lastCatalogAddr	= addrTemp;
					}
					if(earlyCatalog.utc > catalogTemp.u.utc)
					{
						earlyCatalog		= catalogTemp.u;
						earlyCatalogAddr	= addrTemp;
					}
				}
				else
				{
					// 数据类型不同，无需继续查找
					break;
				}
				addrTemp += CATALOG_INFO_LENGTH;
			}

			if(lastCatalog.utc == 0)
			{
				// 无数据可进行恢复
			}
			else
			{
				// 目录区域恢复
				if(lastCatalogAddr >= earlyCatalogAddr)
				{
					// 未跨结束与开始地址
					dataClassifyInfo[i].catalogCycle	= 0;

					dataClassifyInfo[i].catalogValidStartAddr	= earlyCatalogAddr;

					dataClassifyInfo[i].catalogValidEndAddr		= lastCatalogAddr + CATALOG_INFO_LENGTH;

					// 有效目录地址越界
					if(dataClassifyInfo[i].catalogValidEndAddr > dataClassifyInfo[i].catalogAreaEndAddr)
						dataClassifyInfo[i].catalogValidEndAddr		= dataClassifyInfo[i].catalogAreaStartAddr;

					// 目录结束位置是否刚好为块开始地址
					if((dataClassifyInfo[i].catalogValidEndAddr % DM_APP_SECTOR_LENGTH) == 0)
					{
						dataClassifyInfo[i].catalogWritableStartAddr	= dataClassifyInfo[i].catalogValidEndAddr;
						dataClassifyInfo[i].catalogWritableLength	= 0;
					}
					else
					{
						// 丢弃下一个目录，可能目录不完整,可进行部分恢复，根据需求做修改
						dataClassifyInfo[i].catalogWritableStartAddr	= dataClassifyInfo[i].catalogValidEndAddr + CATALOG_INFO_LENGTH;

						if(dataClassifyInfo[i].catalogWritableStartAddr > dataClassifyInfo[i].catalogAreaEndAddr)
							dataClassifyInfo[i].catalogWritableStartAddr		= dataClassifyInfo[i].catalogAreaStartAddr;

						// 是否刚好跨块
						if((dataClassifyInfo[i].catalogWritableStartAddr % DM_APP_SECTOR_LENGTH) == 0)
						{
							dataClassifyInfo[i].catalogWritableLength	= 0;
						}
						else
						{
							dataClassifyInfo[i].catalogWritableLength = 
								DM_APP_SECTOR_LENGTH - (dataClassifyInfo[i].catalogWritableStartAddr%DM_APP_SECTOR_LENGTH);
						}

					}
				}
				else
				{
					// 跨结束与开始地址
					dataClassifyInfo[i].catalogCycle	= 1;

					dataClassifyInfo[i].catalogValidStartAddr	= earlyCatalogAddr;

					dataClassifyInfo[i].catalogValidEndAddr		= lastCatalogAddr + CATALOG_INFO_LENGTH;

					// 目录结束位置是否刚好为块开始地址
					if((dataClassifyInfo[i].catalogValidEndAddr % DM_APP_SECTOR_LENGTH) == 0)
					{
						dataClassifyInfo[i].catalogWritableStartAddr	= dataClassifyInfo[i].catalogValidEndAddr;
						dataClassifyInfo[i].catalogWritableLength	= 0;
					}
					else
					{
						// 丢弃下一个目录，可能目录不完整,可进行部分恢复，根据需求做修改
						dataClassifyInfo[i].catalogWritableStartAddr	= dataClassifyInfo[i].catalogValidEndAddr + CATALOG_INFO_LENGTH;


						dataClassifyInfo[i].catalogWritableLength = 
							DM_APP_SECTOR_LENGTH - (dataClassifyInfo[i].catalogWritableStartAddr%DM_APP_SECTOR_LENGTH);

					}
				}

				// 数据区域恢复
				lastDataAddr	= lastCatalog.startAddr;
				earlyDataAddr	= earlyCatalog.startAddr + earlyCatalog.dataLength;

				if(lastDataAddr > dataClassifyInfo[i].dataAreaEndAddr)
					lastDataAddr -= dataClassifyInfo[i].dataAreaEndAddr + dataClassifyInfo[i].dataAreaStartAddr;

				if(lastDataAddr >= earlyDataAddr)
				{
					// 未跨开始与结束地址
					dataClassifyInfo[i].dataCycle	= 0;
				}
				else
				{
					// 跨开始与结束地址
					dataClassifyInfo[i].dataCycle	= 1;
				}

				dataClassifyInfo[i].dataValidStartAddr	= earlyDataAddr;
				dataClassifyInfo[i].dataValidEndAddr	= lastDataAddr;

				// 该片区为写区域均丢弃
				dataClassifyInfo[i].dataWritableStartAddr	= 
					lastDataAddr - (lastDataAddr % DM_APP_SECTOR_LENGTH) + DM_APP_SECTOR_LENGTH;

				if(dataClassifyInfo[i].dataWritableStartAddr > dataClassifyInfo[i].dataAreaEndAddr)
					dataClassifyInfo[i].dataWritableStartAddr	= dataClassifyInfo[i].dataAreaStartAddr;

				dataClassifyInfo[i].dataWritableLength		= 0;
			}
		}
	}
	return 0;
}



void DataManageTest(void)
{
	uint32_t	i;
	uint32_t	j;
	uint32_t	k;
	uint32_t	m;
	uint32_t	n;
	uint16_t	catalogNumTemp;
	uint32_t	dataLengthTemp;
	uint8_t		datatemp[32];
	uint8_t		a, b;
	uint16_t	result;
	uint8_t		testFlag;
	uint8_t		saveLength;
	catalogInfo_s		testCatalog;
	catalogInfo_s		testCatalogRead;
	testFlag	= 0;
	uint32_t	g;
	m = 100;
	n = 4;
	a = 2;
	b = 10;
	saveLength = 31;
	DataManageInit();

	ClassifyDataInit(STEPDATA_CLASSIFY, DM_APP_SECTOR_LENGTH * a, DM_APP_SECTOR_LENGTH * b);

	if(testFlag)
		StorageDataRecover();
	result			= 0;
	// 写入测试
	for(i = 0; i < n; i++)
	{
		testCatalog.utc				= 3600 * (i+1);
		g							= testCatalog.utc/3600;
		testCatalog.dataClassify	= STEPDATA_CLASSIFY;
		testCatalog.sampleUnit		= DATASAMPLE_UNIT_10S;
		testCatalog.sampleInterval	= 30;
		testCatalog.unitLength		= 4;

		CreateCatalog(& testCatalog);

		for(j = 0; j < m; j++)
		{
			for(k = 0; k < saveLength; k++)
			{
				datatemp[k] = j+k+g;
			}

			ClassifyDataSave(datatemp, saveLength, STEPDATA_CLASSIFY);
		}
		DataSaveEnd(STEPDATA_CLASSIFY);
	}

	// 读取测试
	// ClassifyDataInfoRead(&catalogNumTemp, &dataLengthTemp, STEPDATA_CLASSIFY);


	// for(i = 0; i < n; i++)
	// {
	// 	ClassifyDataCatalogRead(&testCatalogRead, STEPDATA_CLASSIFY, i);
		
	// 	g		= testCatalogRead.utc/100;
	// 	if(testCatalogRead.utc != (g)*100)
	// 	{
	// 		result++;
	// 	}

	// 	for(j = 0; j < m; j++)
	// 	{

	// 		ClassifyDataRead(datatemp, j*saveLength, saveLength, testCatalogRead.utc, STEPDATA_CLASSIFY);
	// 		for(k = 0; k < saveLength; k++)
	// 		{
	// 			if(datatemp[k] != (j+k+g))
	// 				result++;
	// 		}
	// 	}

	// 	DeleteClassifyDataUtc(STEPDATA_CLASSIFY, testCatalogRead.utc);
	// }
}

// // dataManage
const dataManage_s dataManage =
{
	.Init						= DataManageInit,
	.DataInit					= ClassifyDataInit,
	.CreateCatalog				= CreateCatalog,
	.DataSaveEnd				= DataSaveEnd,
	.DataSave					= ClassifyDataSave,
	.DataInfoRead				= ClassifyDataInfoRead,
	.DataCatalogRead			= ClassifyDataCatalogRead,
	.DataRead					= ClassifyDataRead,
	.CatalogLenRead				= ReadCatalogDataLen,
	.DeleteTotalData			= DeleteTotalData,
	.DeleteClassifyData			= DeleteClassifyData,
	.DeleteClassifyDataUtc		= DeleteClassifyDataUtc,
	.StorageDataRecover			= StorageDataRecover,
};
