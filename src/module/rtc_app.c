#include "general.h"


func *RtcHalfSecIsr;
func *RtcSecIsr;
func *RtcMinIsr;
func *RtcDayIsr;



// 2000年-2099年的闰年标记与第一天的星期
static const uint8_t Table_YearFirstDay[] =
{
	0x0e,0x01,0x02,0x03,0x0c,0x06,0x00,0x01,0x0a,0x04,
	0x05,0x06,0x08,0x02,0x03,0x04,0x0d,0x00,0x01,0x02,
	0x0b,0x05,0x06,0x00,0x09,0x03,0x04,0x05,0x0e,0x01,
	0x02,0x03,0x0c,0x06,0x00,0x01,0x0a,0x04,0x05,0x06,
	0x08,0x02,0x03,0x04,0x0d,0x00,0x01,0x02,0x0b,0x05,
	0x06,0x00,0x09,0x03,0x04,0x05,0x0e,0x01,0x02,0x03,
	0x0c,0x06,0x00,0x01,0x0a,0x04,0x05,0x06,0x08,0x02,
	0x03,0x04,0x0d,0x00,0x01,0x02,0x0b,0x05,0x06,0x00,
	0x09,0x03,0x04,0x05,0x0e,0x01,0x02,0x03,0x0c,0x06,
	0x00,0x01,0x0a,0x04,0x05,0x06,0x08,0x02,0x03,0x04,
};
////////////////////////////////////////////////////////
static const uint16_t Table_Monthdyas[] =
{
//	31,28,31,30,31,30,31,31,30,31,30,31
	0,0,31,59,90,120,151,181,212,243,273,304,334,365
};


////////////////////////////////////////////////////////
static const uint32_t Table_YearFirstDayUTC[101] =
{
	946684800,978307200,1009843200,1041379200,1072915200,1104537600,1136073600,1167609600,1199145600,1230768000,
	1262304000,1293840000,1325376000,1356998400,1388534400,1420070400,1451606400,1483228800,1514764800,1546300800,
	1577836800,1609459200,1640995200,1672531200,1704067200,1735689600,1767225600,1798761600,1830297600,1861920000,
	1893456000,1924992000,1956528000,1988150400,2019686400,2051222400,2082758400,2114380800,2145916800,2177452800,
	2208988800,2240611200,2272147200,2303683200,2335219200,2366841600,2398377600,2429913600,2461449600,2493072000,
	2524608000,2556144000,2587680000,2619302400,2650838400,2682374400,2713910400,2745532800,2777068800,2808604800,
	2840140800,2871763200,2903299200,2934835200,2966371200,2997993600,3029529600,3061065600,3092601600,3124224000,
	3155760000,3187296000,3218832000,3250454400,3281990400,3313526400,3345062400,3376684800,3408220800,3439756800,
	3471292800,3502915200,3534451200,3565987200,3597523200,3629145600,3660681600,3692217600,3723753600,3755376000,
	3786912000,3818448000,3849984000,3881606400,3913142400,3944678400,3976214400,4007836800,4039372800,4070908800,
	4102444800,
};



// ******************************************************************************
// *Funtion name:AutoWeek
// *Description :
// *
// *Input: data
// *Output:None
// /*****************************************************************************
 uint8_t AutoWeek(uint16_t LocalYear,uint8_t LocalMonth,uint8_t LocalDay)
{
	uint16_t Intervaldays ;
	LocalYear = Table_YearFirstDay[(uint8_t)LocalYear % 100];
	Intervaldays = Table_Monthdyas[LocalMonth];
	if((LocalYear & 0x08) && (LocalMonth > 2))
	{
        Intervaldays++;
	}
	Intervaldays += LocalDay + (LocalYear & 0x07) - 1;
	Intervaldays = Intervaldays % 7;					
	return Intervaldays;						//0:Sunday
}








 void RtcHalfSecCbInit(func *Cb)
{
	RtcHalfSecIsr	= Cb;
}

 void RtcSecCbInit(func *Cb)
{
	RtcSecIsr		= Cb;
}

 void RtcMinCbInit(func *Cb)
{
	RtcMinIsr		= Cb;
}

 void RtcDayCbInit(func *Cb)
{
	RtcDayIsr		= Cb;
}


//static uint8_t	HalfSecCnt = 0;
//static uint8_t	SecCnt     = 0;

void RtcIsr(void)
{
	if(RtcSecIsr)
		RtcSecIsr();
	
	// HalfSecCnt++;
	// if(HalfSecCnt > 1)
	// {
	// 	HalfSecCnt	= 0;
	// 	if(RtcSecIsr)
	// 		RtcSecIsr();
	// }
	// else
	// {
	// 	if(RtcHalfSecIsr)
	// 		RtcHalfSecIsr();
	// }
}

static rtc_time_s		rtcTime;

 void RtcInit(void)
{
	static uint8_t hasInit	= 0;

	if(hasInit)
		return;
	bspRtc.BspInit();
	bspRtc.CbIsrInit(RtcIsr);
//	HalfSecCnt		= 1;
	rtcTime.year  	= 0;
	rtcTime.month	= 1;
	rtcTime.day		= 1;
	rtcTime.hour    = 0;
	rtcTime.min		= 0;
	rtcTime.sec		= 0;
	rtcTime.zone    = 0x0800;
	rtcTime.week 	= SAT;



	hasInit			= 1;
}


 void RtcTimeRead(rtc_time_s *timeTemp)
{
	timeTemp->year  = rtcTime.year;
	timeTemp->month = rtcTime.month;
	timeTemp->day   = rtcTime.day;
	timeTemp->hour  = rtcTime.hour;
	timeTemp->min   = rtcTime.min;
	timeTemp->sec   = rtcTime.sec;
	timeTemp->zone  = rtcTime.zone;
	timeTemp->week  = rtcTime.week;
}


 void RtcTimeWrite(rtc_time_s *timeTemp)
{
	rtcTime.year	= timeTemp->year;
	rtcTime.month   = timeTemp->month;
	rtcTime.day     = timeTemp->day;
	rtcTime.hour	= timeTemp->hour;
	rtcTime.min     = timeTemp->min; 
	rtcTime.sec     = timeTemp->sec;
	rtcTime.zone    = timeTemp->zone;
	rtcTime.week	= AutoWeek(rtcTime.year, rtcTime.month, rtcTime.day);
}


// *******************************************************************************
// *Funtion name: MonthDay
// *Description :
// *
// *Input: the month and year
// *Output:the last day of the month
// /******************************************************************************
uint8_t MonthDay(uint16_t year_1, uint8_t month_1)
{
	uint8_t maxday;
	if((year_1%4) == 0)
	{
		if(year_1 % 100)
			maxday = 29;
		else
		{
			if(((year_1+2000)%400 == 0))
			{
				maxday = 29;
			}
			else
			{
				maxday = 28;
			}
		}
	}
	else
	{
		maxday = 28;
	}
	switch(month_1)
	{
		case 1:
			return(31);
		case 2:
			return(maxday);
		case 3:
			return(31);
		case 4:
			return (30);
		case 5:
			return(31);
		case 6:
			return(30);
		case 7:
			return(31);
		case 8:
			return (31);
		case 9:
			return(30);
		case 10:
			return(31);
		case 11:
			return(30);
		case 12:
			return (31);
		default:
			break;
	}
	return 0xff;
}


 void RtcSecPeriodProcess(void)
{
	rtcTime.sec ++;
	if(rtcTime.sec > 59)
	{
		rtcTime.min ++;
		rtcTime.sec = 0;
		RtcMinIsr();
		if(rtcTime.min > 59)
		{
			rtcTime.min = 0;
			rtcTime.hour ++;
			if(rtcTime.hour > 23)
			{
				rtcTime.hour = 0;
				rtcTime.day ++;
				RtcDayIsr();
				if(rtcTime.day > MonthDay(rtcTime.year, rtcTime.month))
				{
					rtcTime.day = 1;
					rtcTime.month ++;
					if(rtcTime.month > 12)
					{
						rtcTime.month = 1;
						rtcTime.year ++;
						if(rtcTime.year > 99)
						{
							rtcTime.year = 0;
						}
//every Year
					}
//every Month
				}
//every Day
				rtcTime.week = AutoWeek(rtcTime.year, rtcTime.month, rtcTime.day);
			}
//every Hour
		}
//every min
	}
}

 void RtcStart(void)
{
	bspRtc.Start();
}

 void RtcStop(void)
{
	bspRtc.Stop();
}



// ******************************************************************************
// *Funtion name:TimeTransformUtc
// *Description :将时间转换为UTC时间，不考虑时区
// *
// *Input: 当前时间信息,不需要时区信息
// *Output:	0x00:	转换成功
// 			0xff:	转换失败
// /*****************************************************************************
uint16_t TimeTransformUtc(rtc_time_s *timeTemp, uint32_t *utcTemp)
{
	uint32_t	CurdayCnt;
	uint32_t	utcTemp2;
	utcTemp2		= Table_YearFirstDayUTC[timeTemp->year];
	
	CurdayCnt		= Table_Monthdyas[timeTemp->month];

	// 闰年且大于2月多加一天
	if((timeTemp->month > 2) && (Table_YearFirstDay[timeTemp->year] & 0x08))
		CurdayCnt++;
	CurdayCnt		+= timeTemp->day - 1;

// 当前时区的UTC时间
	utcTemp2			= utcTemp2 + (uint32_t)CurdayCnt * 86400 
						+ ((uint32_t)timeTemp->hour*60 + (uint32_t)timeTemp->min)*60 + timeTemp->sec;

	*utcTemp		= utcTemp2;
	return 0;
}

// ******************************************************************************
// *Funtion name:UtcTransformTime
// *Description :将UTC时间转换为
// *
// *Input: 当前时间信息，包含时区
// *Output:返回0时区的UTC时间
// /*****************************************************************************
uint16_t UtcTransformTime(uint32_t utcTemp, rtc_time_s *timeTemp)
{
	uint16_t i;
	uint16_t dayCntTemp;
	uint16_t flagTemp;
	
	flagTemp	= 0;

	// 从2001年开始判断
	for(i = 1; i <= 100; i++)
	{
		if(utcTemp < Table_YearFirstDayUTC[i])
		{
			timeTemp->year = i - 1;
			break;
		}
	}
	if(i == 101)
		return 0xff;

	// 整数天数
	dayCntTemp	= (utcTemp - Table_YearFirstDayUTC[timeTemp->year]) / 86400;

	// 判断是否大于2月28日
	if(dayCntTemp >= 59)
	{
		// 判断是否闰年
		if(Table_YearFirstDay[timeTemp->year] & 0x08)
		{
			// 减去闰月多出来的一天
			dayCntTemp--;
			flagTemp	= 1;
		}

	}

	// 从1月开始查找月份
	for(i = 2; i <= 13; i++)
	{
		if(dayCntTemp < Table_Monthdyas[i])
		{
			timeTemp->month		= i - 1;
			timeTemp->day		= dayCntTemp - Table_Monthdyas[i - 1] + 1;

			if((flagTemp) && (dayCntTemp == 58))
			{
				timeTemp->day	= 29;
			}
			break;
		}
	}

	// 当天剩余的秒
	utcTemp				= utcTemp % 86400;
	timeTemp->hour		= utcTemp / 3600;

	// 当前小时剩余的秒
	utcTemp				= utcTemp % 3600;
	timeTemp->min		= utcTemp / 60;
	timeTemp->sec		= utcTemp % 60;
	
	return 0;
}



// ******************************************************************************
// *Funtion name:TimeZoneTransform
// *Description :根据输入curtime信息，转换为目标时区时间
// *
// *Input:	curTime：当前时间，需要输入正确的当前时间与时区
// *		aimTime：需输入目标时区，其他时间信息不需要输入，
// *Output:	0x00:	转换成功
// *		0xff:	转换失败
// /*****************************************************************************
uint16_t TimeZoneTransform(rtc_time_s *curTime, rtc_time_s *aimTime)
{
	uint32_t	utcTemp, utcTemp2;
	int8_t		curZoneIntTemp, aimZoneIntTemp, zoneIntTemp;
	uint8_t		curZoneFractionTemp, aimZoneFractionTemp, zoneFractionTemp;
	uint8_t		curZoneLargeThanaimZone;
	// 将时间转换为UTC
	if(TimeTransformUtc(curTime, &utcTemp) == 0xff)
		return 0xff;
	// 将时区范围由（-12--+12）转换为（0--24）
	curZoneIntTemp		= (int8_t)(curTime->zone >> 8) + 12;
	aimZoneIntTemp		= (int8_t)(aimTime->zone >> 8) + 12;

	curZoneFractionTemp	= (uint8_t)(curTime->zone);
	aimZoneFractionTemp	= (uint8_t)(aimTime->zone);

	if(curZoneIntTemp >= aimZoneIntTemp)
	{
		if(curZoneIntTemp == aimZoneIntTemp)
		{
			if(curZoneFractionTemp >= aimZoneFractionTemp)
			{
				curZoneLargeThanaimZone		= 1;
			}
			else
			{
				curZoneLargeThanaimZone		= 0;
			}
		}
		else
		{
			curZoneLargeThanaimZone			= 1;
		}
	}
	else
	{
		curZoneLargeThanaimZone				= 0;
	}

	if(curZoneLargeThanaimZone)
	{
		zoneIntTemp		= curZoneIntTemp - aimZoneIntTemp;
		if(curZoneFractionTemp >= aimZoneFractionTemp)
		{
			zoneFractionTemp	= curZoneFractionTemp - aimZoneFractionTemp;
		}
		else
		{
			zoneFractionTemp	= 100 + curZoneFractionTemp - aimZoneFractionTemp;
			zoneIntTemp--;
		}
	}
	else
	{
		zoneIntTemp		= aimZoneIntTemp - curZoneIntTemp;
		if(aimZoneFractionTemp >= curZoneFractionTemp)
		{
			zoneFractionTemp	= aimZoneFractionTemp - curZoneFractionTemp;
		}
		else
		{
			zoneFractionTemp	= 100 + aimZoneFractionTemp - curZoneFractionTemp;
			zoneIntTemp--;
		}
	}

	utcTemp2	= (uint32_t)zoneIntTemp * 3600 + (uint32_t)zoneFractionTemp * 3600 / 100;

	if(curZoneLargeThanaimZone)
	{
		utcTemp	= utcTemp - utcTemp2;
	}
	else
	{
		utcTemp	= utcTemp + utcTemp2;
	}

	return UtcTransformTime(utcTemp, aimTime);
}


// ******************************************************************************
// *Funtion name:Read_GMT_Utc
// *Description :返回0时区的UTC时间
// *
// *Input: data
// *Output:None
// /*****************************************************************************
uint32_t Read_GMT_Utc(void)
{
	rtc_time_s		timeTemp;
	uint32_t		utcTemp;
	// 目的时区为零时区
	timeTemp.zone	= 0;

	if(TimeZoneTransform(&rtcTime, &timeTemp) == 0xff)
		return 0;

	TimeTransformUtc(&timeTemp, &utcTemp);
	return utcTemp;
}



// ******************************************************************************
// *Funtion name:Read_Cur_Utc
// *Description :返回当前时间的UTC时间
// *
// *Input: none
// *Output: 0 : 转换失败
// 			其他：UTC时间
// /*****************************************************************************
uint32_t Read_Cur_Utc(void)
{
	uint32_t utcTemp;
	if(TimeTransformUtc(&rtcTime, &utcTemp) == 0x00)
		return utcTemp;
	else
		return 0;
}



// rtc
const rtc_s rtcApp =
{
	.Init				= RtcInit,
	.Start				= RtcStart,
	.Stop				= RtcStop,
	.Reset				= RtcInit,
	.Read				= RtcTimeRead,
	.Write				= RtcTimeWrite,
	.SecPeriodProcess	= RtcSecPeriodProcess,
	.AutoWeek			= AutoWeek,
	.Cb_HalfSecIsrInit	= RtcHalfSecCbInit,
	.Cb_SecIsrInit		= RtcSecCbInit,
	.Cb_MinIsrInit		= RtcMinCbInit,
	.Cb_DayIsrInit		= RtcDayCbInit,
	.TimeTransformUtc	= TimeTransformUtc,
	.UtcTransformTime	= UtcTransformTime,
	.TimeZoneTransform	= TimeZoneTransform,
	.Read_GMT_Utc		= Read_GMT_Utc,
	.Read_Cur_Utc		= Read_Cur_Utc,
};




