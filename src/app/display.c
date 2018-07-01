#include "general.h"

#define COMPLETE_START_POS		38
#define COMPLETE_END_POS		23


void DisplaySportComplete(uint16_t percent)
{
	uint8_t pinPos, tempPos;
	uint16_t curPosision;
    movt_task_msg_t msg;

	if(COMPLETE_START_POS > COMPLETE_END_POS)
	{
		pinPos = ((uint16_t)(COMPLETE_START_POS - COMPLETE_END_POS) * (uint16_t) percent / 100) + COMPLETE_END_POS;
	}
	else
	{
		pinPos = ((uint16_t)(COMPLETE_END_POS - COMPLETE_START_POS) * (uint16_t) percent / 100) + COMPLETE_START_POS;
	}

	curPosision = GetMovtCurPos();
	tempPos     = curPosision % 60;

	if(tempPos > pinPos)
	{
		curPosision = curPosision - tempPos +60 + pinPos;
	}
	else
	{
		curPosision = curPosision - tempPos + pinPos;
	}


	msg.aim 		= curPosision;
    msg.id          = MOVT_MSG_MC_SET_AIM;
    MovtEventSet(msg);

}
