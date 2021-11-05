
#ifndef _MkrGeoDB_H
#define _MkrGeoDB_H

#include <string>
#include <mutex>
#include "sqlite_modern_cpp.h"

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
	void writeMetadata(std::string key, std::string value);
	void saveTile(int zoom, int x, int y, std::string *data);
	bool readTile(int z, int x, int y, std::vector<char> &OutV);
};

#endif //_MkrGeoDB_H

