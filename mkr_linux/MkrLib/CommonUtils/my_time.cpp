#include "my_time.h"
#include "my_utils.h"

uint64_t currentTimeMillis()
{
#ifdef WIN32
	struct _timeb tt;
#ifdef _INC_TIME_INL
	_ftime_s(&t);
#else
	_ftime(&tt);
#endif
	return ((uint64_t)tt.time) * 1000 + tt.millitm;
#else
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	return ((uint64_t)tv.tv_sec) * 1000 + tv.tv_usec / 1000;
#endif
}


//===========================TimeSpan===============================
TimeSpan TimeSpan::MaxValue = TimeSpan(0x7FFFFFFF);
TimeSpan TimeSpan::MinValue = TimeSpan();
TimeSpan TimeSpan::Zero = TimeSpan();
TimeSpan::TimeSpan(int h, int m, int s)
{
	val = h * 60 * 60 + m * 60 + s;
}
TimeSpan::TimeSpan(int days, int h, int m, int s)
{
	int hh = days * 24 + h;
	val = hh * 60 * 60 + m * 60 + s;
}
double TimeSpan::TotalHours()
{
	return val / 3600.0;
}
double TimeSpan::TotalMinutes()
{
	return val / 60.0;
}
double TimeSpan::TotalSeconds()
{
	return val;
}
TimeSpan TimeSpan::Add(TimeSpan ts)
{
	int v=this->val + ts.val;
	return TimeSpan(v);
}
std::string TimeSpan::ToString()
{
	char buf[32] = { 0 };
	int h = this->val / 3600;
	int m = (this->val - (h * 3600)) / 60;
	int s = this->val - (h * 3600) - (m * 60);
	sprintf(buf, "%d:%d:%d", h, m, s);
	return buf;
}
TimeSpan TimeSpan::operator+=(TimeSpan ts)
{
	//int rr = this->val + ts.val;
	//TimeSpan ts1 = TimeSpan((int)rr);
	//return ts1;
	this->val += ts.val;
	return *this;
}
TimeSpan TimeSpan::operator-(TimeSpan ts)
{
	int rr = this->val - ts.val;
	TimeSpan ts1 = TimeSpan((int)rr);
	return ts1;
}
TimeSpan TimeSpan::operator+(TimeSpan ts)
{
	int rr = this->val + ts.val;
	TimeSpan ts1 = TimeSpan((int)rr);
	return ts1;
}
bool TimeSpan::operator<(TimeSpan ts)
{
	return (this->val < ts.val);
}
bool TimeSpan::operator>(TimeSpan ts)
{
	return (this->val > ts.val);
}
bool TimeSpan::operator<=(TimeSpan ts)
{
	return (this->val <= ts.val);
}
bool TimeSpan::operator==(TimeSpan ts)
{
	return (this->val == ts.val);
}
bool TimeSpan::operator!=(TimeSpan ts)
{
	return (this->val != ts.val);
}
//==========================DateTime================================
static string _StringFromMillis(int64_t _ms,const char* format) {
	time_t now = _ms / 1000;
    struct tm *tstruct;
   // bool use_utc=true;
    //if (use_utc)
        tstruct = gmtime(&now);
    //else
    //    tstruct = localtime(&now);
    
	char       buf[48] = { 0 };
	strftime(buf, sizeof(buf), format, tstruct); //http://linux.die.net/man/3/strftime
	return buf;
}

//https://stackoverflow.com/questions/4781852/how-to-convert-a-string-to-datetime-in-c
// Converts UTC time string to a time_t value.
std::time_t getEpochTime(const std::string& dateTime, const std::string& dateTimeFormat)
{
	// Let's consider we are getting all the input in
	// this format: '2014-07-25T20:17:22Z' (T denotes
	// start of Time part, Z denotes UTC zone).
	// A better approach would be to pass in the format as well.
	//static const std::string dateTimeFormat{ "%Y-%m-%dT%H:%M:%SZ" };

	// Create a stream which we will use to parse the string,
	// which we provide to constructor of stream to fill the buffer.
	std::istringstream ss{ dateTime };

	// Create a tm object to store the parsed date and time.
	std::tm dt;

	// Now we read from buffer using get_time manipulator
	// and formatting the input appropriately.
	ss >> std::get_time(&dt, dateTimeFormat.c_str());

	// Convert the tm structure to time_t value and return.
	return std::mktime(&dt);
}
uint64_t DateTime::getMidNightMillis(uint64_t ts)
{
	//DateTime dt(ts);
	//return  dt.Date().getMillis();
    uint64_t x=ts % (24*3600*1000);
    x= (ts-x);
    return x;
}
int DateTime::GetMinutesFromMidNight(int64_t ts)
{
    /*DateTime dt(ts);
    int64_t t1=dt.getMillis();
    int64_t t0=dt.Date().getMillis();
    int rv=(t1-t0)/(60*1000);
    return rv>0?rv:0;*/

    uint64_t x=ts % (24*3600*1000);
    int rv=x/(60*1000);
    return rv;
    
}
DateTime DateTime::MinValue = DateTime(/*1970,1,1*/0);
DateTime DateTime::UtcNow()
{
	DateTime dt(currentTimeMillis(), true);
	return dt;
}
DateTime DateTime::Now()
{
	DateTime dt(currentTimeMillis());
	return dt;
}
#if 0
DateTime DateTime::parse(std::string str)
{
	int year = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0;
	vector<string> ss1 = split(str, ' ');
	if (ss1.size() >= 2)
	{
		vector<string> xx = split(ss1[0], '-');
		if (xx.size() >= 3)
		{
			year = Str2Int(xx[0]);
			month = Str2Int(xx[1]);
			day = Str2Int(xx[2]);
		}
		vector<string> zz = split(ss1[1], ':');
		if (zz.size() >= 3)
		{
			hour = Str2Int(zz[0]);
			min = Str2Int(zz[1]);
			sec = Str2Int(zz[2]);
		}

	}


	DateTime dt(year,  month,  day,  hour,  min,  sec);
	return dt;
}
#endif
/*
// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::string DateTime::currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y-%m-%dT%T", &tstruct);

	return buf;
}*/
struct tm* DateTime::getTM()
{
	struct tm  *tstruct;
	time_t now = this->millis / 1000;
	//if (this->use_utc)
		tstruct = gmtime(&now);
	//else
	//	tstruct = localtime(&now);
	return tstruct;
}
/*http://stackoverflow.com/questions/7183761/sqlite-how-to-bind-and-insert-date-from-c
1.2 Date and Time Datatype

SQLite does not have a storage class set aside for storing dates and/or times. Instead, the built-in Date And Time Functions of SQLite are capable of storing dates and times as TEXT, REAL, or INTEGER values:
•TEXT as ISO8601 strings ("YYYY-MM-DD HH:MM:SS.SSS").
•REAL as Julian day numbers, the number of days since noon in Greenwich on November 24, 4714 B.C. according to the proleptic Gregorian calendar.
•INTEGER as Unix Time, the number of seconds since 1970-01-01 00:00:00 UTC.

Applications can chose to store dates and times in any of these formats and freely convert between formats using the built-in date and time functions.

*/
std::string DateTime::ToString() {
	//time_t     now = time(0);
	struct tm  *tstruct = getTM();
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	char       buf[48] = { 0 };
	//strftime(buf, sizeof(buf), "%Y-%m-%d.%X", tstruct);
	// SQLite expected date string format is "YYYY-MM-DD HH:MM:SS" (there are others too)
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tstruct);

	return buf;
}
std::string DateTime::ToStringFromMillis(int64_t _ms) {
	
	return _StringFromMillis(_ms, "%Y-%m-%d %H:%M:%S");
}
std::string DateTime::DateStrFromMillis(int64_t _ms) {
	return _StringFromMillis(_ms, "%a, %d %b %Y");
}

std::string DateTime::DateStrFromMillis_1(int64_t _ms) {
	return _StringFromMillis(_ms, "%Y/%m/%d");
}

std::string DateTime::DateHMStrFromMillis(int64_t _ms) {
    return _StringFromMillis(_ms, "%a, %d %b %Y %H:%M");
}
/*std::string DateTime::DateStrDbFromMillis(int64_t _ms)
{
    return _StringFromMillis(_ms, "%Y-%m-%d");
}*/
std::string DateTime::TimeHMStringFromMillis(int64_t _ms) {
    return _StringFromMillis(_ms, "%H:%M");
}
//to_string("yyyy-MM-dd HH:mm:ss")
std::string DateTime::to_string() {
	struct tm  *ti = getTM();
	char       buf[48] = { 0 };
	sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", ti->tm_year + 1900, ti->tm_mon + 1, ti->tm_mday, ti->tm_hour, ti->tm_min, ti->tm_sec);

	return buf;
}
//to_string("yyyy-MM-dd HH:mm:ss")
std::string DateTime::to_file_string() {
    struct tm  *ti = getTM();
    char       buf[48] = { 0 };
    sprintf(buf, "%04d%02d%02d%02d%02d%02d", ti->tm_year + 1900, ti->tm_mon + 1, ti->tm_mday, ti->tm_hour, ti->tm_min, ti->tm_sec);
    
    return buf;
}
TimeSpan DateTime::operator-(DateTime dt)
{
	time_t t1 = this->millis / 1000;
	time_t t2 = dt.millis / 1000;
	double rr = difftime(t1, t2);
	TimeSpan ts= TimeSpan((int)rr);
	return ts;
}
TimeSpan DateTime::Subtract(DateTime dt)
{
	time_t t1 = this->millis / 1000;
	time_t t2 = dt.millis / 1000;
	double rr = difftime(t1, t2);
	TimeSpan ts = TimeSpan((int)rr);
	return ts;
}
DateTime DateTime::AddDays(int d)
{
	uint64_t t = this->millis;
	t += (d*24 * 60 * 60 * 1000);
	DateTime dt = DateTime(t);
	return dt;
}
DateTime DateTime::AddHours(int h)
{
	uint64_t t = this->millis;
	t += (h * 60 * 60 * 1000);
	DateTime dt= DateTime(t);
    return dt;
}
DateTime DateTime::AddMinutes(int m)
{
	uint64_t t = this->millis;
	t += (m * 60 * 1000);
	DateTime dt = DateTime(t);
	return dt;
}
DateTime DateTime::AddSeconds(int sec)
{
	uint64_t t = this->millis;
	t += (sec * 1000);
	DateTime dt = DateTime(t);
	return dt;
}
int DateTime::CompareTo(DateTime dt)
{
	if(this->millis==dt.millis) return 0;
	if (this->millis > dt.millis) return 1;
	else
		return -1;

}
bool DateTime::operator==(DateTime dt)
{
	if (this->millis == dt.millis) return true;
	else return false;
}
bool DateTime::operator!=(DateTime dt)
{
	return (this->millis != dt.millis);
}
bool DateTime::operator>(DateTime dt)
{
	return (this->millis > dt.millis) ;
}
bool DateTime::operator<(DateTime dt)
{
	return (this->millis < dt.millis);
}
bool DateTime::operator<=(DateTime dt)
{
	return (this->millis <= dt.millis);
}
bool DateTime::operator>=(DateTime dt)
{
	return (this->millis >= dt.millis);
}
#if 0
DateTime::DateTime(int year, int month, int day, int hour, int min, int sec)
{
	/* get current timeinfo and modify it to the user's choice */
	time_t now = time(0);
    struct tm  *timeinfo = gmtime(&now);//localtime(&now); //gmtime(&now);
	timeinfo->tm_year = year - 1900;
	timeinfo->tm_mon = month - 1;
	timeinfo->tm_mday = day;

	timeinfo->tm_hour = hour;
	timeinfo->tm_min = min;
	timeinfo->tm_sec = sec;

	/* call mktime: timeinfo->tm_wday will be set */
	now = mktime(timeinfo);
	//double xx=now * 1000.0;
	uint64_t xx = (uint64_t)now * (uint64_t)1000;
//	this->use_utc = false;// true;
	this->millis = (uint64_t)xx;
}
DateTime::DateTime(int year, int month, int day):DateTime(year, month, day, 0, 0, 0){}
#endif
DateTime DateTime::Date()
{
	//struct tm  *ti = getTM();
	//DateTime dt(ti->tm_year+1900, ti->tm_mon+1, ti->tm_mday);
    uint64_t ts=this->getMidNightMillis(this->millis);
	return DateTime(ts);

}
bool DateTime::HasValue()
{
	return (this->millis > 0);
}
int DateTime::Hour()
{
	struct tm  *ti = getTM();
	return ti->tm_hour;
}
int DateTime::Second()
{
	struct tm  *ti = getTM();
	return ti->tm_sec;
}
int DateTime::Minute()
{
	struct tm  *ti = getTM();
	return ti->tm_min;
}
int DateTime::Month()
{
	struct tm  *ti = getTM();
	return ti->tm_mon+1;
}
int DateTime::Day()
{
	struct tm  *ti = getTM();
	return ti->tm_mday;
}
int DateTime::Year()
{
	struct tm  *ti = getTM();
	return ti->tm_year+1900;
}
DateTime DateTime::SkipMs()
{
	uint64_t t = this->millis/1000;
	DateTime dt = DateTime(t*1000);
	return dt;
}
//===============================End DateTime class===========================
