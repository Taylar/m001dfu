#include "general.h"





void MovtMClockForwardFinish(void)
{

	bsp_movt.clockM->currentPosition ++;
	if(bsp_movt.clockM->currentPosition == bsp_movt.clockM->movtRangeMax)
	{
		bsp_movt.clockM->currentPosition = 0;
	}

	bsp_movt.Compare(bsp_movt.clockM);
}


void MovtMClockReverseFinish(void)
{

	if(bsp_movt.clockM->currentPosition > 0)
	{
		bsp_movt.clockM->currentPosition--;
	}
	else
	{
		bsp_movt.clockM->currentPosition = bsp_movt.clockM->movtRangeMax - 1;
	}

	bsp_movt.Compare(bsp_movt.clockM);
}



void ClockMFrowardFinishIsr(void)
{
	SetMovtEvent(MOVT_FORWARD_FINISH_EVENT);	
}


void ClockMResverseFinishIsr(void)
{
	SetMovtEvent(MOVT_RESVERSE_FINISH_EVENT);	
}


void MovtAppInit(void)
{
	bsp_movt.Init();
	bsp_movt.ClockMFrowardFinishCbInit(ClockMFrowardFinishIsr);
	bsp_movt.ClockMResverseFinishCbInit(ClockMResverseFinishIsr);

}

// init the task, the drive interface
void MovtEventSet(movt_task_msg_t msg)
{
	switch(msg.id)
	{
// clock M
		case MOVT_MSG_MC_FORWARD:
		bsp_movt.clockM->aimPosition++;

		if(bsp_movt.clockM->aimPosition >= bsp_movt.clockM->movtRangeMax)
			bsp_movt.clockM->aimPosition 	= 0;

		bsp_movt.clockM->directionSet		= MOVT_DIR_AUTO;
		bsp_movt.clockM->repeat				= 0;
//                bsp_movt.clockM->stop               = 0;
		bsp_movt.Compare(bsp_movt.clockM);
		break;
		
		case MOVT_MSG_MC_REVERSE:
		if(bsp_movt.clockM->aimPosition == 0)
			bsp_movt.clockM->aimPosition = bsp_movt.clockM->movtRangeMax - 1;
		else
			bsp_movt.clockM->aimPosition--;

		bsp_movt.clockM->directionSet		= MOVT_DIR_AUTO;
		bsp_movt.clockM->repeat				= 0;
//                bsp_movt.clockM->stop               = 0;
		bsp_movt.Compare(bsp_movt.clockM);
		break;

		case MOVT_MSG_MC_FORWARDING:
		bsp_movt.clockM->directionSet		= MOVT_DIR_SETTING;
		bsp_movt.clockM->tempDirection		= MOVT_DIR_FORWARD;
		bsp_movt.clockM->repeat				= 1;
//                bsp_movt.clockM->stop               = 0;
		bsp_movt.Compare(bsp_movt.clockM);
		break;

		case MOVT_MSG_MC_REVERSEING:
		bsp_movt.clockM->directionSet		= MOVT_DIR_SETTING;
		bsp_movt.clockM->tempDirection		= MOVT_DIR_REVERSE;
		bsp_movt.clockM->repeat				= 1;
//                bsp_movt.clockM->stop               = 0;
		bsp_movt.Compare(bsp_movt.clockM);
		break;

		case MOVT_MSG_MC_SET_CUR:
		case MOVT_MSG_MC_SET_CUR_FORWARD:
		case MOVT_MSG_MC_SET_CUR_REVERSE:
		if(msg.id == MOVT_MSG_MC_SET_CUR)
		{
			bsp_movt.clockM->directionSet		= MOVT_DIR_AUTO;
		}
		else
		{
			bsp_movt.clockM->directionSet		= MOVT_DIR_SETTING;
			if(msg.id == MOVT_MSG_MC_SET_CUR_FORWARD)
				bsp_movt.clockM->tempDirection		= MOVT_DIR_FORWARD;
			else
				bsp_movt.clockM->tempDirection		= MOVT_DIR_REVERSE;
		}

		bsp_movt.clockM->currentPosition	= msg.cur;
		if(bsp_movt.clockM->currentPosition >= bsp_movt.clockM->movtRangeMax)
			bsp_movt.clockM->currentPosition 	= bsp_movt.clockM->currentPosition%bsp_movt.clockM->movtRangeMax;
		bsp_movt.clockM->repeat				= 0;
//                bsp_movt.clockM->stop               = 0;
		bsp_movt.Compare(bsp_movt.clockM);
		break;

		case MOVT_MSG_MC_SET_AIM:
		case MOVT_MSG_MC_SET_AIM_FORWARD:
		case MOVT_MSG_MC_SET_AIM_REVERSE:
		if(msg.id == MOVT_MSG_MC_SET_AIM)
		{
			bsp_movt.clockM->directionSet		= MOVT_DIR_AUTO;
		}
		else
		{
			bsp_movt.clockM->directionSet		= MOVT_DIR_SETTING;
			if(msg.id == MOVT_MSG_MC_SET_AIM_FORWARD)
				bsp_movt.clockM->tempDirection		= MOVT_DIR_FORWARD;
			else
				bsp_movt.clockM->tempDirection		= MOVT_DIR_REVERSE;
		}

		bsp_movt.clockM->aimPosition		= msg.aim;
		if(bsp_movt.clockM->aimPosition >= bsp_movt.clockM->movtRangeMax)
			bsp_movt.clockM->aimPosition 	= bsp_movt.clockM->aimPosition%bsp_movt.clockM->movtRangeMax;

		bsp_movt.clockM->repeat				= 0;
//                bsp_movt.clockM->stop               = 0;
		bsp_movt.Compare(bsp_movt.clockM);
		break;

		case MOVT_MSG_MC_SET_CUR_AIM:
		case MOVT_MSG_MC_SET_CUR_AIM_FORWARD:
		case MOVT_MSG_MC_SET_CUR_AIM_REVERSE:
		if(msg.id == MOVT_MSG_MC_SET_CUR_AIM)
		{
			bsp_movt.clockM->directionSet		= MOVT_DIR_AUTO;
		}
		else
		{
			bsp_movt.clockM->directionSet		= MOVT_DIR_SETTING;
			if(msg.id == MOVT_MSG_MC_SET_CUR_AIM_FORWARD)
				bsp_movt.clockM->tempDirection		= MOVT_DIR_FORWARD;
			else
				bsp_movt.clockM->tempDirection		= MOVT_DIR_REVERSE;
		}

		bsp_movt.clockM->currentPosition		= msg.cur;
		bsp_movt.clockM->aimPosition			= msg.aim;
		if(bsp_movt.clockM->currentPosition >= bsp_movt.clockM->movtRangeMax)
			bsp_movt.clockM->currentPosition 	= bsp_movt.clockM->currentPosition%bsp_movt.clockM->movtRangeMax;

		if(bsp_movt.clockM->aimPosition >= bsp_movt.clockM->movtRangeMax)
			bsp_movt.clockM->aimPosition 	= bsp_movt.clockM->aimPosition%bsp_movt.clockM->movtRangeMax;


		bsp_movt.clockM->repeat				= 0;
//                bsp_movt.clockM->stop               = 0;
		bsp_movt.Compare(bsp_movt.clockM);
		break;

		case MOVT_MSG_MC_FORWARD_FINISH:
		MovtMClockForwardFinish();
		break;

		case MOVT_MSG_MC_REVERSE_FINISH:
		MovtMClockReverseFinish();
		break;

		case MOVT_MSG_MC_STOP:
		bsp_movt.clockM->stop				= 1;
		break;
        case MOVT_MSG_MC_RECOVER:
        case MOVT_MSG_MC_RECOVER_FORWARD:
		case MOVT_MSG_MC_RECOVER_REVERSE:
		if(msg.id == MOVT_MSG_MC_RECOVER)
		{
			bsp_movt.clockM->directionSet		= MOVT_DIR_AUTO;
		}
		else
		{
			bsp_movt.clockM->directionSet		= MOVT_DIR_SETTING;
			if(msg.id == MOVT_MSG_MC_RECOVER_FORWARD)
				bsp_movt.clockM->tempDirection		= MOVT_DIR_FORWARD;
			else
				bsp_movt.clockM->tempDirection		= MOVT_DIR_REVERSE;
		}

        bsp_movt.clockM->stop               = 0;
        bsp_movt.clockM->repeat             = 0;
        bsp_movt.Compare(bsp_movt.clockM);
        break;

		default:
		break;
	}
}



uint16_t GetMovtCurPos(void)
{
	return bsp_movt.clockM->currentPosition;
}


uint16_t GetMovtAimPos(void)
{
	return bsp_movt.clockM->aimPosition;
}
