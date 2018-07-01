#ifndef				RTC_H
#define				RTC_H



typedef struct
{
	void		(*BspInit)(void);
	void		(*Start)(void);
	void		(*Stop)(void);
	void		(*CbIsrInit)(func *Cb);
} bsp_rtc_s;

extern const bsp_rtc_s bspRtc;

// function declaration
void BspRtcCbIsrInit(func *Cb);

void BspRtcInit(void);

void BspRtcTimerStart(void);

void BspRtcTimerStop(void);

#endif		// RTC_H
