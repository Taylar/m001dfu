#include "general.h"


//kx022 pin
#define			SPI_SS_PIN			(8)
#define			SPI_MISO_PIN		(30)
#define			SPI_MOSI_PIN		(31)
#define			SPI_SCK_PIN			(4)

// *************************************************
#define		Kx022_CS_Enable()				nrf_gpio_pin_write(SPI_SS_PIN, 0)
#define		Kx022_CS_Disable()				nrf_gpio_pin_write(SPI_SS_PIN, 1)



// register macro
#define XHPL			(0x00)		// R
#define XHPH			(0x01)		// R
#define YHPL			(0x02)		// R
#define YHPH			(0x03)		// R
#define ZHPL			(0x04)		// R
#define ZHPH			(0x05)		// R
#define XOUTL			(0x06)		// R
#define XOUTH			(0x07)		// R
#define YOUTL			(0x08)		// R
#define YOUTH			(0x09)		// R
#define ZOUTL			(0x0A)		// R
#define ZOUTH			(0x0B)		// R
#define COTR			(0x0C)		// R

#define WHO_AM_I		(0x0F)		// RW
#define TSCP			(0x10)		// R
#define TSPP			(0x11)		// R
#define INS1			(0x12)		// R
#define INS2			(0x13)		// R
#define INS3			(0x14)		// R
#define STAT			(0x15)		// R

#define INT_REL			(0x17)		// R
#define CNTL1			(0x18)		// RW
#define CNTL2			(0x19)		// RW
#define CNTL3			(0x1A)		// RW
#define ODCNTL			(0x1B)		// RW
#define INC1			(0x1C)		// RW
#define INC2			(0x1D)		// RW
#define INC3			(0x1E)		// RW
#define INC4			(0x1F)		// RW
#define INC5			(0x20)		// RW
#define INC6			(0x21)		// RW
#define TILT_TIMER		(0x22)		// RW
#define WUFC			(0x23)		// RW
#define TDTRC			(0x24)		// RW
#define TDTC			(0x25)		// RW
#define TTH				(0x26)		// RW
#define TTL				(0x27)		// RW
#define FTD				(0x28)		// RW
#define STD				(0x29)		// RW
#define TLT				(0x2A)		// RW
#define TWS				(0x2B)		// RW

#define ATH				(0x30)		// RW
#define TILT_ANGLE_LL	(0x32)		// RW
#define TILT_ANGLE_HL	(0x33)		// RW
#define HYST_SET		(0x34)		// RW
#define LP_CNTL			(0x35)		// RW

#define BUF_CNTL1		(0x3A)		// RW
#define BUF_CNTL2		(0x3B)		// RW
#define BUF_STATUS_1	(0x3C)		// R
#define BUF_STATUS_2	(0x3D)		// R
#define BUF_CLEAR		(0x3E)		// W
#define BUF_READ		(0x3F)		// R
#define SELF_TEST		(0x60)		// RW


#define SPI_INSTANCE  0 /**< SPI instance index. */
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  /**< SPI instance. */
static volatile bool spi_xfer_done;  /**< Flag used to indicate that SPI instance completed the transfer. */



void BspSpi_0_Init(void)
{

	nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;

    spi_config.ss_pin   = NULL;
    spi_config.miso_pin = SPI_MISO_PIN;
    spi_config.mosi_pin = SPI_MOSI_PIN;
    spi_config.sck_pin  = SPI_SCK_PIN;

    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, NULL, NULL));

}


static void SPI_WriteData(uint8_t regAddr, uint8_t* data_Point, uint16_t length )
{
	Kx022_CS_Enable();
	nrf_drv_spi_transfer(&spi, &regAddr, 1, NULL, NULL);
	nrf_drv_spi_transfer(&spi, data_Point, length, NULL, NULL);
	Kx022_CS_Disable();
}


static void SPI_ReadData(uint8_t regAddr, uint8_t* data_Point, uint16_t length)
{
	regAddr |= 0x80;
	Kx022_CS_Enable();
	nrf_drv_spi_transfer(&spi, &regAddr, 1, NULL, NULL);
	nrf_drv_spi_transfer(&spi, NULL, NULL, data_Point, length);
	Kx022_CS_Disable();
}

//disable hw SPI 1 of ambiq 
void BspSpi_0_Disable(void)
{
    nrf_drv_spi_uninit(&spi);
    nrf_gpio_cfg_output(SPI_MISO_PIN);
    nrf_gpio_cfg_output(SPI_MOSI_PIN);
    nrf_gpio_cfg_output(SPI_SCK_PIN);
    nrf_gpio_cfg_output(SPI_SS_PIN);
    nrf_gpio_pin_write(SPI_SS_PIN, 1);
    nrf_gpio_pin_write(SPI_MISO_PIN, 0);
    nrf_gpio_pin_write(SPI_MOSI_PIN, 0);
    nrf_gpio_pin_write(SPI_SCK_PIN, 0);
}


void BspKx022SetRegister(uint8_t regAddr, uint8_t data)
{
	SPI_WriteData(regAddr, &data, 1);
}



void BspKx022Set_25HzSample_Buf(void)
{
	BspKx022SetRegister(CNTL1, 	0x00);
	BspKx022SetRegister(ODCNTL, 	0x81);
	BspKx022SetRegister(LP_CNTL,	0x2b);
	BspKx022SetRegister(BUF_CNTL2, 0x81);
	BspKx022SetRegister(BUF_CLEAR, 0x00);
	BspKx022SetRegister(INC1, 		0x10);
	BspKx022SetRegister(INC2, 		0x3f);
	BspKx022SetRegister(CNTL1, 	0x80);
}

void BspKx022Set_100HzSample_Buf(void)
{
	BspKx022SetRegister(CNTL1, 	0x00);
	BspKx022SetRegister(ODCNTL, 	0x83);
	BspKx022SetRegister(LP_CNTL,	0x2b);
	BspKx022SetRegister(BUF_CNTL2, 0x81);
	BspKx022SetRegister(BUF_CLEAR, 0x00);
	BspKx022SetRegister(INC1, 		0x10);
	BspKx022SetRegister(INC2, 		0x3f);
	BspKx022SetRegister(CNTL1, 	0x80);
}




void BspKx022_Readbuf(uint16_t* length, uint8_t* dataAddr)
{
	uint8_t data;
	SPI_ReadData(0xbc, &data, 1);
	*length = data;

	// 防止读取溢出
	if(data > 256)
		*length	= 256;
	if(data == 0)
		return;
	
	SPI_ReadData(0xbf, dataAddr, *length);
}


void BspKx022_Sleep(void)
{
	BspKx022SetRegister(CNTL1, 	0x00);
}



uint16_t BspKx022_Selftest(void)
{
	uint16_t comparedata;

	BspKx022SetRegister(CNTL1, 	0x80);
	SPI_ReadData(CNTL1, (uint8_t *)&comparedata, 1);

	if(comparedata != 0x80)
	{
		return 0xff;
	}

	BspKx022SetRegister(CNTL1, 	0x00);
	SPI_ReadData(CNTL1, (uint8_t *)&comparedata, 1);

	if(comparedata != 0x00)
	{
		return 0xff;
	}

	return 0;
}




// accel
const bsp_accel_s 	bspAccel = 
{
	.BspInterfaceEnable		= BspSpi_0_Init,
	.BspInterfaceDisable	= BspSpi_0_Disable,
	.Sleep					= BspKx022_Sleep,
	.SelfTest				= BspKx022_Selftest,
	.SampleSet				= BspKx022Set_25HzSample_Buf,
	.BufRead				= BspKx022_Readbuf,
};


