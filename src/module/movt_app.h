#ifndef			MOVTAPP_H
#define			MOVTAPP_H

#include "movt.h"



#ifndef			MOVT_ERROR
#define			MOVT_ERROR

#define			MOVT_M_ERROR							0X01
#define			MOVT_3_ERROR							0X02
#define			MOVT_6_ERROR							0X04
#define			MOVT_9_ERROR							0X08

#endif


typedef enum _movt_task_msg_id{
	MOVT_MSG_MC_FORWARD			= 0x0001,		//position 2clock msg
	MOVT_MSG_MC_REVERSE,
	MOVT_MSG_MC_FORWARDING,
	MOVT_MSG_MC_REVERSEING,
	MOVT_MSG_MC_SET_CUR,
	MOVT_MSG_MC_SET_AIM,
	MOVT_MSG_MC_SET_CUR_AIM,
	MOVT_MSG_MC_SET_CUR_FORWARD,
	MOVT_MSG_MC_SET_AIM_FORWARD,
	MOVT_MSG_MC_SET_CUR_AIM_FORWARD,
	MOVT_MSG_MC_SET_CUR_REVERSE,
	MOVT_MSG_MC_SET_AIM_REVERSE,
	MOVT_MSG_MC_SET_CUR_AIM_REVERSE,
	MOVT_MSG_MC_STOP,
	MOVT_MSG_MC_RECOVER,
	MOVT_MSG_MC_RECOVER_FORWARD,
	MOVT_MSG_MC_RECOVER_REVERSE,
	MOVT_MSG_MC_READ_CUR,
	MOVT_MSG_MC_READ_AIM,
	MOVT_MSG_MC_FORWARD_FINISH,
	MOVT_MSG_MC_REVERSE_FINISH,
} movt_task_msg_id;


typedef	struct
{
	uint16_t	id;
	uint16_t	cur;
	uint16_t	aim;
}movt_task_msg_t;


void MovtAppInit(void);

uint16_t MovtSelfTest(void);

void MovtEventSet(movt_task_msg_t msg);

void MovtMClockForwardFinish(void);

void MovtMClockReverseFinish(void);

uint16_t GetMovtCurPos(void);

uint16_t GetMovtAimPos(void);


#endif	//MOVTAPP_H
