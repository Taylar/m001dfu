#ifndef			BAT_H
#define			BAT_H

typedef		void xBatCb(uint16_t adcTemp);


typedef struct
{
	void		(*BspInit)(void);
	void		(*Enable)(void);
	void		(*Disable)(void);
	void		(*StartDetect)(void);
	void		(*InitIsrCb)(xBatCb *Cb);	//for left
} bsp_bat_s;

extern const bsp_bat_s	bspBat;


// function declaration
void BspBatInit(void);

void BspEnableADC(void);

void BspDisableADC(void);

void BspBatStartDetect(void);

void BspBatIsrCbInit(xBatCb *Cb);


#endif			// BAT_H
