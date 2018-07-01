#ifndef			DAILY_SPORT_H
#define			DAILY_SPORT_H

#define 		DAILY_SPORT_PERIOD_25HZ_DIV 	(25)//25HZ
#define 		DAILY_SPORT_PERIOD_100HZ_DIV 	(100)//100HZ


extern uint32_t dailyTotalStep;
extern uint32_t dailyStepAim;
extern uint16_t dailyStepComplete;


void DailyStepProcess(void);

void DailySportInit(void);

//void DailyActionReconProcess(void);

//void ClearBuf(void);
#endif			//DAILY_SPORT_H
