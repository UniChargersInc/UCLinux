
#include <cstdio>
#include <string>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <sstream> 

#include <exception>
#include <mutex>

#include "udorx_server_config.h"
void m_printf(const char *format, ...);
//#if !USE_LIBEVENT
//#include "server/httplib.h"
//using namespace httplib;
//#else
#include "server/libevhtp_server.h"

//#endif
#include <google/protobuf/stubs/strutil.h>  //GlobalReplaceSubstring

void /*sk_log_to_file*/m_printf(const char *format, ...);



#define M_TILE_PRINTF m_printf

#if MKR_USE_INTERNAL_MAP_TILES_SERVICE

#define MKR_USE_OLD_SCHEMA	0

typedef struct {
	int z, x, y;
}TileS;
unsigned int toTileId(int zoom, int x, int y);
TileS fromTileId(unsigned int tid);


#include "sip_helpers.h"


unsigned int toTileId(int zoom, int x, int y)
{
	unsigned int rv = zoom << 28;
	rv |= ((x & 0x3fff) << 14);
	rv |= (y & 0x3fff);
	return rv;
}

TileS fromTileId(unsigned int tid)
{
	TileS ts;
	ts.y = tid & 0x3fff;
	ts.x = (tid >> 14) & 0x3fff;
	ts.z = ((tid >> 28) & 0xf);
	return ts;
}
#if 0
bool get_sat_tile_from_net(int z, int x, int y, std::string& result)
{
	httplib::HttpClient cli("api.mapbox.com", 80);   //a.tiles.mapbox.com -> 13.224.228.43
	/*
	https://api.mapbox.com/v4/mapbox.satellite/12/3616/1603.webp?sku=101ekwp73lzF8&access_token=pk.eyJ1IjoiZXhhbXBsZXMiLCJhIjoiY2p0MG01MXRqMW45cjQzb2R6b2ptc3J4MSJ9.zA2W0IkI0c6KaAhJfk9bWg
	*/

	//const char *token = "pk.eyJ1Ijoic2t1bHlrb3YiLCJhIjoiY2l4dnpkdWFlMDAxNjJxbzdiY3UwcDB1cSJ9.Z2e99vwl0pHnz7bvwcTr9Q";
	
	//string token_1 = "pk.eyJ1IjoiZGFucGF0IiwiYSI6ImNpcHIwOTBrMTA2am1meG5vdXE4eDN1aDkifQ.Ksnrtuh0LEJuFSVE-SlhxQ";
	std::string token_2 = "pk.eyJ1IjoibWFwYm94IiwiYSI6ImNpejY4M29iazA2Z2gycXA4N2pmbDZmangifQ.-g_vE53SD2WrJ6tFX7QHmA";
	//string token_3 = "pk.eyJ1IjoiZXhhbXBsZXMiLCJhIjoiY2p0MG01MXRqMW45cjQzb2R6b2ptc3J4MSJ9.zA2W0IkI0c6KaAhJfk9bWg";    // does not work
	//string token_my = "pk.eyJ1Ijoic2t1bHlrb3YiLCJhIjoiY2l4dnpkdWFlMDAxNjJxbzdiY3UwcDB1cSJ9.Z2e99vwl0pHnz7bvwcTr9Q";    // my token


	char buf[256];
	//int z = 13, x = 2288, y = 2988;  //13/2288/2988
	::sprintf(buf, "/v4/mapbox.satellite/%d/%d/%d.webp?access_token=%s", z, x, y, token_2.c_str());
	auto res = cli.get(buf);
	if (res) {
		/*cout << res->status << endl;
		cout << res->get_header_value("Content-Type") << endl;
		cout << res->get_header_value("Content-Encoding") << endl;
		cout << res->body << endl;*/
		if (res->status == 200)
		{
			result.assign(res->body.begin(), res->body.end());  // OR result.assign(res->body.data(), res->body.size());
			return true;
		}
	}
	return false;
}
#endif
#if 0
bool get_font_from_net(std::string fpath, std::string& result)
{
	httplib::HttpClient cli("api.mapbox.com", 80);   //a.tiles.mapbox.com -> 13.224.228.43
	/*
	https://api.mapbox.com/fonts/v1/mapbox/Open%20Sans%20Regular,Arial%20Unicode%20MS%20Regular/0-255.pbf?access_token=pk.eyJ1IjoibHVrYXNtYXJ0aW5lbGxpIiwiYSI6ImNpem85dmhwazAyajIyd284dGxhN2VxYnYifQ.HQCmyhEXZUTz3S98FMrVAQ
	*/

	//const char *token = "pk.eyJ1Ijoic2t1bHlrb3YiLCJhIjoiY2l4dnpkdWFlMDAxNjJxbzdiY3UwcDB1cSJ9.Z2e99vwl0pHnz7bvwcTr9Q";

	//string token_1 = "pk.eyJ1IjoiZGFucGF0IiwiYSI6ImNpcHIwOTBrMTA2am1meG5vdXE4eDN1aDkifQ.Ksnrtuh0LEJuFSVE-SlhxQ";
	//std::string token_2 = "pk.eyJ1IjoibWFwYm94IiwiYSI6ImNpejY4M29iazA2Z2gycXA4N2pmbDZmangifQ.-g_vE53SD2WrJ6tFX7QHmA";
	//string token_3 = "pk.eyJ1IjoiZXhhbXBsZXMiLCJhIjoiY2p0MG01MXRqMW45cjQzb2R6b2ptc3J4MSJ9.zA2W0IkI0c6KaAhJfk9bWg";    // does not work
	//string token_my = "pk.eyJ1Ijoic2t1bHlrb3YiLCJhIjoiY2l4dnpkdWFlMDAxNjJxbzdiY3UwcDB1cSJ9.Z2e99vwl0pHnz7bvwcTr9Q";    // my token


	std::string token_2 = "pk.eyJ1IjoibHVrYXNtYXJ0aW5lbGxpIiwiYSI6ImNpem85dmhwazAyajIyd284dGxhN2VxYnYifQ.HQCmyhEXZUTz3S98FMrVAQ";

	char buf[256];
	//int z = 13, x = 2288, y = 2988;  //13/2288/2988
	::sprintf(buf, "/fonts/v1/mapbox/%s?access_token=%s", fpath.c_str(), token_2.c_str());
	auto res = cli.get(buf);
	if (res) {
		/*cout << res->status << endl;
		cout << res->get_header_value("Content-Type") << endl;
		cout << res->get_header_value("Content-Encoding") << endl;
		cout << res->body << endl;*/
		if (res->status == 200)
		{
			result.assign(res->body.begin(), res->body.end());  // OR result.assign(res->body.data(), res->body.size());
			return true;
		}
		else if (res->status == 301)
		{
			cout << res->get_header_value("Location") << endl;
			cout << res->get_header_value("Content-Location") << endl;
		}
	}
	return false;
}
#endif

#define USE_SQLITE	1

#if USE_SQLITE
#include "sqlite_modern_cpp.h"
//#define sqlite_db_file_name string("america_coatlines_tiles.db")
#define sqlite_db_file_name string("tiles.db")

#define sqlite_sat_0_11 string("tiles_sat_0_11.db")
using namespace  sqlite;

#endif

#define SERVER_CERT_FILE "./cert.pem"
#define SERVER_PRIVATE_KEY_FILE "./key.pem"


using namespace std;
using namespace evhtp::evhtphelp;

static int read_bin_file(const char *full_path, std::string& str)
{
	streampos size;
	int length = 0;

	ifstream file(full_path, ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		length = size;
		file.seekg(0, ios::beg);
		str.resize(length, '\0');
		file.read(&str[0], size);
		file.close();

	}

	return length;
}

static int read_bin_file(const char *full_path, std::vector<char>& outV)
{
	streampos size;
	int length = 0;

	ifstream file(full_path, ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		length = size;
		file.seekg(0, ios::beg);
		outV.reserve(length);
		/*file.read(&str[0], size);
		file.close();*/

		outV.assign(std::istreambuf_iterator<char>(file),
			std::istreambuf_iterator<char>());

	}

	return length;
}



//static bool split(const std::string &s, char delim, std::vector<std::string> &elems)
//{
//	std::stringstream ss(s);
//	std::string item;
//	while (getline(ss, item, delim)) {
//		elems.push_back(item);
//	}
//	return true;
//}
//
//
//static std::vector<std::string> split(const std::string &s, char delim) {
//	std::vector<std::string> elems;
//	split(s, delim, elems);
//	return elems;
//}


static bool m_compress = true;

#if USE_SQLITE
static sqlite::database db, db0 , db_sat_0_11;
static std::mutex tile_mutex;
#endif

int tile_server_init(void)
{

#if USE_SQLITE
	
	string db_name = map_tiles_dir + sqlite_db_file_name;
	db.init(db_name);
	db_name = map_tiles_07_dir + string("tiles_0_7.db") ;
	db0.init(db_name);

	db_name = map_sat_tiles_dir + sqlite_sat_0_11;
	db_sat_0_11.init(db_name);
#endif
    return 0;
}

int read_tile_from_db(int z,int x,int y, std::vector<char>& OutV)
{
	int rv = 0;
		try {
			char buf[256];
#if	MKR_USE_OLD_SCHEMA
			sprintf(buf, "SELECT tile_data from tiles where zoom_level=%d and tile_column=%d and tile_row=%d ;", z, x, y);
#else
			unsigned int tid = toTileId(z, x, y);
			sprintf(buf, "SELECT tile_data from tiles where tileid=%d ;", tid);
#endif
			std::string strSelect(buf);
			std::lock_guard<std::mutex> lock(tile_mutex); // this is MUST
			if (z >= 8)
			{
				db << strSelect >> OutV;
			}
			else
			{
				db0 << strSelect >> OutV;


				/*char full_path[128];
				sprintf(full_path, "C:\\My\\mb_load_tiles\\%d\\%d\\%d.pbf", z, x, y);
				
				int rv=read_bin_file(full_path, OutV);
				if(rv>0)
					printf("got tile %d.%d.%d\r\n", z, x, y);
				else {
					printf("error get tile %d.%d.%d\r\n", z, x, y);
				}*/
			}
			rv = OutV.size();
		}
		catch (std::exception &ex)
		{
			//	m_toLog("mbtiles.readTile Exc: %s \r\n", ex.what());
			rv = -1;
		}
		
		return rv;
}


int read_sat_tile_from_db(int z, int x, int y, std::vector<char>& OutV)
{
	int rv = 0;
	if (z <= 11)
	{
		try {
			char buf[256];
#if	1
			sprintf(buf, "SELECT tile_data from tiles where zoom_level=%d and tile_column=%d and tile_row=%d ;", z, x, y);
#else
			unsigned int tid = toTileId(z, x, y);
			sprintf(buf, "SELECT tile_data from tiles where tileid=%d ;", tid);
#endif
			std::string strSelect(buf);
			std::lock_guard<std::mutex> lock(tile_mutex); // this is MUST
			//if (z <= 11)
			{
				db_sat_0_11 << strSelect >> OutV;
			}

			rv = OutV.size();
		}
		catch (std::exception &ex)
		{
			//	m_toLog("mbtiles.readTile Exc: %s \r\n", ex.what());
			rv = -1;
		}
	}
	else {
		char buf[256];
#ifdef WIN32
		sprintf(buf, "%s%d\\%d\\%d.webp", map_sat_tiles_dir, z, x, y);
#else
		sprintf(buf, "%s%d/%d/%d.webp", map_sat_tiles_dir, z, x, y);
#endif
		rv = read_bin_file(buf, OutV);
		rv = OutV.size();
	}

	return rv;
}

#if !USE_LIBEVENT
int tile_server_process_request(const Request& req, Response& res, std::string &req_ext)
{
		int retV = 0;
		std::string req_path = req.path;
		//m_printf("req:%s ", req_path.c_str());
		string content_str = "application/octet-stream";
		std::string tileData;
		int length = 0;
		string path = "";
		bool tiles_request = false;
		bool sat_tiles_request = false;
		if (req_ext=="pbf" && req_path.find("tiles") != std::string::npos) {
			//path = "D:\\d_mb_tilemaker\\Toronto" + req_path;
			//content_str = "application/octet-stream";
			if (m_compress)
				//res.set_header("Content-Encoding", "gzip");  // added to allow gzipped tile content
				res.set_header( "Content-Encoding", "gzip");
			tiles_request = true;
			retV = 1;
			//m_printf("tile req:%s ", req_path.c_str());
		}else
		if (req_ext == "webp" && req_path.find("sat") != std::string::npos) {
			//path = "D:\\d_mb_tilemaker\\mb_sat_tiles" + req_path;
			//content_str = "application/octet-stream";
			M_TILE_PRINTF("%s \r\n",path.c_str());  //---------------------------------------------------------------------------
			sat_tiles_request = true;
			retV = 1;
		}
		else if (req_ext == "pbf" && req_path.find("fonts/mikerel") != std::string::npos) {
			path = map_data_offline+string("glyph.pbf");
			//content_str = "application/octet-stream";
			retV = 1;
			
		}
		else if (req_path.find("sprite/sprite@2x.json") != std::string::npos) {
			path = map_data_offline + string("sprite@2x.json");
			content_str = "text/html; charset=utf-8";
			retV = 1;
			
		}
		else if (req_path.find("sprite/sprite@2x.png") != std::string::npos) {
			path = map_data_offline + string("sprite@2x.png");
			content_str = "image/png";
			retV = 1;
			M_TILE_PRINTF("Sprite %s\r\n", req_path.c_str());
		}
		/*else if (req_path.find("mapbox.mapbox-streets-v7") != std::string::npos) {
			path = "C:\\MY\\mapbox\\map_data\\offline\\mapbox.street-v7_downloaded.json";
			content_str = "text/html; charset=utf-8";
		}*/


#if USE_SQLITE
		if (tiles_request || sat_tiles_request)
		{

			vector<char> OutV;
			int z = 0, x = 0, y = 0 ,rv;

			vector<string> v = split(req_path, '/');
			if (v.size() >= 5)
			{
				z = std::stoi(v[2]);
				x = std::stoi(v[3]);
				y = std::stoi(v[4]);
			}
			
			if (sat_tiles_request)
			{
				M_TILE_PRINTF("Sat tile req z:%d x:%d y:%d \r\n", z, x, y);
				rv = read_sat_tile_from_db(z, x, y, OutV);
				if (rv == 0)
				{
					length = 0;
					// there is nothing in db and in the cache  !!!
					M_TILE_PRINTF("there is nothing in db and in the cache \r\n");
					bool rV= get_sat_tile_from_net(z, x, y, tileData);
					if (rV)
					{
						M_TILE_PRINTF("Got sat net resp %d  -> write to file\r\n", tileData.size());
						int write_to_file_dir_check(std::string &, std::string &);
						char fname_buf[256];
#ifdef WIN32
						sprintf(fname_buf, "%s%d\\%d\\%d.webp", map_sat_tiles_dir, z, x, y);
#else
						sprintf(fname_buf, "%s%d/%d/%d.webp", map_sat_tiles_dir, z, x, y);
#endif
						std::string fname(fname_buf);
						int rRes = write_to_file_dir_check(fname, tileData);

						length = tileData.size();
						M_TILE_PRINTF("Got sat net resp %d \r\n",length);
					}
					else {
						m_printf("Could not load sat %d/%d/%d", z, x, y);
					}
					
				}else if (rv > 0)
				{
					tileData.assign(OutV.begin(), OutV.end());
					length = tileData.size();
				}
			}
			else 
			{
				//m_printf("tile req z:%d x:%d y:%d \r\n", z, x, y);
				rv = read_tile_from_db(z, x, y, OutV);
				if (rv > 0)
				{
					tileData.assign(OutV.begin(), OutV.end());
					length = tileData.size();
				}
			}
			

		}
		else
		{
			if (!path.empty())
			{
				length = read_bin_file(path.c_str(), tileData);
			}
		}
#else
		length = read_bin_file(path.c_str(), tileData);
#endif
		if (length > 0)
		{
			//res.status = 200;
			res.set_header("Access-Control-Allow-Origin", "*");
			
			res.set_header("Content-Type", /*"application/pdf" */content_str.c_str());
			res.set_header("Cache-Control", "public, max-age=345600");
			//res.set_content((const char*)tileData.data(), length, content_str.c_str());
			set_response_buf(res,(const char*)tileData.data(), tileData.size());

			M_TILE_PRINTF("tiles -> sent %d bytes\r\n", length);

		}
		else
		{
			if (retV == 1)
			{
				M_TILE_PRINTF(" tile -> could not find \r\n");
				//set_response_buf(res, EVHTP_RES_NOTFOUND);
				res.status = 404;

			}
		}

		return retV;
}

// vim: et ts=4 sw=4 cin cino={1s ff=unix

#else
extern const char * content_enc;
extern const char* content_type;
extern const char *access_control;
extern const char *cache_control;

static std::vector<std::string> fontsDirList;
int tile_server_process_request(evhtp_request_t * request, std::string &rpath, std::string &req_ext)
{
	int retV = 0;
	std::string req_path = rpath;
	//M_TILE_PRINTF("tile req:%s ", req_path.c_str());
	string content_str = "";
	std::string tileData;
	int length = 0;
	string path = "";
	bool tiles_request = false;
	bool sat_tiles_request = false;
	if (req_ext == "pbf") {
		content_str = "application/octet-stream";
		if (req_path.find("tiles") != std::string::npos) {
			if (m_compress)
				set_response_attr(request, content_enc, "gzip", 0); // 0 tells do not allocate content_type string in a heap
			tiles_request = true;
			retV = 1;
		}
		else if (indexOf(req_path, "fonts/mikerel") != -1) {
			//path = map_data_offline + string("glyph.pbf");
			//M_TILE_PRINTF("Get font req_path:%s \r\n", req_path.c_str());

			path = extractSubStr(req_path, "fonts/mikerel/", "?");
			if (path.empty())
			{
				//M_TILE_PRINTF("Get font_00 \r\n");
				//https://api.mapbox.com/fonts/v1/mapbox/Open%20Sans%20Regular,Arial%20Unicode%20MS%20Regular/0-255.pbf?access_token=pk.eyJ1IjoibHVrYXNtYXJ0aW5lbGxpIiwiYSI6ImNpem85dmhwazAyajIyd284dGxhN2VxYnYifQ.HQCmyhEXZUTz3S98FMrVAQ
				return 0;
			}
			std::string full_path = map_data_offline_fonts + path;
			bool exists = M_PathFileExists((char*)full_path.c_str());
			//M_TILE_PRINTF("Get font path:%s \r\n", full_path.c_str());
			if (!exists)
			{
				//1. get folder name from the path
				std::string dir, filename;
				bool rv = SplitFilename2Dir(path, dir, filename);
				if (rv)
				{
					
					
					if (fontsDirList.size() == 0)
					{
#if MKR_LINUX
						fontsDirList.push_back("Open%20Sans%20Bold,Arial%20Unicode%20MS%20Bold");
						fontsDirList.push_back("Open%20Sans%20Italic,Arial%20Unicode%20MS%20Regular");
						fontsDirList.push_back("Open%20Sans%20Regular,Arial%20Unicode%20MS%20Regular");
						fontsDirList.push_back("Open%20Sans%20Semibold,Arial%20Unicode%20MS%20Bold");
#else
						std::string dir_path = map_data_offline_fonts;
						int nn = getDirectoryListing(dir_path, fontsDirList);
						if (nn == 0)
						{
							M_TILE_PRINTF("Get-font_01 \r\n");
							return 0;
						}
#endif
					}
					//2. now see if if the dir exists as a subdir in fontsDirList
					bool found = false;
					for (auto& s : fontsDirList)
					{
						int ind = indexOf(s, dir);
						if (ind != -1)
						{
							found = true;
							path = s + "/" + filename;
						}
					}
					if (!found) {
						M_TILE_PRINTF("Get font_02 \r\n");
						return 0;
					}
				}
				else {
					M_TILE_PRINTF("Get font_03 \r\n");
					return 0;
				}
			}
			
			//M_TILE_PRINTF("Get font_RR path:%s \r\n", path.c_str());
			path = map_data_offline_fonts + path;
			retV = 1;
		}
	}
	else if (req_ext == "webp" && req_path.find("sat") != std::string::npos) {
			content_str = "application/octet-stream";
			M_TILE_PRINTF("%s \r\n", path.c_str());  //---------------------------------------------------------------------------
			sat_tiles_request = true;
			retV = 1;
	}
	else if (req_path.find("sprite/sprite@2x.json") != std::string::npos || req_path.find("sprite/sprite.json") != std::string::npos)
	{
		path = map_data_offline + string("sprite@2x.json");
		content_str = "text/html; charset=utf-8";
		retV = 1;

	}
	else if (req_path.find("sprite/sprite@2x.png") != std::string::npos || req_path.find("sprite/sprite.png") != std::string::npos)
	{
		path = map_data_offline + string("sprite@2x.png");
		content_str = "image/png";
		retV = 1;
		M_TILE_PRINTF("Sprite %s\r\n", req_path.c_str());
	}
	
	if (!retV)
		return retV;
#if USE_SQLITE
	if (tiles_request || sat_tiles_request)
	{

		vector<char> OutV;
		int z = 0, x = 0, y = 0, rv;

		vector<string> v = split1(req_path, '/');
		if (v.size() >= 5)
		{
			z = std::stoi(v[2]);
			x = std::stoi(v[3]);
			y = std::stoi(v[4]);
		}

		if (sat_tiles_request)
		{
			M_TILE_PRINTF("Sat tile req z:%d x:%d y:%d \r\n", z, x, y);
			rv = read_sat_tile_from_db(z, x, y, OutV);
			if (rv == 0)
			{
				length = 0;
				// there is nothing in db and in the cache  !!!
				M_TILE_PRINTF("there is nothing in db and in the cache \r\n");
				bool rV = false;// get_sat_tile_from_net(z, x, y, tileData);
				if (rV)
				{
					M_TILE_PRINTF("Got sat net resp %d  -> write to file\r\n", tileData.size());
					int write_to_file_dir_check(std::string &, std::string &);
					char fname_buf[256];
#ifdef WIN32
					sprintf(fname_buf, "%s%d\\%d\\%d.webp", map_sat_tiles_dir, z, x, y);
#else
					sprintf(fname_buf, "%s%d/%d/%d.webp", map_sat_tiles_dir, z, x, y);
#endif
					std::string fname(fname_buf);
					int rRes = write_to_file_dir_check(fname, tileData);

					length = tileData.size();
					M_TILE_PRINTF("Got sat net resp %d \r\n", length);
				}
				else {
					m_printf("Could not load sat %d/%d/%d", z, x, y);
				}

			}
			else if (rv > 0)
			{
				tileData.assign(OutV.begin(), OutV.end());
				length = tileData.size();
			}
		}
		else
		{
			//m_printf("tile req z:%d x:%d y:%d \r\n", z, x, y);
			rv = read_tile_from_db(z, x, y, OutV);
			if (rv > 0)
			{
				tileData.assign(OutV.begin(), OutV.end());
				length = tileData.size();
			}
		}


	}
	else
	{
		if (!path.empty())
		{
			length = read_bin_file(path.c_str(), tileData);
		}
	}
#else
	length = read_bin_file(path.c_str(), tileData);
#endif
	if (length > 0)
	{
		//res.status = 200;
		set_response_attr(request, access_control, "*", 0);
		set_response_attr(request, content_type, content_str.c_str(), 0); 
		set_response_attr(request, cache_control, "public, max-age=345600", 0); //res.set_header("Cache-Control", "public, max-age=345600");
		
		set_response_buf(request, (const char*)tileData.data(), tileData.size());

		//M_TILE_PRINTF("tiles -> sent %d bytes\r\n", length);

	}
	else
	{
		if (retV == 1)
		{
			//M_TILE_PRINTF(" tile -> could not find \r\n");
			evhtp_send_reply(request, EVHTP_RES_NOTFOUND);
			//res.status = 404;

		}
	}

	return retV;
}

#endif //#if !USE_LIBEVENT

#if 0
int test_font_load()
{
	//std::string inStr="https://api.mapbox.com/fonts/v1/mapbox/Open%20Sans%20Regular,Arial%20Unicode%20MS%20Regular/0-255.pbf?access_token=pk.eyJ1IjoibHVrYXNtYXJ0aW5lbGxpIiwiYSI6ImNpem85dmhwazAyajIyd284dGxhN2VxYnYifQ.HQCmyhEXZUTz3S98FMrVAQ";
	std::string inStr = "https://www.mikerel.com/fonts/mikerel/Open%20Sans%20Regular,Arial%20Unicode%20MS%20Regular/0-255.pbf?access_token=pk.eyJ1IjoibHVrYXNtYXJ0aW5lbGxpIiwiYSI6ImNpem85dmhwazAyajIyd284dGxhN2VxYnYifQ.HQCmyhEXZUTz3S98FMrVAQ";

				//name = "65024-65279-Reg";
				//path = map_data_offline_fonts + name + string(".pbf");

	std::string fpath= extractSubStr(inStr, "fonts/mikerel/", "?");

	std::string fpath_copy = fpath;


	//removeCharsFromString(fpath, "%20");
	//int zz= replaceAll(fpath, "%20", " ");
	google::protobuf::GlobalReplaceSubstring("%20", " ", &fpath);
	std::string fontData;
	bool rv = get_font_from_net(fpath_copy, fontData);
	if (rv)
	{
		M_TILE_PRINTF("Got sat net resp %d  -> write to file\r\n", fontData.size());
		int write_to_file_dir_check(std::string &, std::string &);
		char fname_buf[256];
#ifdef WIN32
		sprintf(fname_buf, "%s\\%s", map_data_offline_fonts, fpath.c_str());
#else
		sprintf(fname_buf, "%s%d/%d/%d.webp", map_sat_tiles_dir, z, x, y);
#endif
		std::string fname(fname_buf);
		int rRes = write_to_file_dir_check(fname, fontData);

		int length = fontData.size();
		M_TILE_PRINTF("Got sat net resp %d \r\n", length);
		int retV = 1;
	}
	else {
		m_printf("Could not load font %s", fpath.c_str());
		return 0;
	}
   
	return 1;

}
#endif


#endif

