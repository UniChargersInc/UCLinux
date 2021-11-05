
#include "mkr_geocoder.h"
#include <cmath>

#include "entity.h"
#include "MyEncodingStream.hpp"


using namespace sqlite;
using namespace std;
using namespace mkr_geocoding;

//we are going to use 6 digits -> it degrates the accuracy but make encoding more efficient !!!
//#define CoorKr	10000000.0   // 7 digits
#define _CoorKr_	1000000.0


//https://www.raspberrypi.org/forums/viewtopic.php?t=256322
/* The built-in file locking already does this. You don't need to add your own locking layer on top. 
You only have a single writer but it is at any point in time. 
It doesn't mean only a single process can write. Read https://www.sqlite.org/faq.html#q5

//-----------------------------------------------
Re: Writing to SQLIte database from multiple processes
Fri Nov 08, 2019 9:51 pm

Thanks for pointing out that SQLite implements internal file-based locking. 
For my purposes, that is probably enough, though the FAQ rather glosses over the situations where it is not enough and record-based locking (which SQLite does not implement) is required. 
If all the multiple writing processes are doing is adding new records and/or deleting old ones, file-based locking should prevent most potential problems, though relationships between Tables may break if a key record is deleted. 
If one or more process is updating records, however, things become much more 'iffy', since there is then potential for two or more processes to try to update the same record, with the last one to commit the update probably "winning". Unlike trying to update a record that another process has just deleted (which will at least provoke an error), "duelling updates" without record-level locking will almost inevitably silently result in a record containing bad or incomplete data.

//----------------------------------------------
Re: Writing to SQLIte database from multiple processes
Fri Nov 08, 2019 10:07 pm

Transactions would solve the problems you listed. Read https://www.sqlite.org/lockingv3.html#t ... on_control
*/

unsigned int toTileId(int zoom, int x, int y);

GeoDbCls::GeoDbCls() { open_flag = false; }

GeoDbCls::~GeoDbCls() {
		 //if(open_flag)
			// db << "COMMIT;"; // commit all the changes if open
}

void GeoDbCls::open(string &filename) {
	db.init(filename);
	open_flag = true;
	db << "PRAGMA synchronous = OFF;";  //https://blog.devart.com/increasing-sqlite-performance.html
	db << "PRAGMA LOCKING_MODE = EXCLUSIVE";
#if 0   // must be existing db
	db << "CREATE TABLE IF NOT EXISTS metadata (name text, value text, UNIQUE (name));";


	db << "CREATE TABLE IF NOT EXISTS names ( name text, UNIQUE (name));";
	db << "CREATE TABLE IF NOT EXISTS entities ( nameid integer,cityid integer,tileid integer,zoom integer,type integer, geometry blob,feature blob);";
#endif
	
}
#if MKR_USE_GeoDbCls_WRITE
void GeoDbCls::transactionBegin()
{
	db << "BEGIN;"; // begin a transaction
}
void GeoDbCls::transactionCommit()
{
	db << "COMMIT;"; // commit all the changes if open
}

void GeoDbCls::writeMetadata(string key, string value) {
	m.lock();
	//db << "insert INTO metadata (name,value) VALUES (?,?);" << key << value;
	db << "replace INTO metadata (name,value) VALUES (?,?);" << key << value;
	m.unlock();
}


void GeoDbCls::insertEntity(Entity *ent)
{
	int64_t nameid = -1;
	std::string name = ent->getName();
	try {

		db << "select nameid from names where name = ? ;" << name >> nameid;

	}
	catch (sqlite_exception &ex)
	{
		int code = ex.get_code();
		if (code == 101)  //https://www.sqlite.org/c3ref/c_abort.html
		{
			nameid = -1;
		}
	}

	try {
		if (nameid == -1)
		{
			db << "INSERT INTO names (name) VALUES (?);" << name;
			db << "SELECT last_insert_rowid();" >> nameid;
		}
	}
	catch (sqlite_exception &ex)
	{
		int code = ex.get_code();
	}

	if (nameid != -1)
	{
		uint tId = ent->tileInd;
		int zoom = ent->zoom;
		int type = ent->etype;
		string geometry = ent->serializeGeometry();
		std::vector<char> V(geometry.begin(), geometry.end());
		//nameid integer,tileid integer,zoom integer,type integer, geometry blob,feature blob
		db << "INSERT INTO entities (nameid, tileid, zoom, type, geometry) VALUES (?,?,?,?,?);" << nameid << tId << zoom << type << V;
	}
}



#endif

static void processStreetInterpolation(uint64_t nameid,uint64_t cityid,uint64_t entid, MyEncodingStream& stream,std::vector<GeometryInfo>& geomInfos)
{
	std::shared_ptr<GeoEntity> ent = std::make_shared<GeoStreetInterpolation>(EntityType::STREET_INTERPOLATION_TYPE);
	ent->nameid = nameid;
	ent->cityid = cityid;

	int32_t gType = stream.readNumber<int32_t>();
	int32_t interp_type = stream.readNumber<int32_t>();   //======================
	int32_t numberOfNodes = stream.readNumber<int32_t>();
	
	bool first_read_done = false;
	int32_t lon0=0, lat0 = 0;
	for (int i = 0; i < numberOfNodes; i++)
	{
		int32_t lati, loni;
		if (!first_read_done)
		{
			lati = stream.readNumber<int32_t>();
			loni = stream.readNumber<int32_t>();
			lon0 = loni, lat0 = lati;
			first_read_done = true;
		}
		else {
			lati = stream.readNumber<int32_t>();
			loni = stream.readNumber<int32_t>();
			lati = lat0 - lati;
			loni = lon0 - loni;
		}
		double lon = loni / _CoorKr_, lat = lati / _CoorKr_;

		std::string houseNumber = stream.readString();
		GeoBuilding bld(lat, lon, houseNumber);
		if (!houseNumber.empty())
		{
			int32_t asize = stream.readNumber<int32_t>();// size of additional tags
			if (asize > 0)
			{
				// read tags info
				for (int j = 0; j < asize; j++)
				{
					std::string key = stream.readString();
					std::string val = stream.readString();
				}
			}
		}
		//ent->buildings.emplace_back(std::move(bld));  //https://stackoverflow.com/questions/27502968/how-do-i-implement-polymorphism-with-stdshared-ptr
		auto derived2 = std::dynamic_pointer_cast<GeoStreetInterpolation>(ent);
		derived2->interp_type = interp_type;
		derived2->buildings.emplace_back(std::move(bld));
	}
	std::pair<uint64_t, std::shared_ptr<GeoEntity> > p1(entid, ent);
	geomInfos.emplace_back(p1);
}
static void processBuilding_Street_Admin_Boundaries(int type,uint64_t nameid, uint64_t cityid, uint64_t entid, MyEncodingStream& stream, std::vector<GeometryInfo>& geomInfos)
{
	std::shared_ptr<GeoBoundary> ent = std::make_shared<GeoBoundary>((EntityType)type);
	
	ent->nameid = nameid;
	ent->cityid = cityid;

	if (type == EntityType::BUILDING_BOUNDARIES)
	{
		ent->house_number = stream.readString();
        ent->bnameid = stream.readNumber<uint64_t>();
	}

	int32_t gType = stream.readNumber<int32_t>();
	int32_t numberOfNodes = stream.readNumber<int32_t>();
	auto derived2 = std::dynamic_pointer_cast<GeoBoundary>(ent);
	MyLinestring &ls = derived2->getLS();

	bool first_read_done = false;
	int32_t lon0 = 0, lat0 = 0;

	for (int i = 0; i < numberOfNodes; i++)
	{
		int32_t lati, loni;
		if (!first_read_done)
		{
			lati = stream.readNumber<int32_t>();
			loni = stream.readNumber<int32_t>();
			lon0 = loni, lat0 = lati;
			first_read_done = true;
		}
		else {
			lati = stream.readNumber<int32_t>();
			loni = stream.readNumber<int32_t>();
			lati = lat0 - lati;
			loni = lon0 - loni;
		}

		double lon = loni / _CoorKr_, lat = lati / _CoorKr_;

		PointLL latlon(lon, lat);//MyLatLon latlon(lat, lon);

		ls.emplace_back(latlon);

	}
	std::pair<uint64_t, std::shared_ptr<GeoEntity> > p1(entid, ent);
	geomInfos.emplace_back(p1);
}

static void processPlace_Boundaries(int type, uint64_t nameid, uint64_t cityid, uint64_t entid, MyEncodingStream& stream, std::vector<GeometryInfo>& geomInfos)
{
	// process places
	std::shared_ptr<GeoEntity> ent = std::make_shared<GeoPlace>((EntityType)type);
	ent->nameid = nameid;
	ent->cityid = cityid;

	int32_t lati = stream.readNumber<int32_t>();
	int32_t loni = stream.readNumber<int32_t>();

	double lon = loni / _CoorKr_, lat = lati / _CoorKr_;

	auto place = std::dynamic_pointer_cast<GeoPlace>(ent);
	place->latp = lat;
	place->lon = lon;

	std::pair<uint64_t, std::shared_ptr<GeoEntity> > p1(entid, ent);
	geomInfos.emplace_back(p1);
}
std::string GeoDbCls::read_isinStr(uint64_t isinid)
{
	std::string sql, name="";
	if (isinid > 0)
	{
		sql = "SELECT name FROM names WHERE rowid=" + std::to_string(isinid);
		db << sql >> name;
	}
	return name;
}

std::vector<GeometryInfo> GeoDbCls::readGeometryInfo(const std::vector<std::uint32_t>& quadIndices/*, const PointConverter& converter*/) {
	std::string sql = "SELECT rowid,nameid,isinid,type,zoom,geometry FROM entities WHERE tileid in (";
	for (std::size_t i = 0; i < quadIndices.size(); i++) {
		sql += (i > 0 ? "," : "") + std::to_string(quadIndices[i]);
	}
	sql += ")";

	std::vector<GeometryInfo> geomInfos;
	
	db << sql >> [&](uint64_t entid,int nameid, int cityid,int type,int zoom, std::vector<char> OutV) {
		MyEncodingStream stream(OutV.data(), OutV.size());
		if (type == EntityType::STREET_INTERPOLATION_TYPE)
		{
			processStreetInterpolation(nameid, cityid, entid, stream, geomInfos);
		}
		else
		if (type == EntityType::ADMIN_BOUNDARIES || type == EntityType::STREET_BOUNDARIES || type == EntityType::BUILDING_BOUNDARIES || type == EntityType::GEO_PLACE_BOUNDARIES)
		{
			processBuilding_Street_Admin_Boundaries(type, nameid, cityid, entid, stream, geomInfos);
		}
		else {
			processPlace_Boundaries(type, nameid, cityid, entid, stream, geomInfos);
		}
	};

	return  (std::move(geomInfos));
}


std::string GeoDbCls::readGeometryInfo(int nameid,int isinid,std::string *isinStr)
{
	if (nameid == 0)
		return "";
	std::string sql, name;
	if (isinid == 0)
	{
		sql = "SELECT name FROM names WHERE rowid=" + std::to_string(nameid);
		db << sql >> name;
	}
	else {
		//select name from names where rowid IN(7, 6111)
		sql = "SELECT name FROM names WHERE rowid IN (" + std::to_string(nameid) + "," + std::to_string(isinid) + ")";
		int ind = 0,nameInd=0;
		// it selects row starting with min rowid  !!
		if (nameid > isinid)
			nameInd = 1;
		db << sql >> [&](std::string strName) {
			if (ind == nameInd)
				name = strName;
			else 
				*isinStr = strName;
			ind++;
		};
	}
	return name;
}

int GeoDbCls::readGeometryInfo(std::string& requestAddress, MkrGeoCoderResponseResult &result,int type)
{
	//std::string sql = "SELECT name,rowid FROM names WHERE name like '%" + requestAddress + "%' ;";
	std::string sql = "SELECT distinct n.name,en.nameid,en.isinid,en.type FROM names n,entities en WHERE (n.name like '%" + requestAddress + "%' collate nocase) and en.nameid=n.rowid ";
	if (type > 0)
	{
		sql = sql + "and en.type=" + std::to_string(type);
	}
	db << sql >> [&](std::string strName, uint64_t nameid, uint64_t isinid,int type) {

		MkrGeoCoderResponseObj obj(strName, nameid);
		obj.type = type;
		obj.isinid = isinid;
		result.objs.emplace_back(obj);
	};
	int size = result.objs.size();
	return size;
}
#if 0
int GeoDbCls::readGeometryInfo(const std::vector<std::string>& req,/*GeoResponse& gResp*/MkrGeoCoderResponseResult &result)
{
	std::string sql = "";
	std::string reqAddr = req[0];
	int size = readGeometryInfo(reqAddr, result);
	result.numberOfIsInResponses = 0;

	/*
	if size >1 we should query db with the following statement
	  SELECT rowid,name from names where rowid IN (SELECT DISTINCT isinid FROM entities WHERE nameid=448)
	*/

	if (size>0 && size==1 && req.size() >= 2)
	{
		std::string isinStr = req[1];
	
		//sql = "SELECT nameid FROM names WHERE name like '%" + req[1] + "%'";
		sql = "SELECT rowid FROM names WHERE name='" + isinStr + "' COLLATE NOCASE;";
		db << sql >> [&](uint64_t nameid) {
			result.isinidV.emplace_back(nameid);
			result.numberOfIsInResponses++;

		};

		result.sts = 1;
		return 1;

	}
	else {
		//request has a simple word e.g. "lauderdale" OR
		// request has  e.g. "Yonge st, toronto"  where "Yonge st" returns multiple choices
#if 1
		
		if (req.size() == 1)
		{
			for (int j = 0; j < size; j++)
			{
				MkrGeoCoderResponseObj &obj = result.objs.at(j);

				char buf[256];
				sprintf(buf, "SELECT rowid,name from names where rowid IN (SELECT DISTINCT isinid FROM entities WHERE nameid=%lld);", obj.nameid); // select boundaries (isin) 
				sql = std::string(buf);
				db << sql >> [&](uint64_t nameid, std::string name) {
					MkrGeoName gname(name, nameid);
					obj.isinV.emplace_back(gname);
				};
			}
		}
		else if (req.size() >= 2)
		{
			std::string isinStr = req[1];
			for (int j = 0; j < size; j++)
			{
				MkrGeoCoderResponseObj &obj = result.objs.at(j);

				char buf[256];
				sprintf(buf, "SELECT rowid,name from names where rowid IN (SELECT DISTINCT isinid FROM entities WHERE nameid=%lld) and name='%s' COLLATE NOCASE;", obj.nameid, isinStr.c_str()); // select boundaries (isin) 
				sql = std::string(buf);
				db << sql >> [&](uint64_t nameid, std::string name) {
					MkrGeoName gname(name, nameid);
					obj.isinV.emplace_back(gname);
				};
			}
		}
#else
#endif
		// at this point we just return the list of posible addresses   !!!
		result.sts = 0;
		return 0;
	}
	
	return 1;
}
#endif

//this might return several street segments in city boundary , Make numberOfRows=1 to limit it
int GeoDbCls::readGeometryInfo(uint64_t nameid, uint64_t isinid, int type, std::vector<GeometryInfo>& geomInfos,int numberOfRows)
{
	
	char buf[256];
	/*sprintf(buf, "select rowid,nameid,geometry from entities where type=%d and isinid=%lld and nameid IN (",  type, isinid);
	std::string sql = std::string(buf);
	for (std::size_t i = 0; i < nameidV.size(); i++) {
		sql += (i > 0 ? "," : "") + std::to_string(nameidV[i]);
	}
	sql += ")";*/

	sprintf(buf, "select rowid,nameid,geometry from entities where type=%d and isinid=%lld and nameid=%lld", type, isinid,nameid);
	std::string sql = std::string(buf);
	
	if (numberOfRows != 0)
	{
		sql += (" LIMIT " + std::to_string(numberOfRows));  //https://www.sqlitetutorial.net/sqlite-limit/
	}


	db << sql >> [&](uint64_t entid,uint64_t nameid, std::vector<char> OutV) {
		MyEncodingStream stream(OutV.data(), OutV.size());
		switch (type)
		{
		case EntityType::STREET_INTERPOLATION_TYPE:
			processStreetInterpolation(nameid, isinid, entid, stream, geomInfos);
			break;
		case EntityType::ADMIN_BOUNDARIES:
		case EntityType::STREET_BOUNDARIES:
		case EntityType::BUILDING_BOUNDARIES:
		case EntityType::GEO_PLACE_BOUNDARIES:
			processBuilding_Street_Admin_Boundaries(type, nameid, isinid, entid, stream, geomInfos);
			break;
		case GEO_PLACES_FUEL:
			processPlace_Boundaries(type, nameid, isinid, entid, stream, geomInfos);
			
			break;
		}
		
	};
	return geomInfos.size();
}

