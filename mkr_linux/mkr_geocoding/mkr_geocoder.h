
#ifndef MKR_geocoder_PCH_H
#define MKR_geocoder_PCH_H

#include <string>
#include <mutex>
#include "sqlite_modern_cpp.h"
#include "entity.h"


#include <math.h>
#include <tuple>
#include <algorithm>

#include "geo_coordinates.h"

#include <sstream> 

//to get rid of boost::   trim
//https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

#include <google/protobuf/stubs/strutil.h>  //GlobalReplaceSubstring
#include "boost/algorithm/string/trim.hpp"

#include "geocoder.pb.h"
using namespace udorx_geocoder;


//=========================database==================================
#define MKR_USE_GeoDbCls_METADATA	0

#define MKR_USE_GeoDbCls_WRITE	0
#define MKR_USE_GeoDbCls_READ	1

/** \brief Write to GeoDbCls (sqlite) database
*
* (note that sqlite_modern_cpp.h is very slightly changed from the original, for blob support and an .init method)
*/

//============================================================================
#include <functional>
#include <algorithm>
#include <numeric>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/classification.hpp>

//#include "TaggedTokenList.h"
#include "StringMatcher.h"
#include "StringUtils.h"

#include <string>
#include <regex>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>

#include <boost/optional.hpp>

struct MkrToken {
	std::string token;
	EntityType type;
	EntityType db_type;

	MkrToken() {
		token = "";
		type = EntityType::PLACE_NONE;
		db_type = EntityType::PLACE_NONE;
	}
};


using namespace carto;
using namespace geocoding;
//
//
//enum class FieldType {
//	NONE, COUNTRY, REGION, COUNTY, LOCALITY, NEIGHBOURHOOD, STREET, POSTCODE, NAME, HOUSENUMBER
//};
//struct Token {
//	std::uint64_t id = 0;
//	std::uint64_t count = 0;
//	std::string token;
//	std::uint32_t typeMask = 0;
//	float idf = 1.0f;
//};
//
//using TokenList = TaggedTokenList<std::string, FieldType, std::vector<Token>>;
//========================================================================================

using GeometryInfo = std::pair<std::uint64_t, std::shared_ptr<GeoEntity>>;
class MkrGeoCoderResponseResult;
class GeoDbCls
{
	sqlite::database db;
	std::mutex m;
	bool open_flag;

public:
	GeoDbCls();
	virtual ~GeoDbCls();
	void open(std::string &filename);

	bool IsOpen() { return open_flag; }
#if MKR_USE_GeoDbCls_WRITE
	void writeMetadata(std::string key, std::string value);

	void transactionBegin();
	void transactionCommit();

	void insertEntity(Entity *ent);
#endif

	/*void saveTile(int tid, std::string *data);
	bool readTile(int tid, std::vector<char> &OutV);
	bool readTile(int z, int x, int y, std::vector<char> &OutV);*/

	std::vector<GeometryInfo> readGeometryInfo(const std::vector<std::uint32_t>& quadIndices/*, const PointConverter& converter*/);
	std::string readGeometryInfo(int nameid,int isinid=0,std::string *isinStr=NULL);

	std::string read_isinStr(uint64_t isinid);

	int readGeometryInfo(/*std::vector<uint64_t> nameidV*/uint64_t nameid, uint64_t isinid, int type, std::vector<GeometryInfo>& outV,int numberOfRows=0);

#if 0
	// for geocoding
	int readGeometryInfo(const std::vector<std::string>& requestAddress, /* GeoResponse& gResp*/MkrGeoCoderResponseResult &result);
#endif
	//====================================new stuff==============================================
	int readGeometryInfo(std::string& requestAddress, MkrGeoCoderResponseResult &result, int type=0);


};

//======================geocoder structs====================================================

struct MkrGeoName
{
	uint64_t nameid;
	string name;
	
	MkrGeoName(string s, uint64_t id)
	{
		this->name = s;
		this->nameid = id;
	}
};
struct MkrGeoCoderResponseObj : public MkrGeoName {
	
	int type=0;
	uint64_t isinid=0;
	std::string isinStr="";
	MkrGeoCoderResponseObj(string s, uint64_t id) : MkrGeoName(s, id) {}
};

struct MkrGeoCoderResponseResult
{
	int sts;
	int numberOfIsInResponses;
	std::vector<MkrGeoCoderResponseObj> objs;

};

typedef std::map<uint64_t, MkrGeoCoderResponseObj* >  m_map_geo_obj_t;
typedef std::map<uint64_t, m_map_geo_obj_t> m_map_result_t;

//==========helper===============
void trim(std::string &s);
void rtrim(std::string &s);
void ltrim(std::string &s);
double measure(double lat1, double lon1, double lat2, double lon2);

std::tuple<int, int, int> calcPointTile(double lng, double lat, int zoom);
uint calcTileQuadIndex(int zoom, uint xt, uint yt);

std::vector<std::string> split(const std::string &s, char delim, bool do_trim);

int convertStr2Int(std::string str);


void printGeoResponse(GeoResponse& gResp, GeoDbCls& geodb);

bool getRevGeoAddressOffline(float lat, float lon,std::string &result);


class MkrGeoCoderBase
{
	GeoDbCls geodb;
	bool db_initialized = false;
public:
	void initDatabase();
	bool isDbInitialized() { return db_initialized; }
	GeoDbCls &getDB() { return geodb; }
};

class MkrGeoCoder : public MkrGeoCoderBase
{
	

	std::string addrToFind;
	std::string *resultStr= NULL;

	
	bool verifyHouseNumvber(std::string hn);
	GeoBuilding findClosestMatch(std::vector<GeometryInfo>& geomInfos, std::string houseN);
	
	bool generateFoundBuildingResponse(GeoBuilding &b);
	bool generateListPossibleAddresses(MkrGeoCoderResponseResult& resp, std::vector<std::string> &v);

	bool findLocationMatch4Building(std::vector<GeometryInfo>& geomInfos, GeoBuilding &b);

	bool findLocationMatch4Boundary(std::vector<GeometryInfo>& geomInfos, GeoBoundary &b);
	bool generateFoundBoundaryResponse(GeoBoundary &b);

public:

	MkrGeoCoder(std::string reqStr, std::string *resultStr)
	{
		addrToFind = reqStr;
		this->resultStr = resultStr;
		initDatabase();
	}

	bool processGeocoderRequest(/*TokenList &tokenList*/std::vector<MkrToken> &tokens);
	//std::string & getResultStr() { return resultStr; }

	bool findClosestBuildingAndGenerateResult(/*std::vector<uint64_t> &vv*/uint64_t nameid, uint64_t isinid, int type, std::string houseNumber);
	bool findLocationAndGenerateResult(/*std::vector<uint64_t> &vv*/uint64_t nameid, uint64_t isinid, int numberOfRows=0);
	bool findClosestBuildingAndGenerateResult(/*std::vector<uint64_t> &vv*/uint64_t nameid, uint64_t isinid, std::string houseNumber);

	
	void setAddrToFind(std::string a) { addrToFind = a; }

	int addResponsePlaceItem(GeoResponse &gResp, std::shared_ptr<GeoEntity> &gg, MkrGeoCoderResponseObj &xx,std::string &isInStr);
	int addResponseStreetItem(GeoResponse &gResp,  MkrGeoCoderResponseObj *xx);

};


class MkrRevGeoCoder : public MkrGeoCoderBase
{
	std::string *resultStr = NULL;
	float lat, lon;

	std::string name = "", city = "";
	bool processRevGeoStreetInterpolation(std::shared_ptr <GeoEntity> &xx, std::string &address);
	bool processRevGeoBuildingBoundary(std::shared_ptr < GeoEntity> &xx, std::vector<std::string> &addresses);
	bool processRevGeoStreetBoundary(std::shared_ptr < GeoEntity> &xx, std::string &address);
	bool processRevGeoBoundary(std::shared_ptr < GeoEntity> &xx, std::string &address);
	bool getNameAndIsInStr(std::shared_ptr <GeoEntity> &xx);
public:
	MkrRevGeoCoder(float lat,float lon, std::string *resultStr)
	{
		this->lat = lat;
		this->lon = lon;
		this->resultStr = resultStr;
		initDatabase();
	}

	std::vector<std::string> rev_geo_address_look_up(float radius);
};

void test_rev_geocoder();

#endif //MKR_rev_geocoder_PCH_H
