#ifndef MKR_MY_UTILS_H_INCLUDED
#define MKR_MY_UTILS_H_INCLUDED

#include <unordered_map>
#include <string.h>
#include <cstdarg>
#include <sstream>
#include <vector>

#include <memory>
#include <utility>



//#define toLowerCase(s) std::transform(s.begin(), s.end(), s.begin(), ::tolower)
#include <algorithm>
#define toUpperCase(s) std::transform(s.begin(), s.end(), s.begin(),	[](char c) { return (::toupper(c)); });
#define toLowerCase(s) std::transform(s.begin(), s.end(), s.begin(),	[](char c) { return (::tolower(c)); });

#define USE_CPP11	1


using namespace std;

string Int2Str(int v);
string Double2Str(double v);
string strToUpper(const char *s);
int Str2Int(std::string myString);
string Int642Str(int64_t v);
int64_t Str2Int64(std::string myString);

unsigned long HexStr2Long(std::string s);
std::string Int2HexStr(int x);
std::string getRandomUID();

class stdH
{
public:
#if 1
	
	template <typename KType, typename VType>
	static int GetKeys(const unordered_map <KType , VType> &m , vector<KType> &result) {
		int rv = 0;
		result.reserve(m.size()); // For efficiency
#if USE_CPP11
		for (const auto &kvp : m)
		{
			result.push_back(kvp.first);
			rv++;
		}
		
#else
		for (typename map<KType, VType>::const_iterator itr = m.begin(); itr != m.end(); ++itr)
			result.push_back(itr->first);

#endif
		return rv;
	}

	template <typename K, typename V>
	static bool ClearMapWithDynamicallyAllocatedObjects(unordered_map <K,V> &m) {

#if USE_CPP11
		for (const auto &kvp : m)
		{
			//result.push_back(kvp.first);
			auto v = kvp.second;
			if (v != 0)
			{
				delete v;
			}
		}
		m.clear();
#else
		for (typename map<KType, VType>::const_iterator itr = m.begin(); itr != m.end(); ++itr)
			result.push_back(itr->first);

#endif
		return true;
	}


	template <typename K, typename V>
	static int GetValues(const unordered_map<K, V>& m, vector<K> &result) {
		int rv = 0;
		result.reserve(m.size()); // For efficiency
#if USE_CPP11
		for (const auto &kvp : m)
		{
			result.push_back(kvp.second);
			rv++;
		}

#else
		for (typename map<K, V>::const_iterator itr = m.begin(); itr != m.end(); ++itr)
			result.push_back(itr->second);
#endif
		return rv;
	}
	template <typename K, typename V>
	static  bool containsKey(const unordered_map<K, V>& m, K key) //http://stackoverflow.com/questions/4599792/c-accessing-stdmap-keys-and-values
	{
#if USE_CPP11
		for (const auto &kvp : m)
		{
			if (kvp.first == key)
				return true;
		}
#else
		for (typename map<K, V>::const_iterator itr = m.begin(); itr != m.end(); ++itr)
			if (itr->first == key)
				return true;
#endif
		return false;

	}
	template <typename K, typename V>
	static  V *mapValByKey(const unordered_map<K, V>& m, K key)
	{
#if USE_CPP11
		for (const auto &kvp : m)
		{
			if (kvp.first == key)
				return (V*)&kvp.second;
		}
#else
		for (typename map<K, V>::const_iterator itr = m.begin(); itr != m.end(); ++itr)
			if (itr->first == key)
				return (V*)&itr->second;
#endif
		return NULL;

	}
#endif
	template <typename VType, typename FType>
	static VType vFind(vector<VType> &src, FType func)
	{
		VType v = 0;
		for (auto itr = src.begin(), end = src.end(); itr != end; ++itr)
		{
			// call the function passed into FirstOrDefault and see if it matches
			if (func(*itr))
			{
				v = (*itr);
				break;
			}
		}

		return v;
	}

	template <typename VType, typename FType>
	static VType FirstOrDefault(vector<VType> &src, FType func)
	{
		VType v = 0;
		for (auto itr = src.begin(), end = src.end(); itr != end; ++itr)
		{
			// call the function passed into FirstOrDefault and see if it matches
			if (func(*itr))
			{
				v = (*itr);
				break;
			}
		}

		return v;
	}
	template <typename K, typename V>
	static  bool TryGetValue(unordered_map<K, V> &m, K key, V &defaultV)
	{

		for (auto itr = m.begin(); itr != m.end(); ++itr)
			if (itr->first == key)
			{
				defaultV=(V)&itr->second;
				 return true;
			}
		return false;

	}

	template <typename V>
	static void addAllV(vector<V> *dest, vector<V> *src)
	{
		int len1 = src->size();
		if (len1>0)
		{
			for (int j = 0; j<len1; j++)
			{
				V el = (*src)[j];
				dest->push_back(el);
			}
		}
	}
	template <typename V>
	static void addAll(vector <V>&dest, const vector <V>&src)
	{
		int len1 = src.size();
		if (len1>0)
		{
			for (int j = 0; j<len1; j++)
				dest.push_back(src[j]);
		}
	}
	template <typename V>
	static void addAllAt(int index, vector <V>&dest, const vector <V>&src)
	{
		int len1 = src.size();
		if (len1>0)
		{
			for (int j = 0; j<len1; j++)
				insertItemAtIndex(dest, src[j], index);//dest.push_back(src[j]);
		}
	}
	template <typename V>
	static bool containsItem(vector<V> &v, V x)
	{
		bool rv = false;
		if (v.size() == 0)
			return false;
		if (std::find(v.begin(), v.end(), x) != v.end()) {
			rv = true;// v contains x 
		}
		return rv;
	}
	template <typename V>
	static bool Contains(vector<V> &v, V x)
	{
		int len = v.size();
		if (len == 0)
			return false;
		for (int i = 0; i < len; i++)
		{
			if (x == v[i])
			{
				return true;
			}
		}
		return false;
	}
	template <typename V>
	static bool removeItemAtIndex(vector<V> &v, int index)
	{
		if (v.size() > index) {
			v.erase(v.begin() + index);  //nodes.remove(i);
		}
		return true;
	}
	template <typename V>
	static bool insertItemAtIndex(vector<V> &v, V item, int index)
	{
		typename vector<V>::iterator it1;//vector<V>::iterator it1; !!! //http://stackoverflow.com/questions/11275444/c-template-typename-iterator
		it1 = v.begin();
		it1 += index;
		v.insert(it1, item);
		return true;
	}
	template <typename V>
	static bool reverseV(vector<V> &v)
	{
		reverse(v.begin(), v.end());
		return true;
	}

	template <typename V>
	static bool clearVectorDynItems(vector<V> &v)  // clear all dynamically allocated vector Items
	{
		if (v.size() == 0)
			return false;
		for (auto it : v)
		{
			if (it != 0)
				delete it;
		}
		v.clear();
		return true;
	}

	//====================string manipulations========================
	//http://stackoverflow.com/questions/1878001/how-do-i-check-if-a-c-string-starts-with-a-certain-string-and-convert-a-sub
	static bool contains(string &str, string &substr)
	{
		return str.find(substr) != string::npos;
	}
	static bool startsWith(string &str, string &substr)
	{
		bool rv = false;
		//strncmp(str, substr, strlen(substr));
		int len = substr.length();
		return strncmp(str.c_str(), substr.c_str(), len) == 0;
	}
	static bool endsWith(string &str, string &substr)  //http://stackoverflow.com/questions/874134/find-if-string-endswith-another-string-in-c
	{
		if (str.length() >= substr.length()) {
			return (0 == str.compare(str.length() - substr.length(), substr.length(), substr));
		}
		else {
			return false;
		}

	}
	static bool equalsIgnoreCase(string s, string s1)
	{
		toLowerCase(s);
		toLowerCase(s1);
		return s.compare(s1) == 0;
	}
	static bool isEmpty(string s)
	{
		return s == "";
	}

};



//=============================================================================
std::string m_format(const char* fmt, ...);
//std::string mystr = m_format("%s %d %10.5f", "omg", 1, 10.5);

vector<string> split(const string &s, char delim);
bool split(const string &s, char delim, vector<string> &elems);
std::string string_format(const std::string fmt_str, ...);

#endif