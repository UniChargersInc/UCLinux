#include "my_utils.h"
#include "my_time.h"
#include <stdarg.h>  // For va_start, etc.
#include <memory>    // For std::unique_ptr


std::string getRandomUID()
{
#if 0	
	long randUId = rand();
	char buffer[33];
#ifndef WIN32
	char *intStr = itoa(randUId, buffer, 10);
#else
	char *intStr = _itoa(randUId, buffer, 10);
#endif
	if (intStr == buffer)
		return string(intStr);//"787654543876765";
	else
		return "123456789";
#else
	static bool random_initialized = false;
	if (!random_initialized) {
		/* initialize random seed: */
		srand(time(NULL));
	}
	int rnd = rand();   //Returns a pseudo-random integral number in the range between 0 and RAND_MAX.
	if (rnd > RAND_MAX)
		rnd = RAND_MAX;
	int64_t ts = DateTime::UtcNow().getMillis();
	char buf[64];
	sprintf(buf, "%lld%x", ts, rnd);
	return std::string(buf);
#endif
}

string strToUpper(const char *s)
{
	int len = strlen(s);
	char *buf = new char[len+1];
	for (int i = 0; i <len; i++)
	{
		buf[i] = toupper(s[i]);
	}
	buf[len] = 0;
	string rv = string(buf);
	delete[] buf;
	return rv;
}


string Int2Str(int v)
{
	char buf[32] = { 0 };
	//itoa(v, buf, 10);
	sprintf(buf, "%d", v);  
	return std::string(buf);
}
string Double2Str(double v)
{

	char buf[32] = { 0 };
	sprintf(buf, "%f", v); 
	return std::string(buf);
}
int Str2Int(std::string myString)
{
	//std::string myString = "45";
	int value = atoi(myString.c_str());
	return value;
}
string Int642Str(int64_t v)
{
	char buf[64] = { 0 };
	//itoa(v, buf, 10);
	sprintf(buf, "%lld", v);
	return std::string(buf);
}
int64_t Str2Int64(std::string myString)
{
	if (myString.empty())
		return 0;
	int64_t rv = strtoll(myString.c_str(), 0, 0);
	return rv;
}

unsigned long HexStr2Long(std::string s)
{
	unsigned long x = std::stoul(s, nullptr, 16);
	return x;
}
std::string Int2HexStr(int x)
{
	char buf[32] = { 0 };
	sprintf(buf, "%x", x);
	return std::string(buf);
}
//=================================================================================================
//=================================================================================================
bool split(const string &s, char delim, vector<string> &elems) 
{
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return true;
}


vector<string> split(const string &s, char delim) {
	vector<string> elems;
	split(s, delim, elems);
	return elems;
}

#if 0
//missing string printf
//this is safe and convenient but not exactly efficient
string m_format(const char* fmt, ...){
	int size = 512;
	char* buffer = 0;
	buffer = new char[size];
	va_list vl;
	va_start(vl, fmt);
	int nsize = vsnprintf(buffer, size, fmt, vl);
	if (size <= nsize){ //fail delete buffer and try again
		delete[] buffer;
		buffer = 0;
		buffer = new char[nsize + 1]; //+1 for /0
		nsize = vsnprintf(buffer, size, fmt, vl);
	}
	string ret(buffer);
	va_end(vl);
	delete[] buffer;
	return ret;
}

//http://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
std::string string_format(const std::string fmt_str, ...) {
	int final_n, n = ((int)fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
	std::string str;
	std::unique_ptr<char[]> formatted;
	va_list ap;
	while (1) {
		formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
		strcpy(&formatted[0], fmt_str.c_str());
		va_start(ap, fmt_str);
		final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
		va_end(ap);
		if (final_n < 0 || final_n >= n)
			n += abs(final_n - n + 1);
		else
			break;
	}
	return std::string(formatted.get());
}
#endif