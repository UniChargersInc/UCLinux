
#include "mkr_geo_database.h"
#include <cmath>
#ifdef ANDROID
#include "MkrHeader.h"
#include "my_utils.h"
#endif

#include <memory>    // For std::unique_ptr

#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <string>

#if defined(ANDROID)
#include <boost/filesystem.hpp>
#endif
#include <stdext/miniz.h>

#ifdef MKR_LINUX
#include "my_lib/my_lib.h"
#endif
#ifdef WIN32
#include "my_lib/my_lib.h"
#endif

#define LOG_TEST

using namespace sqlite;
using namespace std;

MkrRouteDB::MkrRouteDB() { open_flag = false; }

MkrRouteDB::~MkrRouteDB() {
		 //if(open_flag)
			// db << "COMMIT;"; // commit all the changes if open
}

void MkrRouteDB::open(string *filename) {
	db.init(*filename);
	open_flag = true;
	db << "PRAGMA synchronous = OFF;";
	db << "CREATE TABLE IF NOT EXISTS metadata (name text, value text, UNIQUE (name));";
	db << "CREATE TABLE IF NOT EXISTS tiles (graphId bigint,  tile_data blob, UNIQUE (graphId));";
	//db << "BEGIN;"; // begin a transaction
}
	
void MkrRouteDB::writeMetadata(string key, string value) {
	m.lock();
	db << "REPLACE INTO metadata (name,value) VALUES (?,?);" << key << value;
	m.unlock();
}
	
//void MkrRouteDB::saveTile(int zoom, int x, int y, string *data) {
//	int tmsY = pow(2,zoom) - 1 - y;
//	m.lock();
//	db << "REPLACE INTO tiles (zoom_level, tile_column, tile_row, tile_data) VALUES (?,?,?,?);" << zoom << x << tmsY && *data;
//	m.unlock();
//}
void MkrRouteDB::saveTile(uint64_t grId, string *data) {
	//int tmsY = pow(2, zoom) - 1 - y;
#if 0
	m.lock();
	db << "REPLACE INTO tiles (zoom_level, tile_column, tile_row, tile_data) VALUES (?,?,?,?);" << zoom << x << y << *data;
	m.unlock();
#else
	std::vector<char> V(data->begin(), data->end());
	std::lock_guard<std::mutex> lock(m);
	db << "REPLACE INTO tiles (graphId, tile_data) VALUES (?,?);" << grId << V;
	
#endif
}

bool MkrRouteDB::readTile(uint64_t grId, std::vector<char> &OutV)
{
	bool rv = false;
	char buf[256];
	sprintf(buf, "SELECT tile_data from tiles where graphId=%lld ;", grId);
	std::string strSelect(buf);
	try {
		db << strSelect >> OutV;
		rv = true;
	}
	catch (std::exception &ex) 
	{
		LOG_TEST("MkrRouteDB::readTile EXC= %s \r\n", ex.what());
	}
	return true;
}

int MkrRouteDB::copyFrom(std::string &pathToDb, std::string &filename)
{
	int rv = 0;


	try {

		db << "BEGIN;";
		db << "REPLACE INTO metadata (name,value) VALUES (?,?);" << filename << ".rdb";
		db << "COMMIT;";

		rv = 1;

		char buf[256];
		sprintf(buf, "ATTACH DATABASE \"%s\" AS aDB ;", pathToDb.c_str());
		std::string sql(buf);

		db << sql;

		db << "BEGIN;";
		//sql = "REPLACE INTO tiles SELECT * FROM aDB.tiles;";
		sql = "INSERT INTO tiles SELECT * FROM aDB.tiles;";
		db << sql;
		db << "COMMIT;";


		sql = "DETACH DATABASE 'aDB';";
		db << sql;
		rv = 2;
	}
	catch (std::exception &ex)
	{
		if (rv == 0) rv = -1;
		else if (rv == 1) rv = -2;
		else if (rv == 2) rv = -3;
#ifndef WIN32
		//MKR_LOGI_LL("Route copyFrom EXC: %s \r\n", ex.what());
#else
		printf("Route copyFrom EXC: %s \r\n", ex.what());
#endif
	}
	//MKR_LOGI_LL("Route copyFrom rv: %d \r\n", rv);
	return rv;

}

int MkrRouteDB::copyFrom(MkrRouteDB &srcDB)
{
	int rv = 0;

	sqlite::database db1 = srcDB.db;

	db << "BEGIN;";
	
	db1 << "select graphId,tile_data from tiles  ;"
		>> [&](uint64_t grId, vector<char> tiles_data) {
		
		try {
			db << "INSERT INTO tiles VALUES (?, ?)" << grId << tiles_data;

		}
		catch (std::exception &ex)
		{
#ifdef WIN32
			printf("copyFrom_1 EXC: %s \r\n", ex.what());
#endif
			rv = -1;
		}
	};
	
	
	//sql = "REPLACE INTO tiles SELECT * FROM aDB.tiles;";
	/*sql = "INSERT INTO tiles SELECT * FROM aDB.tiles;";
	db << sql;*/
	db << "COMMIT;";


	return rv;
}


int MkrRouteDB::doesRoutingTilesetExist(std::string &filename)
{
	int rv = 0;
	char buf[256];
	sprintf(buf,"SELECT value FROM metadata WHERE name like '%s%%'", filename.c_str());
	string sql = std::string(buf);
	string strOut = "";
	try {
		db << sql >> strOut;
		//MKR_LOGI_LL("doesTilesetExist strOut: %s \r\n", strOut.c_str());
		if (!strOut.empty() && strOut == ".rdb")
		{
			rv = 1;
		}
	}
	catch (std::exception &ex)
	{
		//MKR_LOGI_LL("copyFrom EXC: %s \r\n", ex.what());
		rv = -1;
	}
	return rv;
}


//*******************************************************************************
// this function is invoked from valhalla library
//*******************************************************************************
#if defined(ANDROID)
//#define udorx_root_dir_map "/sdcard/unidriver/map_data/"
std::string getUnidriverRootDir();
#elif defined(WIN32)

#endif

static bool init_db = false;
static MkrRouteDB *db=0;

static void init_sqlite_db()
{
	bool rv = true;
#if defined(ANDROID)
	std::string path = getUnidriverRootDir() + "map_data/"+ std::string("routing");
	
	if (!boost::filesystem::exists(path))
	{
		rv = boost::filesystem::create_directories(path);  // to create directory tree !!!
		//MKR_LOGI_LL("init_ROUTE sqlite_db: rv=%d\r\n", rv?1:0);
	}

	string db_name = path + "/rou_tiles.rdb";
	
	if (!boost::filesystem::exists(db_name))  // create an empty file if it does not exist
	{
		boost::filesystem::ofstream(db_name);
		//boost::filesystem::rename("/path/to/file", "/path/to/renamed_file");

		//MKR_LOGI_LL("init_ROUTEsqlite_db: create empty file\r\n");
		rv = true;
	}
	
#else
#ifdef WIN32
	string db_name = "D:\\d_mb_tilemaker\\valhalla\\win_rou_tiles.rdb";
#else
	string db_name = "/home/mkr/service/map_data/routing/rou_tiles.rdb";
#endif
	
#endif
	if (db == 0)
	{
		db = new MkrRouteDB();
	}

	db->open(&db_name);
	init_db = rv;
}

#if 0 //#ifdef WIN32
bool read_valhalla_graph_tile_from_db(uint64_t grId ,std::vector<char> &outV)
{
	if (!init_db)
	{
		init_sqlite_db();
	}

	bool rv = db->readTile(grId, outV);
	
	return rv;
}
#else

// to init routing db from outside this library
bool initialize_routing_db(std::string db_name)
{
	if (!init_db)
	{
		if (db == 0)
		{
			db = new MkrRouteDB();
		}

		db->open(&db_name);
		init_db = true;
		LOG_TEST("initialize_routing_db..OK name= %s \r\n", db_name.c_str());
	}
	
	return true;
}

bool read_valhalla_graph_tile_from_db(uint64_t grId, std::vector<char> &outV)
{
#ifdef WIN32
	printf("read graph tile__0 tid=%d\r\n", grId);
#endif
	if (!init_db)
	{
		init_sqlite_db();
	}

	std::vector<char> tileData;
	//m_printf("read graph tile tid=%d",grId);
	bool rv = db->readTile(grId, tileData);
	//m_printf("read grapg tile after size=%d", tileData.size());
	if (rv && tileData.size()>0)
	{
		//m_printf("read grapg tile after tid=%d", grId);
		rv = miniz::inflate_gzip(tileData.data(), tileData.size(), outV);
		/*if(rv){
			tileData = string(outV.begin(), outV.end());
		}*/
		LOG_TEST("read graph tile after outV Size=%d\r\n", outV.size());
	}
	else {
		rv = false;
	}
	
	return rv;
}
#endif
//********************************************************************************
//********************************************************************************
#if(defined ANDROID)
#include <boost/filesystem.hpp>


int insert_downloaded_route_graph_tiles_to_db(std::string &pathToFile)
{
	int rv = 0;
	if (!boost::filesystem::exists(pathToFile))
	{
		//MKR_LOGI_LL("downloaded_route_tiles_to_db  path=%s  does not exist\r\n", pathToFile.c_str());
		return rv;
	}
	if (!init_db)
	{
		init_sqlite_db();
	}
	boost::filesystem::path p(pathToFile);
	std::string fname = p.filename().string();
	//MKR_LOGI_LL("insert_downloaded_route_tiles_to_db  path=%s  fname=%s\r\n", pathToFile.c_str(), fname.c_str());

	rv = db->copyFrom(pathToFile, fname);
	if (rv >= 0)
	{
		//we successfully copied all data to main tiles database
		if (boost::filesystem::remove(pathToFile))
		{
			//MKR_LOGI_LL("downloaded_route_tiles_to_db  path=%s  fname=%s  REMOVED !!\r\n", pathToFile.c_str(), fname.c_str());
		}
		else {
			rv = -2;
		}
	}
	return rv;
}

int MkrGeoDB_doesRoutingTilesetExist(std::string &filename)
{
	if (!init_db)
	{
		init_sqlite_db();
	}
	return db->doesRoutingTilesetExist(filename);
}
#endif