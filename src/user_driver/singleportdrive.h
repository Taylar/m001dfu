#ifndef		SINGLEPORTDRIVE_H
#define		SINGLEPORTDRIVE_H

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
	void		(*BspInit)(void);
	void		(*PortSet)(uint8_t portIdx, uint8_t state);
	void		(*PortToggle)(uint8_t portIdx);
} bsp_singleport_s;

extern const bsp_singleport_s bspSinglePort;

// function declaration
void BspSinglePortInit(void);

void BspPortSet(uint8_t portIdx, uint8_t state);


#endif		//SINGLEPORTDRIVE_H
