#include "general.h"

uint32_t dailyTotalStep;
uint32_t dailyStepAim;
uint16_t dailyStepComplete;



static uint8_t G_sensorBuf[256];


static uint8_t  rejectCall;



void DailySportInit(void)
{
    dailyTotalStep          = 0;
    dailyStepComplete       = 0;
    dailyStepAim            = 10000;
}



void DailyStepProcess(void)
{
    uint16_t     buflength;
    uint8_t     temp;
	uint8_t     temp2;
    uint16_t     i = 0;
	uint16_t    dailyTotalStepTemp;


	memset(G_sensorBuf,0,256);
    
	setActionThreshold(250, 200, 250, 200);
		
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

                dailyStepComplete   = (uint16_t)(dailyTotalStep * 100 / dailyStepAim);
                if(dailyStepComplete > 100)
                dailyStepComplete = 100;
				
				if(phoneState == PHONE_STATE_DAILY_SPORT)
				{
					dailyTotalStepTemp          = (uint16_t)dailyTotalStep;
//				    bleMsg.id                   = BLE_MSG_SEND;
//					bleMsg.u.protocal           = DAILY_SPORTSCENE_PROTOCAL;
//					bleMsg.u.protocal.att.load.content.parameter[0]  = (uint8_t)(dailyTotalStepTemp >> 8);
//					bleMsg.u.protocal.att.load.content.parameter[1]  = (uint8_t)(dailyTotalStepTemp);
//					ble.EventSet(bleMsg);
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
//                        bleMsg.id           = BLE_MSG_SEND;						
//						bleMsg.u.protocal   = AUTHOR_PASS_PROTOCAL;
//						ble.EventSet(bleMsg);
//						xTimerStop(AuthTimeOutTimer,0);        
//						multiModule.EventSet(NULL_GREEN_LED);
//						multiModule.EventSet(AUTHOR_MOTO);        
//						phoneState      = PHONE_STATE_NORMAL;
//						rejectCall = 0;
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
//						 if(callMsgFrom == BLE_TO_MCU)
//						 bleMsg.u.protocal   = REJECT_CALL_IOS_PROTOCAL;
//						 else
//						 bleMsg.u.protocal   = REJECT_CALL_PROTOCAL;
//						 
//                         bleMsg.id           = BLE_MSG_SEND;
//						 ble.EventSet(bleMsg);
//						 multiModule.EventSet(NULL_RED_LED);
//						 multiModule.EventSet(NULL_GREEN_LED);
//						 multiModule.EventSet(NULL_MOTO);
//						 phoneState      = PHONE_STATE_NORMAL;
//						 dailyMode       = dailyModeBak;
//						 ModeSwitchToutTimerStop();
//						 rejectCall = 0;
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
						 // DailyTakePhotoEvent();
						 rejectCall = 0;
						 return;
					 }
				 }
			 }
             i++;
        }
    }		
}

