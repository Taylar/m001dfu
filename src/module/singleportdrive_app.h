#ifndef			SINGLEPORTDRIVE_APP_H
#define			SINGLEPORTDRIVE_APP_H


#define     SINGLEPORTDRIVE_PERIOD_CLOCK_TIME_MS       5


#define 	LED_PORT_ACTIVE_STATE			0
#define 	MOTO_PORT_ACTIVE_STATE			1

#ifndef		PORT_TYPE_T
#define		PORT_TYPE_T
typedef enum
{
	RED_LED,
	GREEN_LED,
	MOTO,
	PORT_MAX,
} port_type_t;
#endif



typedef struct 
{
	uint16_t		highTime;
	uint16_t		lowTime;
	uint16_t		driveTimes;
	uint16_t		portType;
	uint8_t			state;
}single_port_set_para_s;



void SetSinglePort(uint8_t ledId, uint8_t state, uint16_t period1, uint16_t period2, uint8_t times);

void SinglePortToggle(uint8_t portId);

void SinglePortSetPolar(uint8_t portId, uint8_t status);

void SinglePortDriveInit(void);



#endif			// SINGLEPORTDRIVE_APP_H
