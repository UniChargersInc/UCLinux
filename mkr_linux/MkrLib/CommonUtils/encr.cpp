#include<iostream>
#include<string>
#include<stdlib.h>
#include<math.h>

std::string base64_decode(std::string const& s);


using namespace std;
#define LONG_INT int
#define MAX_KEY_BUF_SIZE	32

static const std::string ENCR_KEY = "EAAAAAUAAAALAAAAEQAAABMAAAAXAAAAHQAAAB8AAAAlAAAAKQAAACsAAAAvAAAANQAAADsAAAA9AAAAQwAAAEcAAAARAAAAEgAAABMAAAAUAAAAFQAAABYAAAAXAAAAGAAAABkAAAAaAAAAGwAAABwAAAAdAAAAHgAAAB8AAAAgAAAAHQAAADsAAAARAAAAEwAAAC8AAAAFAAAABwAAACUAAABBAAAAQwAAABcAAAA1AAAACwAAAA0AAAArAAAARwAAADEAAAAyAAAAMwAAADQAAAA1AAAANgAAADcAAAA4AAAAOQAAADoAAAA7AAAAPAAAAD0AAAAAAAAPwAAAEAAAAA=";


static string decr(string& en,LONG_INT key)
{
	LONG_INT pt, ct,  k;
	int n = 91;
	string de_m = "";
	std::string en_d= base64_decode(en);
	const LONG_INT *temp = (const LONG_INT *)en_d.c_str();
	int len = en_d.size() / 4;
	for (int i=0;i<len;i++)
	{
		ct = temp[i];
		k = 1;
		for (int j = 0; j < key; j++)
		{
			k = k * ct;
			k = k % n;
		}
		pt = k + 96;
		de_m += pt;   //m[i] = pt;
		//i++;
	}
	return de_m;
}

std::string decr_2(std::string& str_en)
{
		
	std::string keyB_d = base64_decode(ENCR_KEY);

	LONG_INT *pKey = (LONG_INT*)keyB_d.c_str();
	int max_key_index = pKey[0];
	LONG_INT *e = (pKey + 1), *d = (pKey + 1 + MAX_KEY_BUF_SIZE);
		
	return decr(str_en, d[max_key_index / 2]);

} 



