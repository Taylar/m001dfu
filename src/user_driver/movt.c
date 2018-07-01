#include "general.h"

#define     user_driver_pin_toggle(pin)     nrf_gpio_pin_toggle(pin)

// hard ware pin define
#define         MOVT_M_PINA                         MOVT_C_A
#define         MOVT_M_PINB                         MOVT_C_B



// movt state 
#define         MOVT_STANDBY                        0
#define         MOVT_BUSY                           1

// 3 point movt
#define         MOVE_M_CLOCK_FORWARD_MAX            32400
#define         MOVE_M_CLOCK_REVERSE_MAX            10800
#define         MOVE_M_CLOCK_MAX_RANGE              43200



movt_att_s  movtConfig[MOVT_MAX_NUM];

func    *MovtForwardFinishIsr[MOVT_MAX_NUM];
func    *MovtResverseFinishIsr[MOVT_MAX_NUM];

void MovtclockCompare(movt_att_s *movtCompare);





// 31us every cnt
static const uint16_t tinyForwardClock_M[] = 
{
	200*31,6*31,14*31,14*31,14*31,14*31,14*31,14*31,14*31,14*31,14*31,14*31,14*31,6*31,100*31,255*31
};

static const uint16_t tinyReverseClock_M[] = 
{
	255*31,7*31,27*31,34*31,112*31,112*31,13*31,13*31,13*31,13*31,13*31,13*31,13*31,13*31,255*31,255*31
};


void ClockMFrowardFinishCbInit(func *cb)
{
	MovtForwardFinishIsr[MOVT_M_CLOCK] = cb;
}

void ClockMResverseFinishCbInit(func *cb)
{
	MovtResverseFinishIsr[MOVT_M_CLOCK] = cb;
}



void BspMovtNormalTimerStart(uint32_t time_us)
{
	bsp_movt_timer_set(time_us);
	bsp_movt_timer_start();
}

void BspMovtNormalTimerStop(void)
{
	bsp_movt_timer_stop();
}





void BspMovtNorTimerIsr(void)
{
	if(movtConfig[MOVT_M_CLOCK].direction == MOVT_DIR_FORWARD)
	{
		switch(movtConfig[MOVT_M_CLOCK].movtStep)
		{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			movtConfig[MOVT_M_CLOCK].timerStop();
			movtConfig[MOVT_M_CLOCK].timerStart(movtConfig[MOVT_M_CLOCK].clockForward[movtConfig[MOVT_M_CLOCK].movtStep + 1]);
			user_driver_pin_toggle(MOVT_M_PINA);
			break;

			case 13:
			movtConfig[MOVT_M_CLOCK].timerStop();
			movtConfig[MOVT_M_CLOCK].timerStart(movtConfig[MOVT_M_CLOCK].clockForward[movtConfig[MOVT_M_CLOCK].movtStep + 1]);
			user_driver_pin_toggle(MOVT_M_PINB);
			break;

			case 14:
			movtConfig[MOVT_M_CLOCK].state  = MOVT_STANDBY;
			movtConfig[MOVT_M_CLOCK].movtStep = 0;
			movtConfig[MOVT_M_CLOCK].timerStop();


/*********clock finish, should be process in forward*************/

			MovtForwardFinishIsr[MOVT_M_CLOCK]();

/*********clock finish, should be process in forward*************/
			return;
			// break;

		}
		movtConfig[MOVT_M_CLOCK].movtStep++;
	}
	else
	{
		switch(movtConfig[MOVT_M_CLOCK].movtStep)
		{
			case 0://turn overone pin
			case 1:
			case 2:
			case 5: 
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			movtConfig[MOVT_M_CLOCK].timerStop();
			movtConfig[MOVT_M_CLOCK].timerStart(movtConfig[MOVT_M_CLOCK]. clockReverse[movtConfig[MOVT_M_CLOCK].movtStep + 1]);
			user_driver_pin_toggle(MOVT_M_PINA);
			break;
			case 3:
			case 4: 
			movtConfig[MOVT_M_CLOCK].timerStop();
			movtConfig[MOVT_M_CLOCK].timerStart(movtConfig[MOVT_M_CLOCK].clockReverse[movtConfig[MOVT_M_CLOCK].movtStep + 1]);
			user_driver_pin_toggle(MOVT_M_PINA);
			user_driver_pin_toggle(MOVT_M_PINB);
			break;
			case 13:
			movtConfig[MOVT_M_CLOCK].timerStop();
			movtConfig[MOVT_M_CLOCK].timerStart(movtConfig[MOVT_M_CLOCK].clockReverse[movtConfig[MOVT_M_CLOCK].movtStep + 1]);
			user_driver_pin_toggle(MOVT_M_PINB);
			break;
									
			case 14:  //clock run finish 
			movtConfig[MOVT_M_CLOCK].state  = MOVT_STANDBY;
			movtConfig[MOVT_M_CLOCK].movtStep = 0;
			movtConfig[MOVT_M_CLOCK].timerStop();

/*********clock finish, should be process in forward*************/
			MovtResverseFinishIsr[MOVT_M_CLOCK]();

/*********clock finish, should be process in forward*************/
			return;
			// break;

		}
		movtConfig[MOVT_M_CLOCK].movtStep++;
	}
		
}






void MovtclockCompare(movt_att_s *movtCompare)
{
	if(movtCompare->stop || movtCompare->state == MOVT_BUSY) // movt is stop state or busy state return
		return;
	if(movtCompare->repeat)
	{
		if(movtCompare->state == MOVT_STANDBY)
		{
			movtCompare->state      = MOVT_BUSY;
			movtCompare->direction  = movtCompare->tempDirection;
			if(movtCompare->direction == MOVT_DIR_FORWARD)
			{
				movtCompare->timerStart(movtCompare->clockForward[0]);
			}
			else
			{
				movtCompare->timerStart(movtCompare->clockReverse[0]);
			}
			
		}
	}
	else
	{
		 //movt is reach the aim position will return
		if(movtCompare->currentPosition  == movtCompare->aimPosition)
			return;
		if(movtCompare->directionSet == MOVT_DIR_AUTO)
		{
			if(movtCompare->currentPosition > movtCompare->aimPosition)
			{
				if((movtCompare->currentPosition - movtCompare->aimPosition) > movtCompare->movtReverseMax)
				{       // forward run
					movtCompare->direction = MOVT_DIR_FORWARD;
					
				}
				else        //reserse run
				{
					movtCompare->direction = MOVT_DIR_REVERSE;
				}
			}
			else
			{
				if(movtCompare->aimPosition - movtCompare->currentPosition > movtCompare->movtForwardMax)
				{       //reserse run
					movtCompare->direction = MOVT_DIR_REVERSE;
				}
				else    //forward run
				{
					movtCompare->direction = MOVT_DIR_FORWARD;
				}
			}
		}
		else
		{
			movtCompare->direction  = movtCompare->tempDirection;
		}
		if(movtCompare->state == MOVT_STANDBY)
		{
			movtCompare->state      = MOVT_BUSY;

			if(movtCompare->directionSet == MOVT_DIR_SETTING)
			{
				movtCompare->direction  = movtCompare->tempDirection;
			}
			if(movtCompare->direction == MOVT_DIR_FORWARD)
			{
				movtCompare->timerStart(movtCompare->clockForward[0]);
			}
			else
			{
				movtCompare->timerStart(movtCompare->clockReverse[0]);
			}
		}
	}
}



uint16_t BspMovtRunDiretion(movt_att_s *movtCompare)
{

	if(movtCompare->repeat)
	{
		if(movtCompare->state == MOVT_STANDBY)
		{
			return movtCompare->direction;			
		}
	}
	else
	{
		 //movt is reach the aim position will return
		if(movtCompare->currentPosition  == movtCompare->aimPosition)
			return 0xff;
		if(movtCompare->directionSet == MOVT_DIR_AUTO)
		{
			if(movtCompare->currentPosition > movtCompare->aimPosition)
			{
				if((movtCompare->currentPosition - movtCompare->aimPosition) > movtCompare->movtReverseMax)
				{		// forward run
					return MOVT_DIR_FORWARD;
					
				}
				else		//reserse run
				{
					return  MOVT_DIR_REVERSE;
				}
			}
			else
			{
				if(movtCompare->aimPosition - movtCompare->currentPosition > movtCompare->movtForwardMax)
				{		//reserse run
					return  MOVT_DIR_REVERSE;
				}
				else	//forward run
				{
					return  MOVT_DIR_FORWARD;
				}
			}
		}
		else
		{
			return  movtCompare->tempDirection;
		}
	}
	return movtCompare->direction;
}


void timer_movt_event_handler(nrf_timer_event_t event_type, void* p_context)
{

    switch (event_type)
    {
        case NRF_TIMER_EVENT_COMPARE0:
            BspMovtNorTimerIsr();
            break;

        default:
            //Do nothing.
            break;
    }
}


void BspMovtInit(void)
{
	uint8_t     i;
	
	bsp_board_movts_init();

	bsp_movt_timer_init(timer_movt_event_handler);



	movtConfig[MOVT_M_CLOCK].currentPosition    = 0;
	movtConfig[MOVT_M_CLOCK].aimPosition        = 0;
	movtConfig[MOVT_M_CLOCK].movtStep           = 0;
	movtConfig[MOVT_M_CLOCK].movtForwardMax     = MOVE_M_CLOCK_FORWARD_MAX;
	movtConfig[MOVT_M_CLOCK].movtReverseMax     = MOVE_M_CLOCK_REVERSE_MAX;
	movtConfig[MOVT_M_CLOCK].movtRangeMax       = MOVE_M_CLOCK_MAX_RANGE;
	movtConfig[MOVT_M_CLOCK].tinyTimeCnt        = 0;
	movtConfig[MOVT_M_CLOCK].direction          = MOVT_DIR_FORWARD;
	movtConfig[MOVT_M_CLOCK].tempDirection      = MOVT_DIR_FORWARD;
	movtConfig[MOVT_M_CLOCK].directionSet       = MOVT_DIR_AUTO;
	movtConfig[MOVT_M_CLOCK].state              = MOVT_STANDBY;
	movtConfig[MOVT_M_CLOCK].repeat             = 0;
	movtConfig[MOVT_M_CLOCK].stop               = 0;
	movtConfig[MOVT_M_CLOCK].clockForward       = (uint16_t *)tinyForwardClock_M;
	movtConfig[MOVT_M_CLOCK].clockReverse       = (uint16_t *)tinyReverseClock_M;
	movtConfig[MOVT_M_CLOCK].timerStart         = BspMovtNormalTimerStart;
	movtConfig[MOVT_M_CLOCK].timerStop          = BspMovtNormalTimerStop;


	for(i = 0; i < MOVT_MAX_NUM; i++)
	{
		MovtForwardFinishIsr[i]     = NULL;
		MovtResverseFinishIsr[i]    = NULL;
	}
}


// movt
const bsp_movt_s		bsp_movt = 
{
	.clockM							= &movtConfig[MOVT_M_CLOCK],
	.Init							= BspMovtInit,
	.ClockMFrowardFinishCbInit		= ClockMFrowardFinishCbInit,
	.ClockMResverseFinishCbInit		= ClockMResverseFinishCbInit,
	.Compare						= MovtclockCompare,
	.ReadRunDir						= BspMovtRunDiretion,
	// .SelfTest						= BspMovtSelfTest,
};
