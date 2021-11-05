#ifndef TimeUtil_H
#define TimeUtil_H

#include <string>
#include <cmath>
#include "CommonUtils/my_time.h"
#define USE_SIMPLE_MIN

namespace HosEngine
{
	class TimeUtil final
	{
	private:
		static float TimeZoneFromServer;
		static int ApplyDayTimeZone;
		static int DLSaving;
	public:
		static void SetTimeZoneFromServer(float v, int applydts);
		static DateTime DateTime_Now(int tz=0);

		static DateTime CurrentTime() ;
		static DateTime SkipMs(DateTime d);
		

		static int getDayLightSaving();

		static std::string LogTimeToString(DateTime logtime);
		static DateTime ParseLogTime(const std::string &logtime);
		static DateTime ParseDateTime(const std::string &str, const std::string &format);

		static std::string ConvertMinutesToHoursandMinutes(int totalMinues);
		static std::string GetTimeStr(DateTime dt);
		static std::string GetLongTimeStr(DateTime dt);
		static std::string GetLongTimeStr_1(DateTime dt);
		static int getTimeZoneOffset(bool flag = false);


	};
}

#endif