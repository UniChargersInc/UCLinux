#include "sip_helpers.h"

#include <cstring>
#include <stdio.h>
#include <string.h>

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>



bool insertToMMap(m_map &my_map,std::string key,ClientSipInfo ci)
{
	my_map[key] = ci;
	return true;
}
bool eraseFromMMap(m_map &my_map, std::string key)
{
	my_map.erase(key);
	return true;
}

//https://stackoverflow.com/questions/20303821/how-to-check-if-string-is-in-array-of-strings
bool in_array(const std::string &value, const std::vector<std::string> &arr)
{
	return std::find(arr.begin(), arr.end(), value) != arr.end();
}


static bool _split(const std::string &s, char delim, std::vector<std::string> &elems)
{
	std::stringstream ss(s);
	std::string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return true;
}


std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	_split(s, delim, elems);
	return elems;
}


bool contain_substr(std::string& s, std::string substring,int &index)
{
	int pos = 0;
	int match_pos = s.find(substring.data(), pos, substring.length());
	index = match_pos;
	return	match_pos != std::string::npos;
}


std::string extractSubStr(std::string &s,std::string delim1,std::string delim2)
{
	// r.contact.substring(r.contact.indexOf(":")+1, r.contact.indexOf(";"));
	int ind1 = -1;
	bool rv1=contain_substr(s, delim1, ind1);
	int ind2 = -1;
	bool rv2 = contain_substr(s, delim2, ind2);
	
	std::string str2 = "";
	if(rv1&&rv2)
	 str2 = s.substr(ind1 + 1, ind2-(ind1+1));
	
	return str2;
}

int indexOf(std::string &s, std::string substring)
{
	int ind1 = -1;
	bool rv1 = contain_substr(s, substring, ind1);
	if (rv1)
		return ind1;
	return -1;
}
bool containsKey(m_map &my_map,std::string key)
{
	return my_map.find(key) != my_map.end();
}
int strToInt(std::string s)
{
	int rv = -1;
	try
	{
		rv = std::stoi(s);
	}
	catch (std::invalid_argument const &e)
	{
	}
	catch (std::out_of_range const &e)
	{
	}
	return rv;
}

std::string extractIpOrPort(std::string s, int choice) //Returns IP or PORT from SIP URI
{
	if (choice == 0) //returns IP if choice == 0
	{
		return s.substr(0, indexOf(s,":"));
	}
	else //else returns PORT
		return s.substr(indexOf(s,":") + 1);
}
bool is_digits(const std::string &str)
{
	return std::all_of(str.begin(), str.end(), ::isdigit); // C++11
}


//============trim============
// trim from start
std::string &ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
		std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
std::string &rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(),
		std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both ends
std::string &trim(std::string &s) {
	return ltrim(rtrim(s));
}

void removeCharFromStr(std::string &str,char ch)
{
	str.erase(std::remove(str.begin(), str.end(), ch), str.end());
}

void insertToFront(std::vector<std::string> &vv, std::string val)
{
	auto it = vv.begin();
	it = vv.insert(it, val);

}

void removeItemFromVector(std::vector<std::string> &vec, int index)
{
	vec.erase(vec.begin() + index);
}

void removeCharsFromString(std::string &str, const char* charsToRemove)
{
	str.erase(std::remove_if(str.begin(), str.end(), IsChars(charsToRemove)), str.end());
}

bool compareAddresses(struct sockaddr_in *pA0, struct sockaddr_in *pA1)
{
	char *ip0 = inet_ntoa(pA0->sin_addr);
	int port0 = ntohs(pA0->sin_port);

	char *ip1 = inet_ntoa(pA1->sin_addr);
	int port1 = ntohs(pA1->sin_port);

	return ((strcmp(ip0, ip1) == 0) && (port0 == port1));
}