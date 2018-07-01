#ifndef			KEYAPP_H
#define			KEYAPP_H




typedef struct 
{
	void	(*Init)(void);
	void	(*ShortKeyIsrInit_S0)(uint16_t appHandle, func *Cb);
	void	(*ShortKeyIsrInit_S1)(uint16_t appHandle, func *Cb);
	void	(*ShortKeyIsrInit_S2)(uint16_t appHandle, func *Cb);
	void	(*HoldKey3sIsrInit_S0)(uint16_t appHandle, func *Cb);
	void	(*HoldKey3sIsrInit_S1)(uint16_t appHandle, func *Cb);
	void	(*HoldKey3sIsrInit_S2)(uint16_t appHandle, func *Cb);
	void	(*HoldKey6sIsrInit_S0)(uint16_t appHandle, func *Cb);
	void	(*HoldKey6sIsrInit_S1)(uint16_t appHandle, func *Cb);
	void	(*HoldKey6sIsrInit_S2)(uint16_t appHandle, func *Cb);
	uint8_t	(*ReadKey_S0)(void);
	uint8_t	(*ReadKey_S1)(void);
	uint8_t	(*ReadKey_S2)(void);
}key_register_s;


extern const key_register_s keyRegister;



void KeyAppInit(void);

void ShortKeyIsrInit_S0(uint16_t appHandle, func *Cb);

void ShortKeyIsrInit_S1(uint16_t appHandle, func *Cb);

void ShortKeyIsrInit_S2(uint16_t appHandle, func *Cb);

void HoldKey3sIsrInit_S0(uint16_t appHandle, func *Cb);

void HoldKey3sIsrInit_S1(uint16_t appHandle, func *Cb);

void HoldKey3sIsrInit_S2(uint16_t appHandle, func *Cb);

void HoldKey6sIsrInit_S0(uint16_t appHandle, func *Cb);

void HoldKey6sIsrInit_S1(uint16_t appHandle, func *Cb);

void HoldKey6sIsrInit_S2(uint16_t appHandle, func *Cb);

uint8_t	ReadKey_S0(void);

uint8_t	ReadKey_S1(void);

uint8_t	ReadKey_S2(void);

// // key
// const key_register_s keyRegister =
// {
// 	.Init					= KeyAppInit,
// 	.ShortKeyIsrInit_S0		= ShortKeyIsrInit_S0,
// 	.ShortKeyIsrInit_S1		= ShortKeyIsrInit_S1,
// 	.ShortKeyIsrInit_S2		= ShortKeyIsrInit_S2,
// 	.ShortKeyIsrInit_S3		= ShortKeyIsrInit_S3,
// 	.ShortKeyIsrInit_S4		= ShortKeyIsrInit_S4,
// 	.HoldKey3sIsrInit_S0	= HoldKey3sIsrInit_S0,
// 	.HoldKey3sIsrInit_S1	= HoldKey3sIsrInit_S1,
// 	.HoldKey3sIsrInit_S2	= HoldKey3sIsrInit_S2,
// 	.HoldKey3sIsrInit_S3	= HoldKey3sIsrInit_S3,
// 	.HoldKey3sIsrInit_S4	= HoldKey3sIsrInit_S4,
// 	.HoldKey6sIsrInit_S0	= HoldKey6sIsrInit_S0,
// 	.HoldKey6sIsrInit_S1	= HoldKey6sIsrInit_S1,
// 	.HoldKey6sIsrInit_S2	= HoldKey6sIsrInit_S2,
// 	.HoldKey6sIsrInit_S3	= HoldKey6sIsrInit_S3,
// 	.HoldKey6sIsrInit_S4	= HoldKey6sIsrInit_S4,
// 	.ReadKey_S0				= ReadKey_S0,
// 	.ReadKey_S1				= ReadKey_S1,
// 	.ReadKey_S2				= ReadKey_S2,
// 	.ReadKey_S3				= ReadKey_S3,
// 	.ReadKey_S4				= ReadKey_S4,
// };
#endif	// KEYAPP_H
	
