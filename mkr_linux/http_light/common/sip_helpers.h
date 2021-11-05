#ifndef _CPPSIPHELPERS_UDPLIB_H_
#define _CPPSIPHELPERS_UDPLIB_H_

#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <regex>
#include <string>
#include <sys/stat.h>
#include <assert.h>

#include <sstream>


struct ClientSipInfo
{
	std::string ipPort;
};
struct CallDetails
{
	std::string callerNumber,
		calleeNumber;
	CallDetails()
	{
		callerNumber="",
			calleeNumber="";
	}
	CallDetails(std::string caller, std::string callee)
	{
		callerNumber=caller,
			calleeNumber=callee;
	}
};

typedef std::map<std::string, ClientSipInfo> m_map;


bool insertToMMap(m_map &my_map, std::string key, ClientSipInfo ci);
bool eraseFromMMap(m_map &my_map, std::string key);
void removeItemFromVector(std::vector<std::string> &v, int index);

bool in_array(const std::string &value, const std::vector<std::string> &arr);


bool contain_substr(std::string& s, std::string substring, int &index);

std::string extractSubStr(std::string &s, std::string delim1, std::string delim2);
int indexOf(std::string &s, std::string substring);
bool containsKey(m_map &my_map, std::string key);

int strToInt(std::string s);
std::string &trim(std::string &s);

void removeCharFromStr(std::string &str, char ch);
std::string extractIpOrPort(std::string s, int choice);
void insertToFront(std::vector<std::string> &vv, std::string val);
std::vector<std::string> split1(const std::string &s, char delim);
int replaceAll(std::string &str, std::string sub1, std::string replacementStr);

namespace sip_helpers {

	template <class Fn>
	void split_m(const char* b, const char* e, char d, Fn fn)
	{
		int i = 0;
		int beg = 0;

		while (e ? (b + i != e) : (b[i] != '\0')) {
			if (b[i] == d) {
				fn(&b[beg], &b[i]);
				beg = i + 1;
			}
			i++;
		}

		if (i) {
			fn(&b[beg], &b[i]);
		}
	}

}//namespace


// to remove a set of chars from a std::string
//https://stackoverflow.com/questions/5891610/how-to-remove-certain-characters-from-a-string-in-c
class IsChars
{
public:
	IsChars(const char* charsToRemove) : chars(charsToRemove) {};

	bool operator()(char c)
	{
		for (const char* testChar = chars; *testChar != 0; ++testChar)
		{
			if (*testChar == c) { return true; }
		}
		return false;
	}

private:
	const char* chars;
};
void removeCharsFromString(std::string &s, const char* charsToRemove);

#endif