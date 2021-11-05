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

#ifndef WIN32
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/socket.h>
#else
#include <fcntl.h>
#include <io.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

struct ClientSipInfo
{
	//std::string ipPort;
	std::string clientName;
	std::string host;  // host name or IP address
	int port;
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

bool split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);

bool contain_substr(std::string& s, std::string substring, int &index);

std::string extractSubStr(std::string &s, std::string delim1, std::string delim2);
int indexOf(std::string &s, std::string substring);
bool containsKey(m_map &my_map, std::string key);

int strToInt(std::string s);
std::string &trim(std::string &s);

void removeCharFromStr(std::string &str, char ch);
std::string extractIpOrPort(std::string s, int choice);
bool is_digits(const std::string &str);
void insertToFront(std::vector<std::string> &vv, std::string val);


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
	template <class Fn>
	void split_m_1(const char* b, const char* e, char d, Fn fn)
	{
		int i = 0;
		int beg = 0;
		bool flag = false;
		while (e ? (b + i != e) : (b[i] != '\0')) {
			if (!flag && b[i] == d) {
				fn(&b[beg], &b[i]);
				beg = i + 1;
				flag = true;
			}
			i++;
		}

		if (i) {
			fn(&b[beg], &b[i]);
		}
	}



}//namespace detail


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
bool compareAddresses(struct sockaddr_in *pA0, struct sockaddr_in *pA1);
#endif