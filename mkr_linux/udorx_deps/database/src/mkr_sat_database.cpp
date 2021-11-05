
#include "mkr_sat_database.h"
#include <cmath>
using namespace sqlite;
using namespace std;

unsigned int toTileId(int zoom, int x, int y)
{
	unsigned int rv = zoom << 28;
	rv |= ((x & 0x3fff) << 14);
	rv |= (y & 0x3fff);
	return rv;
}

MkrSatDB::MkrSatDB() { open_flag = false; }

MkrSatDB::~MkrSatDB() {
		 if(open_flag)
			 db << "COMMIT;"; // commit all the changes if open
}

void MkrSatDB::open(string *filename) {
	db.init(*filename);
	open_flag = true;
	db << "PRAGMA synchronous = OFF;";
	
#if MKR_USE_MBTILES_METADATA
	db << "CREATE TABLE IF NOT EXISTS metadata (name text, value text, UNIQUE (name));";
#endif
#if MKR_USE_OLD_SCHEMA
	db << "CREATE TABLE IF NOT EXISTS tiles (zoom_level integer, tile_column integer, tile_row integer, tile_data blob, UNIQUE (zoom_level, tile_column, tile_row));";
#else
	db << "CREATE TABLE IF NOT EXISTS tiles (tileid integer, tile_data blob, UNIQUE (tileid));";
#endif
	db << "BEGIN;"; // begin a transaction
}
#if MKR_USE_MBTILES_METADATA	
void MkrSatDB::writeMetadata(string key, string value) {
	m.lock();
	db << "REPLACE INTO metadata (name,value) VALUES (?,?);" << key << value;
	m.unlock();
}
#endif
//void MkrSatDB::saveTile(int zoom, int x, int y, string *data) {
//	int tmsY = pow(2,zoom) - 1 - y;
//	m.lock();
//	db << "REPLACE INTO tiles (zoom_level, tile_column, tile_row, tile_data) VALUES (?,?,?,?);" << zoom << x << tmsY && *data;
//	m.unlock();
//}
#if MKR_USE_OLD_SCHEMA
void MkrSatDB::saveTile(int zoom, int x, int y, string *data) {
	//int tmsY = pow(2, zoom) - 1 - y;

	std::vector<char> V(data->begin(), data->end());
	m.lock();
	db << "REPLACE INTO tiles (zoom_level, tile_column, tile_row, tile_data) VALUES (?,?,?,?);" << zoom << x << y << V;
	m.unlock();

}

bool MkrSatDB::readTile(int z, int x, int y, std::vector<char> &OutV)
{
	char buf[256];
	sprintf(buf, "SELECT tile_data from tiles where zoom_level=%d and tile_column=%d and tile_row=%d ;", z, x, y);
	std::string strSelect(buf);
	db << strSelect >> OutV;
	return true;
}
#else
void MkrSatDB::saveTile(int tid, string *data) {
	//int tmsY = pow(2, zoom) - 1 - y;

	std::vector<char> V(data->begin(), data->end());
	std::lock_guard<std::mutex> lock(m);
	db << "REPLACE INTO tiles (tileid, tile_data) VALUES (?,?);" << tid << V;

}

bool MkrSatDB::readTile(int tid, std::vector<char> &OutV)
{
	char buf[256];
	sprintf(buf, "SELECT tile_data from tiles where tileid=%d ;", tid);
	std::string strSelect(buf);
	db << strSelect >> OutV;
	return true;
}
#endif