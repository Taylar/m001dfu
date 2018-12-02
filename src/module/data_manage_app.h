#ifndef		DATA_MANAGE_APP_H
#define		DATA_MANAGE_APP_H

// 外部flash存储空间分配
// 目录区域分配
#define		DM_APP_CATALOG_AREA_START_ADD			0x00000
#define		DM_APP_CATALOG_AREA_LENGTH				0x4000

// 数据区域分配
#define		DM_APP_DATA_AREA_START_ADD				(DM_APP_CATALOG_AREA_START_ADD + DM_APP_CATALOG_AREA_LENGTH)
#define		DM_APP_DATA_AREA_LENGTH					0xA000


// 数据最小片区长度，一次性写入的最大字节数据，超过会报错
#define		DM_APP_SECTOR_LENGTH					4096


// 数据分量总量
#define		DM_APP_DATA_CLASSIFY_NUM			2


// 目录数据需片区对齐，否则会出错
#define			CATALOG_INFO_LENGTH		32


// 数据分类
typedef enum
{
	STEPDATA_CLASSIFY		= 0x00,
	SLEEPDATA_CLASSIFY,
} data_classify_e;


// 采样单位
typedef enum
{
	DATASAMPLE_UNIT_1US		= 1,
	DATASAMPLE_UNIT_1MS,
	DATASAMPLE_UNIT_1S,
	DATASAMPLE_UNIT_10US,
	DATASAMPLE_UNIT_10MS,
	DATASAMPLE_UNIT_10S,
}data_sample_uint_e;


// 目录信息
typedef struct
{
	uint32_t		utc;				// 运动数据开始时间
	uint32_t		startAddr;			// 存储在外部flash开始地址
	uint32_t		dataLength;			// 运动数据总长度
	uint32_t		dataClassify;		// 数据分类
	uint16_t		sampleUnit;			// 数据采样单位
	uint16_t		sampleInterval;		// 数据采样间隔
	uint8_t			unitLength;			// 数据最小数据单位长度
}catalogInfo_s;

typedef union
{
	uint8_t			databuf[CATALOG_INFO_LENGTH];
	catalogInfo_s	u;	
}catalogInfo_u;


// 数据最长存储量，超过需另起目录
#define			DATA_MAX_LENGTH			65535


// 总数据缓存目录最大数量限制
#define			CATALOG_TOTAL_MAX_NUM	16



void DataManageInit(void);

uint16_t ClassifyDataInit(uint32_t dataClassify, uint32_t catalogSize, uint32_t dataSize);

uint16_t CreateCatalog(catalogInfo_s *dataInfo);

uint16_t DataSaveEnd(uint32_t dataClassify);

uint16_t ClassifyDataSave(uint8_t *data, uint32_t length, uint32_t dataClassify);

uint16_t ClassifyDataInfoRead(uint16_t *catalogTotal, uint32_t *dataLength, uint32_t dataClassify);

uint16_t ClassifyDataCatalogRead(catalogInfo_s *dataInfo, uint32_t dataClassify, uint16_t catalogNum);

uint16_t  ClassifyDataRead(uint8_t *data, uint32_t addr, uint32_t length, uint32_t dataUtc, uint32_t dataClassify);

uint16_t ReadCatalogDataLen(uint32_t *length, uint32_t dataUtc, uint32_t dataClassify);

uint16_t DeleteTotalData(void);

uint16_t DeleteClassifyData(uint32_t dataClassify);

uint16_t DeleteClassifyDataUtc(uint32_t dataClassify, uint32_t utc);

uint16_t StorageDataRecover(void);


typedef struct 
{
	void			(*Init)(void);
	uint16_t 		(*DataInit)(uint32_t dataClassify, uint32_t catalogSize, uint32_t dataSize);
	uint16_t		(*CreateCatalog)(catalogInfo_s *dataInfo);
	uint16_t		(*DataSaveEnd)(uint32_t dataClassify);
	uint16_t		(*DataSave)(uint8_t *data, uint32_t length, uint32_t dataClassify);
	uint16_t		(*DataInfoRead)(uint16_t *catalogTotal, uint32_t *dataLength, uint32_t dataClassify);
	uint16_t		(*DataCatalogRead)(catalogInfo_s *dataInfo, uint32_t dataClassify, uint16_t catalogNum);
	uint16_t		(*DataRead)(uint8_t *data, uint32_t addr, uint32_t length, uint32_t dataUtc, uint32_t dataClassify);
	uint16_t		(*CatalogLenRead)(uint32_t *length, uint32_t dataUtc, uint32_t dataClassify);
	uint16_t		(*DeleteTotalData)(void);
	uint16_t		(*DeleteClassifyData)(uint32_t dataClassify);
	uint16_t		(*DeleteClassifyDataUtc)(uint32_t dataClassify, uint32_t utc);
	uint16_t		(*StorageDataRecover)(void);
}dataManage_s;

extern const dataManage_s	dataManage;













// // dataManage
// const dataManage_s dataManage =
// {
// 	.Init						= DataManageInit,
// 	.DataInit					= ClassifyDataInit,
// 	.CreateCatalog				= CreateCatalog,
// 	.DataSaveEnd				= DataSaveEnd,
// 	.DataSave					= ClassifyDataSave,
// 	.DataInfoRead				= ClassifyDataInfoRead,
// 	.DataCatalogRead			= ClassifyDataCatalogRead,
// 	.DataRead					= ClassifyDataRead,
// 	.CatalogLenRead				= ReadCatalogDataLen,
// 	.DeleteTotalData			= DeleteTotalData,
// 	.DeleteClassifyData			= DeleteClassifyData,
// 	.DeleteClassifyDataUtc		= DeleteClassifyDataUtc,
// 	.StorageDataRecover			= StorageDataRecover,
// };

#endif		// DATA_MANAGE_APP_H
