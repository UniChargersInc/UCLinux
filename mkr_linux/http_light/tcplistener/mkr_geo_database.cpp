
#include "mkr_geo_database.h"
#include <cmath>
using namespace sqlite;
using namespace std;

MkrGeoDB::MkrGeoDB() { open_flag = false; }

MkrGeoDB::~MkrGeoDB() {
		 //if(open_flag)
			// db << "COMMIT;"; // commit all the changes if open
}

void MkrGeoDB::open(string *filename) {
	db.init(*filename);
	//open_flag = true;
	//db << "PRAGMA synchronous = OFF;";
	//db << "CREATE TABLE IF NOT EXISTS metadata (name text, value text, UNIQUE (name));";
	//db << "CREATE TABLE IF NOT EXISTS tiles (zoom_level integer, tile_column integer, tile_row integer, tile_data blob, UNIQUE (zoom_level, tile_column, tile_row));";
	//db << "BEGIN;"; // begin a transaction
}
#if MKR_USE_MBTILES_METADATA	
void MkrGeoDB::writeMetadata(string key, string value) {
	/*m.lock();
	db << "REPLACE INTO metadata (name,value) VALUES (?,?);" << key << value;
	m.unlock();*/
}
#endif	
//void MkrGeoDB::saveTile(int zoom, int x, int y, string *data) {
//	int tmsY = pow(2,zoom) - 1 - y;
//	m.lock();
//	db << "REPLACE INTO tiles (zoom_level, tile_column, tile_row, tile_data) VALUES (?,?,?,?);" << zoom << x << tmsY && *data;
//	m.unlock();
//}
void MkrGeoDB::saveTile(int zoom, int x, int y, string *data) {
	//int tmsY = pow(2, zoom) - 1 - y;
#if 0
	m.lock();
	db << "REPLACE INTO tiles (zoom_level, tile_column, tile_row, tile_data) VALUES (?,?,?,?);" << zoom << x << y << *data;
	m.unlock();
#else
	/*std::vector<char> V(data->begin(), data->end());
	m.lock();
	db << "REPLACE INTO tiles (zoom_level, tile_column, tile_row, tile_data) VALUES (?,?,?,?);" << zoom << x << y << V;
	m.unlock();*/
#endif
}

bool MkrGeoDB::readTile(int z, int x, int y, std::vector<char> &OutV)
{
	char buf[256];
	sprintf(buf, "SELECT tile_data from tiles where zoom_level=%d and tile_column=%d and tile_row=%d ;", z, x, y);
	std::string strSelect(buf);
	db << strSelect >> OutV;
	return true;
}