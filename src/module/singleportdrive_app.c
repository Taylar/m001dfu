/*
* @Author: zxt
* @Date:   2018-05-21 16:41:01
* @Last Modified by:   zxt
* @Last Modified time: 2018-05-21 17:57:38
*/
#include "general.h"


// ***************************************************************
// typedef 

typedef struct 
{
	uint8_t enable;
	uint8_t state;
	uint8_t times;
	uint16_t periodT1Set;
	uint16_t periodT2Set;
	uint16_t periodT1;
	uint16_t periodT2;
} singleport_drive_t;

singleport_drive_t singlePortDrive[PORT_MAX];

APP_TIMER_DEF(singlePortTmr);



//***********************************************************************************
//
// Led init.
//
//***********************************************************************************
void SinglePortDriveClkCb(void * p_context)
{
    uint8_t i;
    for(i = 0; i < PORT_MAX; i++)
    {
        if(singlePortDrive[i].enable)
        {
            if(singlePortDrive[i].times)
            {
                if(singlePortDrive[i].periodT1)
                {
                    singlePortDrive[i].periodT1--;
                    if(singlePortDrive[i].periodT1 == 0)
                    	bspSinglePort.PortSet(i, !singlePortDrive[i].state);
                }
                else
                {
                    if(singlePortDrive[i].periodT2)
                    {
                        singlePortDrive[i].periodT2--;
                    }
                    else
                    {
                        singlePortDrive[i].times--;
                        if(singlePortDrive[i].times)
                        	bspSinglePort.PortSet(i, singlePortDrive[i].state);
                        singlePortDrive[i].periodT1    = singlePortDrive[i].periodT1Set;
                        singlePortDrive[i].periodT2    = singlePortDrive[i].periodT2Set;
                    }
                }
            }
            else
            {
            	bspSinglePort.PortSet(i, !singlePortDrive[i].state);
                singlePortDrive[i].enable = 0;
            }
        }
    }
    for(i = 0; i < PORT_MAX; i++)
    {
        if(singlePortDrive[i].enable == true)
            break;
    }
    if(i >= PORT_MAX)
    {
        app_timer_stop(singlePortTmr);
    }
}

//***********************************************************************************
//
// Led control.
//      id:     allow multi led
//      state:  led first state, 0 or 1
//      period: led blink time,  0 means just set led state and no blink
//      times:  led blink times, 0 means just set led state and no blink
//
//***********************************************************************************
void SetSinglePort(uint8_t ledId, uint8_t state, uint16_t period1, uint16_t period2, uint8_t times)
{
    uint8_t i;

    app_timer_stop(singlePortTmr);


    bspSinglePort.PortSet(ledId, state);

    if (period1 == 0 || times == 0) {
        /* Unlock resource */

        for(i = 0; i < PORT_MAX; i++)
        {
            if(singlePortDrive[i].enable)
                break;
        }
        if(i < PORT_MAX)
        {
            app_timer_start(singlePortTmr, APP_TIMER_TICKS(SINGLEPORTDRIVE_PERIOD_CLOCK_TIME_MS), NULL);
        }

        return;
    }

    singlePortDrive[ledId].enable      = true;
    singlePortDrive[ledId].times       = times;
    singlePortDrive[ledId].state       = state;
    singlePortDrive[ledId].periodT1Set = (period1 >= SINGLEPORTDRIVE_PERIOD_CLOCK_TIME_MS)?period1/SINGLEPORTDRIVE_PERIOD_CLOCK_TIME_MS:1;
    singlePortDrive[ledId].periodT2Set = (period2 >= SINGLEPORTDRIVE_PERIOD_CLOCK_TIME_MS)?period2/SINGLEPORTDRIVE_PERIOD_CLOCK_TIME_MS:1;
    singlePortDrive[ledId].periodT1    = singlePortDrive[ledId].periodT1Set;
    singlePortDrive[ledId].periodT2    = singlePortDrive[ledId].periodT2Set;


    app_timer_start(singlePortTmr, APP_TIMER_TICKS(SINGLEPORTDRIVE_PERIOD_CLOCK_TIME_MS), NULL);
}

//***********************************************************************************
//
// Led toggle.
//      id:     allow multi led
//
//***********************************************************************************
void SinglePortToggle(uint8_t portId)
{
	bspSinglePort.PortToggle(portId);
}

//***********************************************************************************
//
// Led toggle.
//      id:     allow multi led
//
//***********************************************************************************
void SinglePortSetPolar(uint8_t portId, uint8_t status)
{
	bspSinglePort.PortSet(portId, status);
}



void SinglePortDriveInit(void)
{
    uint32_t err_code   = NRF_SUCCESS;
    
	bspSinglePort.BspInit();
	memset(singlePortDrive, 0, sizeof(singlePortDrive));

	err_code = app_timer_create(&singlePortTmr,
                                APP_TIMER_MODE_REPEATED,
                                SinglePortDriveClkCb);
}

