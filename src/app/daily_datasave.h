#ifndef			DAILY_DATASAVE_H
#define			DAILY_DATASAVE_H

#define			DATA_UNUPLOADING		0
#define			DATA_UPLOADING			1




void DailyDataSaveInit(void);

void DailyStepSave(uint32_t utc, uint16_t step);

void DailyStepSaveCache(void);

uint16_t DailyDataRequestTotalInfoAck(uint32_t dataClassify);

uint16_t DailyStepCatalogInfoRead(uint32_t dataClassify, uint16_t catalogNum);

uint16_t DailyStepRequestData(uint32_t dataClassify, uint32_t utc, uint16_t packNum);

uint16_t DailyDataUploadProcess(void);

uint16_t DailyDataTotalDelete(void);

uint16_t DailyDataDeleteClassify(uint32_t dataClassify);

uint16_t DailyDataDeleteCatalog(uint32_t dataClassify, uint32_t utc);











#endif			//DAILY_DATASAVE_H
