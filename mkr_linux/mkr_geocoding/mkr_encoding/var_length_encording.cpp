
/**
 * Test program for varint.hpp
 *
 * Compile like this: g++ -std=c++11 -o varint-test varint-test.cpp
 *
 * License: CC0 1.0 Universal
 * Originally published on http://techoverflow.net
 * Copyright (c) 2015 Uli Koehler
 */
#include <iostream>
#include <string>
#include <vector>


#include "MyEncodingStream.hpp"

using namespace mkr_geocoding;

using namespace std;

#if 0
uint64_t zz_map_64(int64_t x)
{
	return ((uint64_t)x << 1) ^ -((uint64_t)x >> 63);
}

int64_t zz_unmap_64(uint64_t y)
{
	return (int64_t)((y >> 1) ^ -(y & 0x1));
}

uint32_t zz_map_32(int x)
{
	return ((uint32_t)x << 1) ^ -((uint32_t)x >> 31);
}

int32_t zz_unmap_32(uint32_t y)
{
	return (int32_t)((y >> 1) ^ -(y & 0x1));
}
#else



int write_int32_ToStream(int32_t val,std::vector<uint8_t> &v)
{
	uint8_t buf[12];
	uint32_t v1 = zz_map<int32_t, uint32_t>(val);
	int outsize = encodeVarint<uint32_t>(v1, buf);
	for (int i = 0; i < outsize; i++)
		v.push_back(buf[i]);
	return outsize;
}

int write_str_ToStream(std::string &str, std::vector<uint8_t> &v)
{
	write_int32_ToStream(str.size(), v);
	for (int i = 0; i < str.size(); i++)
		v.push_back(str[i]);
	return 1;
}

int write_uint64_ToStream(uint64_t val, std::vector<uint8_t> &v)
{
	uint8_t buf[24];
	uint64_t v1 = zz_map<uint64_t, uint64_t>(val);
	int outsize = encodeVarint<uint64_t>(v1, buf);
	for (int i = 0; i < outsize; i++)
		v.push_back(buf[i]);
	return outsize;
}

#endif

//https://techoverflow.net/2013/01/25/efficiently-encoding-variable-length-integers-in-cc/


/*
https://golb.hplar.ch/2019/06/variable-length-int-java.html

Downside

As mentioned above, encoding large numbers like 2,000,000,000 ends up using 5 bytes. 
So if you know in advance that most int numbers in your application will be greater than 268,435,455, stick with the fixed length encoding of 4 bytes. 
268,435,455 is the last number that can be represented with 4 bytes with this encoding.

*/

#define TEST_MKR_VAR_ENCODING 0
#if TEST_MKR_VAR_ENCODING

int lati0 = 0, loni0 = 0;
bool first_delta_insertion = false;
void insertLatLonWithDelta(double lat, double lon, std::vector<uint8_t>& vb)
{
	int lati = (int)(lat*1000000.0);
	int loni = (int)(lon*1000000.0);

	if (!first_delta_insertion)
	{
		write_int32_ToStream(lati, vb);
		write_int32_ToStream(loni, vb);
		first_delta_insertion = true;
		lati0 = lati, loni0 = loni;
	}
	else {
		write_int32_ToStream(lati0 - lati, vb);    // write delta
		write_int32_ToStream(loni0 - loni, vb);
	}
}


int main_var_length_encording() {
	//uint8_t buf[12];
#if 0
	size_t outsize = encodeVarint<uint64_t>(0xCAFE, buf);  //51966=0xcafe
	//Should print 51966
	std::cout << decodeVarint(buf, /*outsize*/12) << endl;

	int64_t val = -51966;
	uint64_t v1 = zz_map_64(val);
	outsize = encodeVarint<uint64_t>(v1, buf);
	val = 0;

	v1=decodeVarint(buf, outsize);
	val = zz_unmap_64(v1);
	std::cout << val << endl;




	int32_t val1 = -77966;
	uint32_t v11 = zz_map_32(val1);
	outsize = encodeVarint<uint32_t>(v11, buf);
	val1 = 0;

	v11 = decodeVarint(buf, outsize);
	val1 = zz_unmap_32(v11);
	std::cout << val1 << endl;



	val1 = 77977;
	v11 = zz_map_32(val1);
	outsize = encodeVarint<uint32_t>(v11, buf);
	val1 = 0;

	v11 = decodeVarint(buf, outsize);
	val1 = zz_unmap_32(v11);
	std::cout << val1 << endl;
#else
	
#if FOR_FIRST_TEST
	size_t outsize = encodeVarint<uint64_t>(0xCAFE, buf);  //51966=0xcafe
	//Should print 51966
	std::cout << decodeVarint(buf, /*outsize*/12) << endl;

	int64_t val = -51966;
	uint64_t v1 = zz_map<int64_t,uint64_t>(val);
	outsize = encodeVarint<uint64_t>(v1, buf);
	val = 0;


	EncodingStream stream(buf, outsize);
	v1 = stream.readNumber<uint64_t>();


	v1 = decodeVarint(buf, outsize);
	val = zz_unmap<int64_t, uint64_t>(v1);
	std::cout << val << endl;




	int32_t val1 = -77966;
	uint32_t v11 = zz_map<int32_t, uint32_t>(val1);
	outsize = encodeVarint<uint32_t>(v11, buf);
	val1 = 0;

	v11 = decodeVarint(buf, outsize);
	val1 = zz_unmap<int32_t, uint32_t>(v11);
	std::cout << val1 << endl;



	val1 = 77977;
	v11 = zz_map<int32_t, uint32_t>(val1);
	outsize = encodeVarint<uint32_t>(v11, buf);
	val1 = 0;

	v11 = decodeVarint(buf, outsize);
	val1 = zz_unmap<int32_t, uint32_t>(v11);
	std::cout << val1 << endl;
#endif
	std::vector<uint8_t> vb;

	write_uint64_ToStream(106386, vb);
	MyEncodingStream stream1(vb.data(), vb.size());

	uint64_t rrv = stream1.readNumber<uint64_t>();
	if (rrv != 106386)
	{

	}


	float lat = 43.45632, lon=-79.654543;

	int lati = (int)(lat*10000000.0);
	int loni = (int)(lon*10000000.0);


	const int buf[] = { loni, lati,478956238+2,478956238+4,-478956238,-478956239,478956238+5,-478956238+7 };  // 8 integers


	const struct {
		float lon, lat, cog;
	} sim_lon_lat_arr[] = {
		{-79.501930 , 43.849999 , 72.956730}, //----ind=0
		{-79.501430 , 43.849999 , 72.956730},
		{-79.501030 , 43.849991 , 72.976730}, //----ind=1
		{-79.500793 , 43.850046 , 71.976730}, //-----ind=2
		{-79.500557 , 43.850101 , 71.976730}, //-----ind=3
		{-79.499859 , 43.850252 , 73.559196}, //-----ind=4
		{-79.499161 , 43.850403 , 73.559196}, //-----ind=5
		{-79.498997 , 43.850447 , 69.428307}, //-----ind=6
		{-79.498833 , 43.850491 , 69.428307}, //-----ind=7
		{-79.498230 , 43.850626 , 72.655403}, //-----ind=8
		{-79.497627 , 43.850761 , 72.655403}, //-----ind=9
		{-79.496655 , 43.850988 , 72.060875}, //-----ind=10
		{-79.495682 , 43.851215 , 72.060875}, //-----ind=11
		{-79.495205 , 43.851318 , 73.017700}, //-----ind=12
		{-79.494728 , 43.851421 , 73.017700}, //-----ind=13
		{-79.494652 , 43.851439 , 73.572327}, //-----ind=14
		{-79.494576 , 43.851456 , 73.572327}, //-----ind=15
	};

	/*for (int i = 0; i < 16; i++)
	{
		int lti = (int)(sim_lon_lat_arr[i].lat*1000000.0);
		int lni = (int)(sim_lon_lat_arr[i].lon*1000000.0);

		write_int32_ToStream(lni, vb);
		write_int32_ToStream(lti, vb);

		float x = lni / 1000000.0;
		float y = lti / 1000000.0;
		if (x != lni)
		{
			x = lni;
		}
	}*/

	for (int i = 0; i < 16; i++)
	{
		insertLatLonWithDelta(sim_lon_lat_arr[i].lat, sim_lon_lat_arr[i].lon, vb);
	}


	/*for (int i = 0; i < 8; i++)
	{
		write_int32_ToStream(buf[i], vb);
	}*/

	string str = "";// " My House Number";
	write_str_ToStream(str, vb);
	write_int32_ToStream(-1234567, vb);

	MyEncodingStream stream(vb.data(), vb.size());


	int i = 0;
	/*while (!stream.eof())
	{
		int32_t valx = stream.readNumber<int32_t>();
		if (buf[i++] != valx)
		{
			printf("Error \r\n");
		}
		
	}*/

	for (int j = 0; j < 8; j++)
	{
		int32_t valx = stream.readNumber<int32_t>();
		if (buf[j] != valx)
		{
			printf("Error \r\n");
		}
	}

	std::string str1=stream.readString();

	int32_t valx = stream.readNumber<int32_t>();

	if (str1 != str)
	{

	}
	if (valx != 1234567)
	{

	}

#endif
	return 0;
}

#endif //TEST_MKR_VAR_ENCODING