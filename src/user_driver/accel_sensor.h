#ifndef			ACCEL_SENSOR_H
#define			ACCEL_SENSOR_H


typedef struct
{
	void		(*BspInterfaceEnable)(void);
	void		(*BspInterfaceDisable)(void);
	void		(*Sleep)(void);
	uint16_t	(*SelfTest)(void);
	void		(*SampleSet)(void);	//for left
	void		(*BufRead)(uint16_t* length, uint8_t *data);	//for left
} bsp_accel_s;

extern const bsp_accel_s 	bspAccel;


// function declaration
void BspSpi_0_Init(void);

void BspSpi_0_Disable(void);

void BspKx022SetRegister(uint8_t regAddr, uint8_t data);

void BspKx022Set_25HzSample_Buf(void);

void BspKx022Set_100HzSample_Buf(void);

void BspKx022_Readbuf(uint16_t* length, uint8_t* dataAddr);

void BspKx022_Sleep(void);

uint16_t BspKx022_Selftest(void);



#endif			// ACCEL_SENSOR_H
