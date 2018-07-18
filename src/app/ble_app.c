#include "general.h"

uint8_t bleSerialNum;
uint8_t modifyBleNameFlah;


protocal_msg_t bleSendMsg;
protocal_msg_t bleRecMsg;

uint8_t DailyProtocalCheckcode(protocal_msg_t *msg)
{
    uint8_t length;
    uint8_t checkCode;
    uint8_t i;
    uint8_t *data;
    data = (uint8_t*)msg;
    length = msg->length + 5;

    checkCode   = 0;
    for(i = 0; i < length; i++)
    {
        checkCode += data[i] ^ i;
    }
    return checkCode;
}

void BlePack(uint32_t command, protocal_msg_t *msg)
{
	uint8_t		serialNumTemp;
	uint16_t	idTemp;
	uint8_t 	i;

	serialNumTemp = msg->serialNum;
	idTemp		  = (msg->id[0] << 8) + msg->id[1];

	msg->stratSign = BLE_START_SIGN;
	msg->serialNum = bleSerialNum;
	bleSerialNum++;
	msg->id[0] 	 = (uint8_t)(command>>8);
	msg->id[1] 	 = (uint8_t)(command);

	switch(command)
	{
		case DEVICE_COM_ACK:
		msg->length = 4;
		msg->load[3] = 0;
		msg->load[0] = serialNumTemp;
		msg->load[1] = (uint8_t)(idTemp >> 8);
		msg->load[2] = (uint8_t)idTemp;
		break;


		case DEVICE_REQUST_RTC:
		msg->length = 0;
		break;


		case DEVICE_INFO:
		msg->length = 5;
		msg->load[0] = (uint8_t)(FW_VERSION >> 8);
		msg->load[1] = (uint8_t)FW_VERSION;

		msg->load[2] = 80;

		msg->load[3] = (uint8_t)(BLE_PRO_VER >> 8);
		msg->load[4] = (uint8_t)BLE_PRO_VER;
		break;


		case DEVICE_COMMIT_LINK:
		msg->length = 0;
		break;


		case DEVICE_CUR_STEP:
		msg->length = 4;

		msg->load[0] = (uint8_t)(dailyTotalStep >> 24);
		msg->load[1] = (uint8_t)(dailyTotalStep >> 16);
		msg->load[2] = (uint8_t)(dailyTotalStep >> 8);
		msg->load[3] = (uint8_t)(dailyTotalStep);
		break;


		case DEVICE_HIS_DATA_INFO:
		break;


		case DEVICE_UPDATA_COMMAND:
		break;


		case DEVICE_REQ_TAKE_PIC:
		msg->length = 0;
		break;

		case DEVICE_SPORT_AIM:
		if(msg->load[0] == 0x01)
		{
			msg->load[0] = 0x01;

			msg->load[1] = (uint8_t)(dailyStepAim >> 24);
			msg->load[2] = (uint8_t)(dailyStepAim >> 16);
			msg->load[3] = (uint8_t)(dailyStepAim >> 8);
			msg->load[4] = (uint8_t)(dailyStepAim);
			msg->length = 5;
		}
		break;

		case DEVICE_REPLY_BLE:
		if(msg->load[0] == 0x01)
		{
			BleMacRead(msg->load+1);
			msg->length = 7;
		}
		break;

		default:
		return;
	}

	msg->load[msg->length] = DailyProtocalCheckcode(msg);

	// for(i = 0; i < msg->length; i++)
	// 	NRF_LOG_INFO("%2x", msg->load[i]);
 //    NRF_LOG_INFO("\n");
    NRF_LOG_INFO("send:%s\n", HexToSprintf((uint8_t*)msg, msg->length+6));			
    
	Ble_CommandHandleSend((uint8_t*)msg, msg->length+6);
}



void BleProtocal(protocal_msg_t *msg)
{
	uint8_t checkCode;
	uint16_t length;
	int32_t temp, command;

	movt_task_msg_t     movtMsg;
    rtc_time_s          dailyRtc, dailyRtc2;


	length = msg->length + 5;
    checkCode = DailyProtocalCheckcode(msg);

    // if(checkCode != msg->load[length])
    // 	return;

    command = ((uint16_t)msg->id[0] << 8) + msg->id[1];
    switch(command)
    {
		case APP_COM_ACK:
		break;


		case APP_REPLY_RTC:
		rtcApp.UtcTransformTime(((uint32_t)msg->load[0]<<24)+ ((uint32_t)msg->load[1]<<16) + ((uint32_t)msg->load[2]<<8) + ((uint32_t)msg->load[3]),
				&dailyRtc);

		dailyRtc.zone = 0;

		if((int8_t)msg->load[4] < 0)
		{
			temp = -(int8_t)msg->load[4];
		}
		dailyRtc2.zone = ((temp / 10) << 8) + (temp % 10 * 10);

		rtcApp.TimeZoneTransform(&dailyRtc, &dailyRtc2);
		rtcApp.Write(&dailyRtc2);

		movtMsg.id          = MOVT_MSG_MC_SET_AIM_FORWARD;
	    movtMsg.aim         = ExchangeTimeforCount(dailyRtc2.hour, dailyRtc2.min, dailyRtc2.sec);
	    MovtEventSet(movtMsg);
	    BlePack(DEVICE_COM_ACK, msg);
		break;


		case APP_REQUST_LINK:
	    BlePack(DEVICE_COM_ACK, msg);
	    phoneState = PHONE_STATE_PEER;
	    appPeerModeTime = 1;
		SetSinglePort(GREEN_LED, LED_PORT_ACTIVE_STATE, 125, 875, 60);
	    BlePack(DEVICE_COM_ACK, msg);
		break;


		case APP_REQUST_DEV_INFO:
	    BlePack(DEVICE_INFO, msg);
		break;


		case APP_SET_STEP_AIM:
		if(msg->load[0] == 0x01)
		{
			BlePack(DEVICE_SPORT_AIM, msg);
		}
		else
		{
			dailyStepAim = (((uint32_t)msg->load[1]) << 24) + 
							(((uint32_t)msg->load[2]) << 16) +
							(((uint32_t)msg->load[3]) << 8) +
							(((uint32_t)msg->load[4]));

		    BlePack(DEVICE_COM_ACK, msg);
		}
		break;


		case APP_REQUST_CUR_STEP:
	    BlePack(DEVICE_CUR_STEP, msg);
		break;


		case APP_TAKE_PIC_SET:
		if(msg->load[0] == 1)
		{
			phoneState = PHONE_STATE_TAKE_PICTURE;
			takePicModeTime = 1;
		}
		else
		{
			phoneState = PHONE_STATE_NORMAL;
			takePicModeTime = 0;
		}
	    BlePack(DEVICE_COM_ACK, msg);
		break;


		case APP_SET_MOVT:
		if(msg->load[0] == 0x01)
		{
			if(msg->load[1] == 0x01)
			{
				movtMsg.id          = MOVT_MSG_MC_STOP;
                MovtEventSet(movtMsg);
			}
			else
			{
				movtMsg.id          = MOVT_MSG_MC_RECOVER_FORWARD;
                MovtEventSet(movtMsg);
			}
		}

		if(msg->load[0] == 0x02)
		{
			rtcApp.UtcTransformTime(((uint32_t)msg->load[1]<<24)+ ((uint32_t)msg->load[2]<<16) + ((uint32_t)msg->load[3]<<8) + ((uint32_t)msg->load[4]),
				&dailyRtc);

			dailyRtc.zone = 0;
			if((int8_t)msg->load[5] < 0)
			{
				temp = -(int8_t)msg->load[5];
			}
			else
			{
				temp = msg->load[5];
			}
			dailyRtc2.zone = ((temp / 10) << 8) + (temp % 10 * 10);

			rtcApp.TimeZoneTransform(&dailyRtc, &dailyRtc2);
			rtcApp.Write(&dailyRtc2);

			movtMsg.cur = ((uint16_t)msg->load[6] << 8) + msg->load[7];
			movtMsg.aim = ExchangeTimeforCount(dailyRtc2.hour, dailyRtc2.min, dailyRtc2.sec);
			
			movtMsg.id  = MOVT_MSG_MC_SET_CUR_FORWARD;
			MovtEventSet(movtMsg);
			if(msg->load[9] == 1)
			{
				movtMsg.id          = MOVT_MSG_MC_RECOVER_FORWARD;
				MovtEventSet(movtMsg);
			}
		}
	    BlePack(DEVICE_COM_ACK, msg);
		break;


		case APP_SET_BRO_NAME:
		if(msg->length < 16)
		{
			memset(m001BraodcastName, 0, sizeof(m001BraodcastName));
			memcpy(m001BraodcastName, (char*)msg->load, msg->length);
			modifyBleNameFlah = true;
		}
	    BlePack(DEVICE_COM_ACK, msg);
		break;


		case APP_REQ_HIS_DATA:
		switch(msg->load[0])
		{
			case HDATA_TOTAL_INFO:
			DailyDataRequestTotalInfoAck(((uint32_t)(msg->load[2]) << 8) + msg->load[3]);
			break;

			case HDATA_CATA_INFO:
			 DailyStepCatalogInfoRead(((uint32_t)(msg->load[1]) << 8) + msg->load[2],
                                        ((uint32_t)(msg->load[3]) << 8) + msg->load[4]);
			break;

			case HDATA_CATA_DATA:
			DailyStepRequestData(((uint32_t)(msg->load[1]) << 8) + msg->load[2],
                                    ((uint32_t)(msg->load[3]) << 24) + ((uint32_t)(msg->load[4]) << 16) +
                                    ((uint32_t)(msg->load[5]) << 8) + msg->load[6],
                                    ((uint32_t)(msg->load[7]) << 8) + msg->load[8]);
			break;

			case HDATA_DEL_ALL:
			DailyDataTotalDelete();
			break;

			case HDATA_DEL_TYPE:
			DailyDataDeleteClassify(((uint32_t)(msg->load[1]) << 8) + msg->load[2]);
			break;

			case HDATA_DEL_CATA:
			DailyDataDeleteCatalog(((uint32_t)(msg->load[1]) << 8) + msg->load[2],
                                    ((uint32_t)(msg->load[3]) << 24) + ((uint32_t)(msg->load[4]) << 16)+
                                    ((uint32_t)(msg->load[5]) << 8) + msg->load[6]);
			break;

		}
		break;


		case APP_FW_UPDATA:
		break;
		
		case APP_REQ_PEER:
		RequestPeer();
		break;

		case APP_REQ_BLE_INFO:
	    BlePack(DEVICE_REPLY_BLE, msg);
		break;


        default:
        break;

    }
}


void BleApp(uint32_t event)
{
	uint16_t  i;
	if(BLE_ADV_EVENT & event)
	{
		bleMode = BLE_BROADCAST_MODE;
		phoneState = PHONE_STATE_NORMAL;
	}


	if(BLE_CONNECT_EVENT & event)
	{
		bleMode = BLE_CONNECT_MODE;

	}


	if(BLE_DISCONNECT_EVENT & event)
	{
		bleMode = BLE_BROADCAST_MODE;
		if(modifyBleNameFlah == true)
		{
			ModifyBleName();
			modifyBleNameFlah = false;
		}
		//advertising_start();
	}


	if(BLE_COMMAND_EVENT & event)
	{
	    NRF_LOG_INFO("rec:%s\n", HexToSprintf((uint8_t*)&bleRecMsg, bleRecMsg.length+6));			
		BleProtocal(&bleRecMsg);
	}


	if(BLE_UPGRADE_EVENT & event)
	{

	}


	if(BLE_HIS_DATA_EVENT & event)
	{

	}

	if(BLE_SEND_FINISH & event)
	{
		DailyDataUploadProcess();
	}






}


uint8_t sprintfData[64];
uint8_t* HexToSprintf(uint8_t *data, uint16_t length)
{
	uint16_t k = 0, j = 0;
	uint8_t  temp1, temp2;
	while(length--)
	{
        
		temp2 = (data[j]>>4);
        temp1 = data[j]&0x0f;
        j++;
		if(temp2 > 9)
		{
			sprintfData[k] = temp2 + 'A'-10;
		}
		else
		{
			sprintfData[k] = temp2 + '0';
		}
		k++;
		
		if(temp1 > 9)
		{
			sprintfData[k] = temp1 + 'A'-10;
		}
		else
		{
			sprintfData[k] = temp1 + '0';
		}
		k++;
	}
	sprintfData[k] = '\0';
	return sprintfData;
}