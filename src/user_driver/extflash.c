#include "general.h"


#define 		FLASH_SCLK_PIN				(5)
#define 		FLASH_SDO_PIN				(28)
#define 		FLASH_SDI_PIN				(29)
#define 		FLASH_CS_PIN				(11)

#define			ExtFlashCS_Enable()					nrf_gpio_pin_write(FLASH_CS_PIN, 0)
#define			ExtFlashCS_Disable()				nrf_gpio_pin_write(FLASH_CS_PIN, 1)



// *************************************************
// chip define
#define FLASH_WAKE_UP                       (0xAB)
#define FLASH_SLEEP                         (0xB9)
#define FLASH_READ_UID                      (0x4B)
#define FLASH_WRITE_ENABLE                  (0x06)
#define FLASH_READ_STATU_REG1               (0x05)
#define FLASH_READ_DATA                     (0x03)
#define FLASH_PAGE_PROGRAM                  (0x02)
#define FLASH_4K_ERASE                  	(0x20)

// *************************************************
// Macros
#define		EXTFLASH_OK						0X00
#define		EXTFLASH_ERROR					0XFF

// statue
#define		EXTFLASH_BUSY					0X01


// 硬件移植时需改CS使能接口
#define			ExtFlashCS_Enable()					nrf_gpio_pin_write(FLASH_CS_PIN, 0)
#define			ExtFlashCS_Disable()				nrf_gpio_pin_write(FLASH_CS_PIN, 1)

/**
 * @brief       soft spi write data,移植时需修改该函数
 * @param[in]  var   write to spi bus
 */
void Spi_WriteByte(uint8_t var)
{
	int i;
	for (i = 0; i < 8; ++i)
	{
		if(var&0x80)
			nrf_gpio_pin_write(FLASH_SDI_PIN, 1);
		else
			nrf_gpio_pin_write(FLASH_SDI_PIN, 0);
        nrf_gpio_pin_write(FLASH_SCLK_PIN, 1);
		nrf_gpio_pin_write(FLASH_SCLK_PIN, 0);
		var<<=1;
	}
}


/**
 * @brief   spi write datas,
 * param	pData:	the writed data point
			length:	the length of data
 * @return     none
 */
void Spi_WriteBytes(uint8_t* pData, uint16_t length)
{
	uint16_t i;
	for(i = 0; i < length; i++)
	{
		Spi_WriteByte(pData[i]);
	}
}

/**
 * @brief     read from the  SPI,移植时需修改该函数
 * @return      the data form spi
 */
uint8_t Spi_ReadByte(void)
{
	uint8_t var=0;
	int i;
	for (i = 0; i < 8; ++i)
	{
		var<<=1;
		nrf_gpio_pin_write(FLASH_SCLK_PIN, 1);
		if(nrf_gpio_pin_read(FLASH_SDO_PIN)>0)
			var|=1;
		nrf_gpio_pin_write(FLASH_SCLK_PIN, 0);
	}
	return var;
}

/**
 * @brief   spi read datas,
 * param	pData:	the read data point
			length:	the length of data
 * @return     none
 */
void Spi_ReadBytes(uint8_t* pData, uint16_t length)
{
	uint16_t i;
	for(i = 0; i < length; i++)
	{
		pData[i] = Spi_ReadByte();
	}
}

/**
 * @brief   关断外部Flash电源并关闭接口，上电后需延时一段时间才可进行操作,
 			移植时需修改该函数，
 * param	none
 * @return  none
 */
void BspPowerDownExtflash(void)
{


}

/**
 * @brief   开启外部flash电源并初始化接口,上电后需延时一段时间才可进行操作,
 			移植时需修改该函数
 * param	none
 * @return  none
 */
void BspPowerOnExtflash(void)
{
	nrf_gpio_cfg_output(FLASH_CS_PIN);
	nrf_gpio_cfg_output(FLASH_SDI_PIN);
	nrf_gpio_cfg_output(FLASH_SCLK_PIN);
	nrf_gpio_cfg_input(FLASH_SDO_PIN, BUTTON_PULL);

	nrf_gpio_pin_write(FLASH_CS_PIN, 1);
	nrf_gpio_pin_write(FLASH_SDI_PIN, 1);
	nrf_gpio_pin_write(FLASH_SCLK_PIN, 0);	
}


void BspExtflashInit(void)
{

}



/**
 * @brief   
 * param	none
 * @return  none
 */
void ExtflashWakeUpCmd(void)
{
	ExtFlashCS_Enable();
	Spi_WriteByte(FLASH_WAKE_UP);
	ExtFlashCS_Disable();
}




/**
 * @brief   the flash enter sleep, all cmd are reject except the Wakeup cmd
 * param	
 * @return     none
 */
void ExtflashSleepCmd(void)
{
	ExtFlashCS_Enable();
	Spi_WriteByte(FLASH_SLEEP);
	ExtFlashCS_Disable();
}



static uint8_t flashUid[8];
/**
 * @brief   read the flash uid[8]
 * param	none
 * @return  the point of uid
 */
uint8_t* ExtflashReadUid(void)
{
	uint8_t *uid=flashUid;
	ExtFlashCS_Enable();
	Spi_WriteByte(FLASH_READ_UID);
	Spi_WriteBytes(uid, 4);
	Spi_ReadBytes(uid, 8);
	ExtFlashCS_Disable();
	return uid;
}


/**
 * @brief     read the register1 statue
			bit0: 1:busy 0:idle
			bit1: 1:write enable 0:write disable
			others: about flash protect
 * @return      the statue of chip
 */
uint8_t ReadExtflashRegister1(void)
{
	uint8_t statue;
	ExtFlashCS_Enable();
	Spi_WriteByte(FLASH_READ_STATU_REG1);
	Spi_ReadBytes(&statue, 1);
	ExtFlashCS_Disable();
	return statue;
}


/**
 * @brief   read the datas from the chip from the indicated addr
 * param	
			pdata:		the point of the read datas buf
			addr:		the indicate addr of the flash
			length:		the length of read data
 * @return  none
 */
void ExtflashReadCmd(uint8_t* pdata, uint32_t addr, uint16_t length)
{
	uint8_t data[3];
	data[2] = (uint8_t)addr;
	data[1] = (uint8_t)(addr >> 8);
	data[0] = (uint8_t)(addr >> 16);
	ExtFlashCS_Enable();
	Spi_WriteByte(FLASH_READ_DATA);
	Spi_WriteBytes(data, 3);
	Spi_ReadBytes(pdata, length);
	ExtFlashCS_Disable();
}


/**
 * @brief   write the datas to the chip at the indicated addr, the busy bit should be 0
 * param	
			pdata:		the point of the write datas buf
			addr:		the indicate addr of the flash
			length:		the length of writed data
 * @return  none
 */
void ExtflashWriteCmd(uint8_t* pdata, uint32_t addr, uint16_t length)
{
	uint8_t data[3];
	data[2] = (uint8_t)addr;
	data[1] = (uint8_t)(addr >> 8);
	data[0] = (uint8_t)(addr >> 16);
	ExtFlashCS_Enable();
	Spi_WriteByte(FLASH_WRITE_ENABLE);
	ExtFlashCS_Disable();
	ExtFlashCS_Enable();
	Spi_WriteByte(FLASH_PAGE_PROGRAM);
	Spi_WriteBytes(data, 3);
	Spi_WriteBytes(pdata, length);
	ExtFlashCS_Disable();
}


/**
 * @brief   erase the sectors(4Kbyte)
 * param	
			startSectorAddr:	the first erase sector 
 * @return  none
 */
void Extflash4kEraseCmd(uint32_t startSectorAddr)
{
	uint8_t data[3];
	data[2]		= (uint8_t)startSectorAddr;
	data[1]		= (uint8_t)(startSectorAddr >> 8);
	data[0]		= (uint8_t)(startSectorAddr >> 16);
	ExtFlashCS_Enable();
	Spi_WriteByte(FLASH_WRITE_ENABLE);
	ExtFlashCS_Disable();
	ExtFlashCS_Enable();
	Spi_WriteByte(FLASH_4K_ERASE);
	Spi_WriteBytes(data, 3);
	ExtFlashCS_Disable();
}



uint16_t ExtflashStateTest(void)
{
	uint8_t statue;

	statue = ReadExtflashRegister1();
	if(statue & EXTFLASH_BUSY)
		return 0xff;

	return 0;
}



// uint16_t ExtflashSelfTest(void)
// {
// 	uint8_t i;
// 	uint8_t     flashTestData[8];
// 	Extflash4kEraseCmd(FACTORY_SELFTEST_START_ADDR);
// 	i = 0;
// 	while(ExtflashStateTest())
// 	{
// 		i++;
// 		if(i == 10)			//wait timer could not large than 1s
// 		{
// 			return 0xff;
// 		}
// 		am_util_delay_ms(100);
// 	}

// 	ExtflashReadCmd(flashTestData, FACTORY_SELFTEST_START_ADDR, 8);
// 	for(i = 0; i < 8; i++)
// 	{
// 		if(flashTestData[i] != 0xff)
// 			return 0xff;
// 		flashTestData[i] = 0x01 << i;
// 	}

// 	ExtflashWriteCmd(flashTestData, FACTORY_SELFTEST_START_ADDR, 8);
// 	am_util_delay_ms(10);//wait timer could not large than 10ms
// 	if(ExtflashStateTest() == 0xff)
// 		return 0xff;
// 	while(ExtflashStateTest())
// 	ExtflashReadCmd(flashTestData, FACTORY_SELFTEST_START_ADDR, 8);
// 	for(i = 0; i < 8; i++)
// 	{
// 		if(flashTestData[i] != 0x01<<i)
// 			return 0xffff;
// 	}
// 	return 0;
// }



// extflash
const bsp_extflash_s bspExtflash =
{
	.PowerOn	= BspPowerOnExtflash,
	.PowerDown	= BspPowerDownExtflash,
	.BspInit	= BspExtflashInit,
	.Sleep		= ExtflashSleepCmd,
	.WakeUp		= ExtflashWakeUpCmd,
	.IdleTest	= ExtflashStateTest,
	.ReadMac	= ExtflashReadUid,
	.ReadCmd	= ExtflashReadCmd,
	.WriteCmd	= ExtflashWriteCmd,
	.Erase4KCmd	= Extflash4kEraseCmd,
};
