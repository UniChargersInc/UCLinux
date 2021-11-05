
#ifndef _MkrGeoDB_H
#define _MkrGeoDB_H

#include <string>
#include <mutex>
#include "sqlite_modern_cpp.h"


#define MKR_USE_MBTILES_METADATA	0
#define MKR_USE_OLD_SCHEMA			0



/** \brief Write to MkrGeoDB (sqlite) database
*
* (note that sqlite_modern_cpp.h is very slightly changed from the original, for blob support and an .init method)
*/
class MkrGeoDB
{ 
	sqlite::database db;
	std::mutex m;
	bool open_flag;
	
public:
	MkrGeoDB();
	virtual ~MkrGeoDB();
	void open(std::string *filename);
#if MKR_USE_MBTILES_METADATA
	void writeMetadata(std::string key, std::string value);
#endif
#if MKR_USE_OLD_SCHEMA
	void saveTile(int zoom, int x, int y, std::string *data);
	bool readTile(int z, int x, int y, std::vector<char> &OutV);
#else
	void saveTile(int tid, std::string *data);
	bool readTile(int tid, std::vector<char> &OutV);
#endif
};

class MkrRouteDB
{
	sqlite::database db;
	std::mutex m;
	bool open_flag;

public:
	MkrRouteDB();
	virtual ~MkrRouteDB();
	void open(std::string *filename);

	void writeMetadata(std::string key, std::string value);

	void saveTile(uint64_t grId, std::string *data);
	bool readTile(uint64_t grIdy, std::vector<char> &OutV);
	int copyFrom(std::string &pathToDb, std::string &filename);
	int copyFrom( MkrRouteDB &srcDB);
	int doesRoutingTilesetExist(std::string &filename);
	
};

#endif //_MkrGeoDB_H

