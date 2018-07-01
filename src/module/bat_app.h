#ifndef			BAT_APP_H
#define			BAT_APP_H
#include <usertype.h>

typedef struct
{
	void 			(*Init)(void);
	void 			(*Enable)(void);
	void 			(*Disable)(void);
	void 			(*StartDetect)(void);
	void            (*ReadVol)(uint16_t *dataTemp);
	void            (*ReadLevel)(uint8_t *dataTemp);
	void            (*VolReadIsrCbInit)(void (*Cb)(void));
} bat_s;

extern const bat_s bat;

void BatMeasureInit(void);
void BatMeasureEnable(void);
void BatMeasureDisable(void);
void BatStartDetect(void);
void BatVolRead(uint16_t *dataTemp);
void BatLevelRead(uint8_t *dataTemp);
void BatVolReadIsrCbInit(void (*Cb)(void));

// const bat_s bat = 
// {
// 	.Init				= BatMeasureInit,
// 	.Enable				= BatMeasureEnable,
// 	.Disable			= BatMeasureDisable,
// 	.StartDetect		= BatStartDetect,
// 	.ReadVol			= BatVolRead,
// 	.ReadLevel			= BatLevelRead,
// 	.VolReadIsrCbInit	= BatVolReadIsrCbInit,
// };
#endif			//BAT_APP_H

