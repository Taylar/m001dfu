#ifndef			EXTFLASH_H
#define			EXTFLASH_H



typedef struct
{
	void		(*PowerOn)(void);
	void		(*PowerDown)(void);
	void		(*BspInit)(void);
	void		(*Sleep)(void);
	void		(*WakeUp)(void);
	uint16_t	(*IdleTest)(void);
	uint8_t *	(*ReadMac)(void);
	void		(*ReadCmd)(uint8_t* pdata, uint32_t addr, uint16_t length);
	void		(*WriteCmd)(uint8_t* pdata, uint32_t addr, uint16_t length);
	void		(*Erase4KCmd)(uint32_t addr);
} bsp_extflash_s;

extern const bsp_extflash_s bspExtflash;

// function declaration
void BspPowerDownExtflash(void);

void BspPowerOnExtflash(void);

void BspExtflashInit(void);

void ExtflashWakeUpCmd(void);

void ExtflashSleepCmd(void);

uint8_t* ExtflashReadUid(void);

uint8_t ReadExtflashRegister1(void);

void ExtflashReadCmd(uint8_t* pdata, uint32_t addr, uint16_t length);

void ExtflashWriteCmd(uint8_t* pdata, uint32_t addr, uint16_t length);

void Extflash4kEraseCmd(uint32_t startSectorAddr);

uint16_t ExtflashStateTest(void);



#endif			//	EXTFLASH_H
