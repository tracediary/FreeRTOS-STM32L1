#ifndef __RTC_H
#define __RTC_H
//#include "sys.h"
#include "stm32l1xx.h"
#include "iwdg.h"
#include "Debug.h"
#include "hardware_version.h"

#define RTC_CLOCK_SOURCE_LSE


//字符串处理时用到
#define ASCII_NUMBER_MIN 	( '0' - 1 )		//'0'的上一个字符
#define ASCII_NUMBER_MAX	( '9' + 1 )		//'9' 的下一个字符
#define TIME_STRING_LEN 	18			//2017-07-21,10:26:25长度-1

#define HOUR_UNIT 			3600
#define MINUTE_UNIT			60

#define DAY2MIN 			(60*24)
#define HOUR2MIN 			(60)

#define ONE_WEEK_TIME		( 7*24*60*60 )
//获取时间的格式
typedef struct _TimeType
{
	RTC_DateTypeDef RTC_DateStructure;		//日期
	RTC_TimeTypeDef RTC_TimeStructure;
}TimeType_s_t;

typedef enum
{
	REAL_TIME_OUT = 0,
	RTC_TIME_NORMAL
}RealTimeStatus_e;

/** 
  * @brief  RTC Alarm Time structure definition  
  */
typedef struct
{
  uint8_t RTC_Hours;    /*!< Specifies the RTC Alarm Time Hour.
                        This parameter must be set to a value in the 0-12 range
                        if the RTC_HourFormat_12 is selected or 0-23 range if
                        the RTC_HourFormat_24 is selected. */

  uint8_t RTC_Minutes;  /*!< Specifies the RTC Alarm Time Minutes.
                        This parameter must be set to a value in the 0-59 range. */
  
  uint8_t RTC_Seconds;  /*!< Specifies the RTC Alarm Time Seconds.
                        This parameter must be set to a value in the 0-59 range. */

  uint8_t RTC_Date;
}RTC_InspectTimeType_s_t; 



void RTC_Config(void);

void RTC_GetTime_struct( TimeType_s_t * rtc_get_realtime );
void RTC_Set_WKUPTime( RTC_TimeTypeDef );
uint8_t RTC_Set_InspectTime( RTC_InspectTimeType_s_t * );
void RTC_Set_RealTime( uint8_t *  );
void RTC_GetTime_string( int8_t * );


#endif
