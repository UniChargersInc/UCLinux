#include "TimeUtil.h"



namespace HosEngine
{

float TimeUtil::TimeZoneFromServer = 0;
int TimeUtil::ApplyDayTimeZone = 1;
int TimeUtil::DLSaving = 0;

	void TimeUtil::SetTimeZoneFromServer(float v, int applydts)
	{
		TimeZoneFromServer = v;
		ApplyDayTimeZone = applydts;
		if (TimeZoneFromServer == 0)
		{
			TimeZoneFromServer = getTimeZoneOffset(false);
		}
		if (applydts == 1)
		{
			DLSaving = getDayLightSaving();
		}

	}

	DateTime TimeUtil::DateTime_Now(int tz)
	{
		DateTime dt = DateTime::UtcNow();
		short tz1= (tz & 0xffff);
		TimeZoneFromServer =(float) tz1;
		DLSaving = 0;
		dt = dt.AddHours(TimeZoneFromServer + DLSaving);
		return dt;
	}

	DateTime TimeUtil::CurrentTime() 
	{ 
		DateTime dt = DateTime_Now();
		return dt;
	}

	DateTime TimeUtil::SkipMs(DateTime d)
	{
		return d.SkipMs();
	}




	int TimeUtil::getDayLightSaving()
	{
#if 0
		if (ApplyDayTimeZone == 1)
		{
			TimeZoneInfo *info = TimeZoneInfo::Local;
			int n = info->IsDaylightSavingTime(TimeUtil::DateTime_Now()) ? 1 : 0;
			return n;
		}
		else
		{
			return 0;
		}
#endif
		return 0;
	}

	std::string TimeUtil::LogTimeToString(DateTime logtime)
	{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
		//std::string rv = logtime.ToString("yyyyMMddHHmmss");
		return "";// rv;
	}

	DateTime TimeUtil::ParseLogTime(const std::string &logtime)
	{
		return DateTime::MinValue;    //ParseExact(logtime, "yyyyMMddHHmmss", MKR_NEW System::Globalization::CultureInfo("en-US"));
	}

	DateTime TimeUtil::ParseDateTime(const std::string &str, const std::string &format)
	{
		//CultureInfo *provider = CultureInfo::CurrentCulture;
		return DateTime::MinValue;  //ParseExact(str, format,provider);
	}

	std::string TimeUtil::ConvertMinutesToHoursandMinutes(int totalMinues)
	{
#if 0
		//return Math.Round(totalHours, 2).ToString();
		std::string sign = "";
		if (totalMinues < 0)
		{
			totalMinues = abs(totalMinues);
			sign = "-";
		}

		int hour_tol = static_cast<int>(floor(static_cast<double>(totalMinues / 60)));
		int minute_left = (totalMinues - hour_tol * 60);
		std::string dspStatus_on = std::string::Format("{0:d2}:{1:d2}", hour_tol, minute_left);
		return sign + dspStatus_on;
#endif
		return "";
	}

	std::string TimeUtil::GetTimeStr(DateTime dt)
	{
	
		std::string rv = "";// dt.ToLongDateString() + std::string(" ") + std::string::Format("{0:d2}:{1:d2}", dt.Hour, dt.Minute);
		return rv;
	}

	std::string TimeUtil::GetLongTimeStr(DateTime dt)
	{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
		return "";// dt.ToString("dddd, MMMM dd, yyyy"); //ToLongDateString ();
	}

	std::string TimeUtil::GetLongTimeStr_1(DateTime dt)
	{
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
		return "";// dt.ToString("0:ddd, MMM dd, yyyy");
	}

	int TimeUtil::getTimeZoneOffset(bool flag )
	{
		int rv = 0;
#if 0		
		TimeZoneInfo *info = TimeZoneInfo::Local;
		TimeSpan ts = info->BaseUtcOffset;

		rv = ts.Hours;
		//if (info.SupportsDaylightSavingTime)
		//	rv += 1;

		if (flag)
		{
			if (ApplyDayTimeZone == 1)
			{
				int n = info->IsDaylightSavingTime(TimeUtil::DateTime_Now()) ? 1 : 0;
				rv += n;
			}
		}
#endif
		return rv;
	}
}
