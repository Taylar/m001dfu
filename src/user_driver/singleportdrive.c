#include "general.h"


#define			RED_LED_PIN			16// old :26 new:16
#define			GREEN_LED_PIN		27
#define			MOTO_PIN			6

const uint8_t PORT_PIN[] = {RED_LED_PIN, GREEN_LED_PIN, MOTO_PIN};



void BspSinglePortInit(void)
{
    bsp_board_init(BSP_INIT_LEDS);
    BspPortSet(MOTO,0);
}


void BspPortSet(uint8_t portIdx, uint8_t state)
{
	nrf_gpio_pin_write(PORT_PIN[portIdx], state);
}


void BspPorttoggle(uint8_t portIdx)
{
	nrf_gpio_pin_toggle(PORT_PIN[portIdx]);
}


const bsp_singleport_s bspSinglePort =
{
	.BspInit		= BspSinglePortInit,
	.PortSet 		= BspPortSet,
	.PortToggle 	= BspPorttoggle,
};
