#include "general.h"


func *ShortKeySysModeS0[SYS_APP_HANDLE_NUM];
func *ShortKeySysModeS1[SYS_APP_HANDLE_NUM];
func *ShortKeySysModeS2[SYS_APP_HANDLE_NUM];

func *Hold3sKeySysModeS0[SYS_APP_HANDLE_NUM];
func *Hold3sKeySysModeS1[SYS_APP_HANDLE_NUM];
func *Hold3sKeySysModeS2[SYS_APP_HANDLE_NUM];

func *Hold6sKeySysModeS0[SYS_APP_HANDLE_NUM];
func *Hold6sKeySysModeS1[SYS_APP_HANDLE_NUM];
func *Hold6sKeySysModeS2[SYS_APP_HANDLE_NUM];





void ShortKeyAppS0(void)
{
	ShortKeySysModeS0[sysAppHandle]();
}

void ShortKeyAppS1(void)
{
	ShortKeySysModeS1[sysAppHandle]();
}

void ShortKeyAppS2(void)
{
	ShortKeySysModeS2[sysAppHandle]();
}


void Hold3sKeyAppS0(void)
{
	Hold3sKeySysModeS0[sysAppHandle]();
}

void Hold3sKeyAppS1(void)
{
	Hold3sKeySysModeS1[sysAppHandle]();
}

void Hold3sKeyAppS2(void)
{
	Hold3sKeySysModeS2[sysAppHandle]();
}


void Hold6sKeyAppS0(void)
{
	Hold6sKeySysModeS0[sysAppHandle]();
}

void Hold6sKeyAppS1(void)
{
	Hold6sKeySysModeS1[sysAppHandle]();
}

void Hold6sKeyAppS2(void)
{
	Hold6sKeySysModeS2[sysAppHandle]();
}


void KeyAppInit(void)
{
	uint8_t	i;
	bspKey.Init();

	bspKey.key[KEY_S0].ShortKey		= ShortKeyAppS0;
	bspKey.key[KEY_S1].ShortKey		= ShortKeyAppS1;
	bspKey.key[KEY_S2].ShortKey		= ShortKeyAppS2;

	bspKey.key[KEY_S0].Hold3sKey	= Hold3sKeyAppS0;
	bspKey.key[KEY_S1].Hold3sKey	= Hold3sKeyAppS1;
	bspKey.key[KEY_S2].Hold3sKey	= Hold3sKeyAppS2;

	bspKey.key[KEY_S0].Hold6sKey	= Hold6sKeyAppS0;
	bspKey.key[KEY_S1].Hold6sKey	= Hold6sKeyAppS1;
	bspKey.key[KEY_S2].Hold6sKey	= Hold6sKeyAppS2;

	for(i = 0; i < SYS_APP_HANDLE_NUM; i++)
	{
		ShortKeySysModeS0[i]	= NullFunc;
		ShortKeySysModeS1[i]	= NullFunc;
		ShortKeySysModeS2[i]	= NullFunc;

		Hold3sKeySysModeS0[i]	= NullFunc;
		Hold3sKeySysModeS1[i]	= NullFunc;
		Hold3sKeySysModeS2[i]	= NullFunc;
		
		Hold6sKeySysModeS0[i]	= NullFunc;
		Hold6sKeySysModeS1[i]	= NullFunc;
		Hold6sKeySysModeS2[i]	= NullFunc;
	}
}


void ShortKeyIsrInit_S0(uint16_t appHandle, func *Cb)
{
	ShortKeySysModeS0[appHandle] = Cb;
}

void ShortKeyIsrInit_S1(uint16_t appHandle, func *Cb)
{
	ShortKeySysModeS1[appHandle] = Cb;
}

void ShortKeyIsrInit_S2(uint16_t appHandle, func *Cb)
{
	ShortKeySysModeS2[appHandle] = Cb;
}



void HoldKey3sIsrInit_S0(uint16_t appHandle, func *Cb)
{
	Hold3sKeySysModeS0[appHandle] = Cb;
}

void HoldKey3sIsrInit_S1(uint16_t appHandle, func *Cb)
{
	Hold3sKeySysModeS1[appHandle] = Cb;
}

void HoldKey3sIsrInit_S2(uint16_t appHandle, func *Cb)
{
	Hold3sKeySysModeS2[appHandle] = Cb;
}


void HoldKey6sIsrInit_S0(uint16_t appHandle, func *Cb)
{
	Hold6sKeySysModeS0[appHandle] = Cb;
}

void HoldKey6sIsrInit_S1(uint16_t appHandle, func *Cb)
{
	Hold6sKeySysModeS1[appHandle] = Cb;
}

void HoldKey6sIsrInit_S2(uint16_t appHandle, func *Cb)
{
	Hold6sKeySysModeS2[appHandle] = Cb;
}



uint8_t	ReadKey_S0(void)
{
	return bspKey.ReadState_S0();
}

uint8_t	ReadKey_S1(void)
{
	return bspKey.ReadState_S1();
}

uint8_t	ReadKey_S2(void)
{
	return bspKey.ReadState_S2();
}

// key
const key_register_s keyRegister =
{
	.Init					= KeyAppInit,
	.ShortKeyIsrInit_S0		= ShortKeyIsrInit_S0,
	.ShortKeyIsrInit_S1		= ShortKeyIsrInit_S1,
	.ShortKeyIsrInit_S2		= ShortKeyIsrInit_S2,
	.HoldKey3sIsrInit_S0	= HoldKey3sIsrInit_S0,
	.HoldKey3sIsrInit_S1	= HoldKey3sIsrInit_S1,
	.HoldKey3sIsrInit_S2	= HoldKey3sIsrInit_S2,
	.HoldKey6sIsrInit_S0	= HoldKey6sIsrInit_S0,
	.HoldKey6sIsrInit_S1	= HoldKey6sIsrInit_S1,
	.HoldKey6sIsrInit_S2	= HoldKey6sIsrInit_S2,
	.ReadKey_S0				= ReadKey_S0,
	.ReadKey_S1				= ReadKey_S1,
	.ReadKey_S2				= ReadKey_S2,
};
