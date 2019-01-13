#include "general.h"

uint32_t dailyTotalStep;
uint32_t dailyStepSave;
uint32_t dailyStepAim;
uint16_t dailyStepComplete;
uint8_t  dailyStepCompleteFlag;



static uint8_t G_sensorBuf[256];


static uint8_t  rejectCall;



void DailySportInit(void)
{
    dailyTotalStep          = 0;
    dailyStepSave           = 0;
    dailyStepComplete       = 0;
    dailyStepCompleteFlag 	= 0;
    dailyStepAim            = 10000;
}

void DailySportClear(void)
{
    dailyTotalStep          = 0;
    dailyStepComplete       = 0;
    dailyStepCompleteFlag 	= 0;
}


uint32_t DailyStepSaveRead(void)
{
	return dailyStepSave;
}

void DailyStepSaveClear(void)
{
	dailyStepSave = 0;
}


void DailyStepProcess(void)
{
    uint16_t     buflength;
    uint8_t     temp;
	uint8_t     temp2;
    uint16_t     i = 0;


	memset(G_sensorBuf,0,256);
    
	setActionThreshold(180, 100, 180, 100);
		
    accel.bsp->BspInterfaceEnable();
    accel.bsp->BufRead(&buflength, G_sensorBuf);
    accel.bsp->BspInterfaceDisable();

    buflength = buflength/3;
    rejectCall = 0;
    if(buflength)
    {
        while(buflength--)
        {
            temp  = CalculateStep(G_sensorBuf + i*3);
			
            if(temp == 1 || temp == 12)
            {
                dailyTotalStep      += temp;
                dailyStepSave 		+= temp;

                dailyStepComplete   = (uint16_t)(dailyTotalStep * 100 / dailyStepAim);
                if(dailyStepComplete > 100)
                {
	                dailyStepComplete = 100;

	            	if(dailyStepCompleteFlag == 0)
	            	{
	            		dailyStepCompleteFlag = 1;
	            		sportModeTime = 1;
						DisplaySportComplete(dailyStepComplete);
	            		SetSinglePort(MOTO, MOTO_PORT_ACTIVE_STATE, 150, 850, 5);
	            		SetSinglePort(RED_LED, LED_PORT_ACTIVE_STATE, 150, 850, 5);
	            	}
                }
				
				if(phoneState == PHONE_STATE_DAILY_SPORT)
				{
					BlePack(DEVICE_CUR_STEP, &bleSendMsg);
				}
            }
			
			if(phoneState == PHONE_STATE_AUTHOR)
			{
			    temp2 = actionRecongnition(G_sensorBuf + i*3);
				if(temp2 == 1)
				{
				    rejectCall++;
					if(rejectCall >= 2)
					{
						return;
					}				
				 }
			 }

			 if(phoneState == PHONE_STATE_REJECT_CALL)
			 {
			     temp2 = actionRecongnition(G_sensorBuf + i*3);
				 if(temp2 == 1)
				 {
				     rejectCall++;
					 if(rejectCall >= 3)
					 {

						 return;				
					 }
				 }
			 }

			 if(phoneState == PHONE_STATE_TAKE_PICTURE)
			 {
			     temp2 = actionRecongnition(G_sensorBuf + i*3);
				 if(temp2 == 1)
				 {
				     rejectCall++;
					 if(rejectCall >= 2)
					 {	
						BlePack(DEVICE_REQ_TAKE_PIC, &bleSendMsg);
						rejectCall = 0;
						return;
					 }
				 }
			 }
             i++;
        }
    }		
}

