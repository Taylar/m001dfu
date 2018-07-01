#ifndef			RTC_APP_H
#define			RTC_APP_H

typedef enum
{
	SUN		= 0,
	MON,
	TUE,
	WED,
	THU,
	FRI,
	SAT,
}week_t;


typedef enum
{
	JAN		= 1,
	FEB,
	MAR,
	APR,
	MAY,
	JUN,
	JUL,
	AUG,
	SEP,
	OCT,
	NOV,
	DEC,
} month_t;




typedef struct 
{
	uint8_t		year;
	uint8_t		month;
	uint8_t		day;
	uint8_t		hour;
	uint8_t		min;
	uint8_t		sec;
	uint8_t		week;
	uint16_t	zone;			//高8位为时区整数部分（-12---+12）低8位为小数部分（0--99）
}rtc_time_s;

typedef struct
{
	void		(*Init)(void);
	void		(*Start)(void);
	void		(*Stop)(void);
	void		(*Reset)(void);
	void		(*Read)(rtc_time_s *timeTemp);
	void		(*Write)(rtc_time_s *timeTemp);
	uint8_t		(*AutoWeek)(uint16_t year, uint8_t month, uint8_t day);
	void		(*SecPeriodProcess)(void);
	void		(*Cb_HalfSecIsrInit)(func *Cb);
	void		(*Cb_SecIsrInit)(func *Cb);
	void		(*Cb_MinIsrInit)(func *Cb);
	void		(*Cb_DayIsrInit)(func *Cb);
	uint16_t	(*TimeTransformUtc)(rtc_time_s *timeTemp, uint32_t *utcTemp);
	uint16_t	(*UtcTransformTime)(uint32_t utcTemp, rtc_time_s *timeTemp);
	uint16_t	(*TimeZoneTransform)(rtc_time_s *curTime, rtc_time_s *aimTime);
	uint32_t	(*Read_GMT_Utc)(void);
	uint32_t	(*Read_Cur_Utc)(void);
} rtc_s;

// rtc 从2000--2099（year = 0--99）
extern const rtc_s rtcApp;

void RtcInit(void);

void RtcStart(void);

void RtcStop(void);

void RtcTimeRead(rtc_time_s *timeTemp);

void RtcTimeWrite(rtc_time_s *timeTemp);

void RtcSecPeriodProcess(void);

uint8_t AutoWeek(uint16_t LocalYear,uint8_t LocalMonth,uint8_t LocalDay);

void RtcHalfSecCbInit(func *Cb);

void RtcSecCbInit(func *Cb);

void RtcMinCbInit(func *Cb);

void RtcDayCbInit(func *Cb);

uint16_t TimeTransformUtc(rtc_time_s *timeTemp, uint32_t *utcTemp);

uint16_t UtcTransformTime(uint32_t utcTemp, rtc_time_s *timeTemp);

uint16_t TimeZoneTransform(rtc_time_s *curTime, rtc_time_s *aimTime);

uint32_t Read_GMT_Utc(void);

uint32_t Read_Cur_Utc(void);


// rtc
// const rtc_s rtcApp =
// {
// 	.Init				= RtcInit,
// 	.Start				= RtcStart,
// 	.Stop				= RtcStop,
// 	.Reset				= RtcInit,
// 	.Read				= RtcTimeRead,
// 	.Write				= RtcTimeWrite,
// 	.SecPeriodProcess	= RtcSecPeriodProcess,
// 	.AutoWeek			= AutoWeek,
// 	.Cb_HalfSecIsrInit	= RtcHalfSecCbInit,
// 	.Cb_SecIsrInit		= RtcSecCbInit,
// 	.Cb_MinIsrInit		= RtcMinCbInit,
// 	.Cb_DayIsrInit		= RtcDayCbInit,
// 	.TimeTransformUtc	= TimeTransformUtc,
// 	.UtcTransformTime	= UtcTransformTime,
// 	.TimeZoneTransform	= TimeZoneTransform,
// 	.Read_GMT_Utc		= Read_GMT_Utc,
// 	.Read_Cur_Utc		= Read_Cur_Utc,
// };















#endif			//	RTC_APP_H
