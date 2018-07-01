#ifndef			ACCEL_APP_H
#define			ACCEL_APP_H

#include "accel_sensor.h"


typedef struct 
{
	bsp_accel_s*			bsp;

}accel_s;


extern const accel_s 	accel;

#endif			//ACCEL_APP_H
