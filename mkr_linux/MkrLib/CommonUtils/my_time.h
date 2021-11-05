#ifndef __MYTIME__interface__
#define __MYTIME__interface__

#include <time.h>
#include <stdio.h>

#ifdef WIN32
#include <sys/timeb.h>
#else
#include<sys/time.h>
#endif
#include <stdint.h>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

/*http://stackoverflow.com/questions/6545254/difference-between-system-datetime-now-and-system-datetime-today
*/
std::time_t getEpochTime(const std::string& dateTime, const std::string& dateTimeFormat);
class TimeSpan;
class DateTime
{
	//bool use_utc;
	//time_t _time;
	uint64_t millis;       // milliseconds since Epoch
						   //uint64_t currentTimeMillis();

	struct tm* getTM();
public:
	static DateTime MinValue;
	//int Year, Month, Day, Hour, Minute, Second;
	uint64_t getMillis() { return millis; }
	static uint64_t getMidNightMillis(uint64_t ts);
    static int GetMinutesFromMidNight(int64_t ts);
	DateTime() { millis = 0; /*use_utc = false;*/ } /*  -time=24 * 60 * 60L; time for Jan 2, 1900 00:00 UTC */
	DateTime(uint64_t t, bool utc_flag = false) { millis = t; /*use_utc = utc_flag;*/ }
	//DateTime(int year, int month, int day, int hour, int min, int sec);
	//DateTime(int year, int month, int day);
	static DateTime Now();
	static DateTime UtcNow();
//	static DateTime parse(std::string str);
	static std::string ToStringFromMillis(int64_t millis);
	static std::string DateStrFromMillis(int64_t millis);
	static std::string DateStrFromMillis_1(int64_t millis);
    static std::string DateHMStrFromMillis(int64_t millis);
 //   static std::string DateStrDbFromMillis(int64_t millis);
    static std::string TimeHMStringFromMillis(int64_t _ms) ;
	//const std::string currentDateTime();
	std::string ToString();
	
	std::string to_string();
    std::string to_file_string();
	DateTime AddDays(int h);
	DateTime AddHours(int h);
	DateTime AddMinutes(int m);
	DateTime AddSeconds(int sec);
	TimeSpan operator-(DateTime dt);
	bool operator==(DateTime dt);
	bool operator!=(DateTime dt);
	bool operator>(DateTime dt);
	bool operator<(DateTime dt);
	bool operator<=(DateTime dt);
	bool operator>=(DateTime dt);

	int CompareTo(DateTime dt );  //int CompareTo

	TimeSpan Subtract(DateTime dt);

	bool HasValue();

	DateTime Date();  //!! when I define result as a reference ( aka DateTime& Date();) the folllowing does not work ( DateTime dt14 = dt3.Date().AddDays(-1); )

	int Hour();
	int Second();
	int Minute();
	int Month();
	int Day();
	int Year();
	DateTime SkipMs();
};

class TimeSpan
{
	int val;  // number of seconds
public:
	static TimeSpan MaxValue;
	static TimeSpan MinValue;
	static TimeSpan Zero;
	TimeSpan() { val = 0; }
	TimeSpan(int v) { val = v; }
	TimeSpan(int h, int m, int s);
	TimeSpan(int days,int h, int m, int s);

	TimeSpan Add(TimeSpan ts);

	double TotalHours();
	double TotalMinutes();
	double TotalSeconds();

	TimeSpan operator+=(TimeSpan ts);
	TimeSpan operator-(TimeSpan ts);
	TimeSpan operator+(TimeSpan ts);
	bool operator<=(TimeSpan ts);
	bool operator==(TimeSpan ts);
	bool operator!=(TimeSpan ts);
	bool operator<(TimeSpan ts);
	bool operator>(TimeSpan ts);
	std::string ToString();
};

#endif