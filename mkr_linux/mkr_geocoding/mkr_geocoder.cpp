
#include "mkr_geocoder.h"
//#include <boost/filesystem.hpp>
#include <set>
#include <algorithm>
#include <cctype>
#include <string>
#include <boost/algorithm/string.hpp>

typedef std::set<std::string> m_vector;
bool is_str_digits(const std::string &str)
{
	return std::all_of(str.begin(), str.end(), ::isdigit); // C++11
}
//https://stackoverflow.com/questions/20303821/how-to-check-if-string-is-in-array-of-strings
bool in_array(const std::string &value, const m_vector &arr)
{
	return std::find(arr.begin(), arr.end(), value) != arr.end();
}

bool m_res_find(uint64_t v, m_map_result_t& m)
{
	bool rv = false;
	m_map_result_t::iterator it(m.find(v));
	if (it != m.end())
	{
		rv = true;
	}
	return rv;
}
bool m_obj_find(uint64_t v, m_map_geo_obj_t& m)
{
	bool rv = false;
	m_map_geo_obj_t::iterator it(m.find(v));
	if (it != m.end())
	{
		rv = true;
	}
	return rv;
}


#ifdef BUILD_WIN32_GLSIMULATOR
bool M_PathFileExists(char *path, bool remove_file_name)
{
	if (remove_file_name)
	{
		std::string pathf = path;
		const size_t last_slash_idx = pathf.find_last_of("\\/");
		if (std::string::npos != last_slash_idx)
		{
			pathf = pathf.substr(0, last_slash_idx);
		}

		struct stat buf;
		return (stat(pathf.c_str(), &buf) == 0);

	}


	//ifstream my_file(path);
	//if (my_file.good())
	//{
	//	// read away
	//}

	//Note that these function to not check if the file is a normal file. 
	//They just check if something (a normal file, a UNIX domain socket, a FIFO, a device file etc.) with the given name exists
	struct stat buf;
	return (stat(path, &buf) == 0);

}
#else
bool M_PathFileExists(char *path, bool remove_file_name);
#endif

using namespace sqlite;

GeoBuilding MkrGeoCoder::findClosestMatch(std::vector<GeometryInfo>& geomInfos, std::string houseN)
{
	
	std::vector<std::shared_ptr<GeoEntity>> results;
	for (auto &gi : geomInfos)
	{
		uint64_t entid = gi.first;
		auto entity = gi.second;
		entity->entid = entid;
		EntityType type = entity->etype;
		int hN=convertStr2Int(houseN);
		if (type == EntityType::STREET_INTERPOLATION_TYPE)
		{
			auto ent = std::dynamic_pointer_cast<GeoStreetInterpolation>(entity);
			bool found = false;
			for (auto &bld : ent->buildings)
			{
				if (!bld.houseNumber.empty())
				{
					int hN1 = convertStr2Int(bld.houseNumber);
					if (!found)
					{
						int dist= std::abs(hN1 - hN);
						ent->distance = dist;
						bld.distance = dist;
						bld.isInsideTheRadius = true;
						found = true;
					}
					else {
						int dist = std::abs(hN1 - hN);
						if (ent->distance > dist)
						{
							ent->distance = dist;
							bld.isInsideTheRadius = true;
							bld.distance = dist;
						}
						else {
							bld.distance = 0x7fffffff;
						}
					}
				}
				else {
					bld.distance = 0x7fffffff;
				}
			}
			if(found)
				results.emplace_back(entity);
		}
		else
			if (type == EntityType::ADMIN_BOUNDARIES)
			{
				auto ent = std::dynamic_pointer_cast<GeoBoundary>(entity);
				bool is_inside_radius = false;
				MyLinestring ls = ent->getLS();
				for (auto &point : ls)
				{
					//int dist = (int)measure(point.lat, point.lon, lat, lng);
					//if (dist < ent->distance)
					//	ent->distance = dist;

					//if (dist < radius)
					//{
					//	is_inside_radius = true;
					//	/*bld.isInsideTheRadius = true;
					//	bld.distance = dist;*/
					//}
					//else {

					//	//bld.isInsideTheRadius = false;
					//}
				}
				if (is_inside_radius)
				{
					//results.emplace_back(entity);
				}
			}
			else
				if (type == EntityType::BUILDING_BOUNDARIES)
				{
					auto ent = std::dynamic_pointer_cast<GeoBoundary>(entity);
					int hN1 = convertStr2Int(ent->house_number);
					MyLinestring ls = ent->getLS();
					
					int dist = std::abs(hN1 - hN);
					ent->distance = dist;
					results.emplace_back(entity);
				}
				else {
					// it might be a place
					auto ent = std::dynamic_pointer_cast<GeoPlace>(entity);
					bool is_inside_radius = false;
				}

	}
	if (results.size() > 0)
	{
		std::sort(results.begin(), results.end(), [](std::shared_ptr<GeoEntity> &first, std::shared_ptr<GeoEntity>& second) {
			return (first->distance < second->distance);
		});
		//for (auto &xx : results)
		auto &xx = results[0];
		{
			if (xx->distance >= 0)
			{
				
				// find out the node in buildings which has a minimum distance
				if (xx->etype == EntityType::STREET_INTERPOLATION_TYPE)
				{
					auto ent = std::dynamic_pointer_cast<GeoStreetInterpolation>(xx);
					std::vector<GeoBuilding>& bb = ent->buildings;
					std::sort(bb.begin(), bb.end(), [](GeoBuilding &first, GeoBuilding& second) {
						return (first.distance < second.distance);
					});
					return bb[0];
				}else if (xx->etype == EntityType::BUILDING_BOUNDARIES)
				{
					auto ent = std::dynamic_pointer_cast<GeoBoundary>(xx);
					string hN = ent->house_number;
					MyLinestring &ls = ent->getLS();
					//float lat = ls.at(0).lat, lon = ls.at(0).lon;             // just take the first corner coord
					float lat = ls.at(0).y(), lon = ls.at(0).x();             // just take the first corner coord
					GeoBuilding bb(lat,lon,hN);
					bb.distance = xx->distance;
					return bb;
				}
			}
		}//for (auto &xx : results)

	}
	GeoBuilding b(0,0,"");
	return b;
}

bool MkrGeoCoder::findLocationMatch4Building(std::vector<GeometryInfo>& geomInfos, GeoBuilding &b)
{
	for (auto &gi : geomInfos)
	{
		uint64_t entid = gi.first;
		auto entity = gi.second;
		entity->entid = entid;
		EntityType type = entity->etype;
		
		if (type == EntityType::STREET_INTERPOLATION_TYPE)
		{
			auto ent = std::dynamic_pointer_cast<GeoStreetInterpolation>(entity);
			bool found = false;
			for (auto &bld : ent->buildings)
			{
				bld.distance = 0;
				b = bld;
				return true;
			}
			
		}
	}
	return false;
}
bool MkrGeoCoder::findLocationMatch4Boundary(std::vector<GeometryInfo>& geomInfos, GeoBoundary &b)
{
	for (auto &gi : geomInfos)
	{
		uint64_t entid = gi.first;
		auto entity = gi.second;
		entity->entid = entid;
		EntityType type = entity->etype;

		if (type == EntityType::STREET_BOUNDARIES)
		{
			auto ent = std::dynamic_pointer_cast<GeoBoundary>(entity);
			if (ent != nullptr)
			{
				//MyLinestring  &ls = ent->getLS();
				b = *ent;
			}

		}
	}
	return false;
}

bool MkrGeoCoder::verifyHouseNumvber(std::string hn)
{
	bool rv = true;
	for (char ch : hn)
	{
		if (!isdigit(ch))
		{
			rv = false;
			break;
		}
	}
	return rv;
}

bool MkrGeoCoder::generateFoundBuildingResponse(GeoBuilding &b)
{
	*resultStr = "";
	if (b.distance >= 0 && b.lat != 0 && b.lon != 0)
	{
		// preapare response
		GeoResponse gResp;
		gResp.set_sts(1);
		gResp.set_reqstr(addrToFind);
		GeoRespItem *it = gResp.add_respil();
		GeoName *gn = it->mutable_geoname();
		gn->set_name("");
		gn->set_nameid(-1);
		it->set_type(EntityType::BUILDING_BOUNDARIES);
		{

			int lati = (int)(b.lat*10000000.0);
			int loni = (int)(b.lon*10000000.0);

			it->add_lla(lati);
			it->add_lla(loni);
		}

		*resultStr = gResp.SerializeAsString();
		return true;
	}
	return false;
}
bool MkrGeoCoder::generateFoundBoundaryResponse(GeoBoundary &b)
{
	*resultStr = "";
	MyLinestring  &ls = b.getLS();
	if (ls.size()>=2)
	{
		// preapare response
		GeoResponse gResp;
		gResp.set_sts(1);
		gResp.set_reqstr(addrToFind);
		GeoRespItem *it = gResp.add_respil();
		GeoName *gn = it->mutable_geoname();
		gn->set_name("");
		gn->set_nameid(-1);
		it->set_type(EntityType::BUILDING_BOUNDARIES);
		{
			valhalla::midgard::PointLL p = ls[0];
			int lati = (int)(p.lat()*10000000.0);
			int loni = (int)(p.lng()*10000000.0);

			it->add_lla(lati);
			it->add_lla(loni);
		}

		*resultStr = gResp.SerializeAsString();
		return true;
	}
	return false;
}

bool MkrGeoCoder::findClosestBuildingAndGenerateResult(uint64_t nameid, uint64_t isinid, int type , std::string houseNumber)
{
	std::vector<GeometryInfo> geomInfos;
	int rv = getDB().readGeometryInfo(nameid, isinid, type, geomInfos);
	if (rv > 0)
	{
		GeoBuilding b = findClosestMatch(geomInfos, houseNumber);
		return generateFoundBuildingResponse(b);
	}
	return false;
}
bool MkrGeoCoder::findClosestBuildingAndGenerateResult(uint64_t nameid, uint64_t isinid, std::string houseNumber)
{
	
	{
		bool rv = findClosestBuildingAndGenerateResult(nameid, isinid, EntityType::BUILDING_BOUNDARIES, houseNumber);
		if (rv)
			return true;
		else {
			// did noy find -> try another type
			rv = findClosestBuildingAndGenerateResult(nameid, isinid, EntityType::STREET_INTERPOLATION_TYPE, houseNumber);
			if (rv)
				return true;
		}
	}
	return false;
}
bool MkrGeoCoder::findLocationAndGenerateResult(uint64_t nameid, uint64_t isinid, int numberOfRows)
{
	std::vector<GeometryInfo> geomInfos;
	int rv = getDB().readGeometryInfo(nameid, isinid, STREET_INTERPOLATION_TYPE, geomInfos, numberOfRows);
	if (rv > 0)
	{
		GeoBuilding b(0,0,"");
		rv = findLocationMatch4Building(geomInfos,b);
		return generateFoundBuildingResponse(b);
	}
	else {
		// did not find -> try another type   TODO optimize the code to do it in one shot !!!!!!!!
		rv = getDB().readGeometryInfo(nameid, isinid, STREET_BOUNDARIES, geomInfos, numberOfRows);
		if (rv > 0)
		{
			GeoBoundary b(STREET_BOUNDARIES);
			rv = findLocationMatch4Boundary(geomInfos, b);
			return generateFoundBoundaryResponse(b);
		}
	}
	return false;
}
void MkrGeoCoderBase::initDatabase()
{
#ifdef WIN32
	string path_dir = "C:\\My\\map_data\\tiles\\";// "D:\\d_mb_tilemaker\\GeoCoder\\Build\\tiles_zz\\";
#elif (defined ANDROID)
	std::string getUnidriverRootDir();
	std::string ud_root=getUnidriverRootDir();
	string path_dir = ud_root + "map_data/geocoder/";
#else
	string path_dir = "/home/mkr/service/map_data/geocoder/";
#endif
	std::string db_name = path_dir + "Canada_Ontario.gdb";  // ontario
#ifdef WIN32
	bool M_PathFileExists(char *path, bool remove_file_name);
	if (!M_PathFileExists((char*)db_name.c_str(),false))
#elif defined MKR_LINUX
	/*bool PathFileExists(const char *path);
	if (!PathFileExists(db_name.c_str()))*/
	bool M_PathFileExists(char *path, bool remove_file_name);
	if (!M_PathFileExists((char*)db_name.c_str(), false))
#else
	
	bool PathFileExists(const char *path);
	if (!PathFileExists((const char*)db_name.c_str()))
#endif
	{
		db_initialized = false;
		return ;
	}

	//make sure the db is open
	db_initialized = true;
	try {
		this->geodb.open( db_name);
	}
	catch (sqlite_exception &ex)
	{
		int code = ex.get_code();
		db_initialized = false;
	}
}
bool MkrGeoCoder::generateListPossibleAddresses(MkrGeoCoderResponseResult& resp, std::vector<std::string> &v)
{
#if 0
	*resultStr = "";
	GeoResponse gResp;
	gResp.set_sts(0);
	gResp.set_reqstr(addrToFind);

	for (auto &rr : resp.objs)
	{
		if ((v.size()>=2 && !v[1].empty()) && rr.isinV.size() == 0)
			continue;
		GeoRespItem *it = gResp.add_respil();
		GeoName *gn = it->mutable_geoname();
		gn->set_name(rr.name);
		gn->set_nameid(rr.nameid);
		if (rr.isinV.size() > 0)
		{
			for (auto &ii : rr.isinV)
			{
				GeoName *gn1 = it->add_isinl();
				gn1->set_name(ii.name);
				gn1->set_nameid(ii.nameid);
			}
		}
		else {
			if (resp.isinidV.size() > 0)
			{
				GeoName *gn1 = it->add_isinl();
				gn1->set_name("");
				gn1->set_nameid(resp.isinidV[0]);   // might need a correction
			}
		}
	}
	*resultStr = gResp.SerializeAsString();
#endif
	return true;
}
int MkrGeoCoder::addResponsePlaceItem(GeoResponse &gResp, std::shared_ptr<GeoEntity> &gg,MkrGeoCoderResponseObj &xx, std::string &isinStr)
{
	GeoRespItem *it = gResp.add_respil();
	GeoName *gn = it->mutable_geoname();
	gn->set_name(xx.name);
	gn->set_nameid(xx.nameid);
	it->set_type(xx.type);

	GeoName *gn1 = it->mutable_isinl();
	gn1->set_name(isinStr);
	gn1->set_nameid(xx.isinid);

	if (xx.type == EntityType::GEO_PLACES_FUEL)
	{
		auto place = std::dynamic_pointer_cast<GeoPlace>(gg);
		int lati = (int)(place->latp*10000000.0);
		int loni = (int)(place->lon*10000000.0);

		it->add_lla(lati);
		it->add_lla(loni);
	}

	return 1;
}
int MkrGeoCoder::addResponseStreetItem(GeoResponse &gResp, MkrGeoCoderResponseObj *xx)
{
	return 1;
}

static const m_vector tab_street_type{ "dr","rd","ave","cr","drive","road","avenue","crescent" };
bool MkrGeoCoder::processGeocoderRequest(/*TokenList &tokenList*/std::vector<MkrToken> &tokens)
{

	std::vector< MkrGeoCoderResponseResult> temp_responses;
	std::vector< MkrGeoCoderResponseResult> responses;
	std::string houseNumber = "";
	for (int i = 0; i < tokens.size(); i++) {
		MkrToken tok = tokens[i];
		std::string token = tok.token;//tokenList.tokens(TokenList::Span(i, 1)).front();
		
		if (tok.type == EntityType::STREET_HOUSE_NUMBER)
		{
			if (token.empty() || is_str_digits(token))
				continue;
			std::vector<std::string> v = split(token, ' ', false);
			if (v.size() > 0)
			{
				MkrGeoCoderResponseResult resp;
				int retV_Size = 0;
				std::string street_type = "";
				for (int j = 0; j < v.size(); j++)
				{
					std::string s = v[j];
					if (s.empty() ) {
						continue; 
					}
					if (is_str_digits(s)) {
						houseNumber = s;
						continue;   // it might be a house number here
					}

					if (in_array(s, tab_street_type))
					{
						street_type = s;
						continue;
					}

					retV_Size = getDB().readGeometryInfo(s, resp);
					
				}
				if (retV_Size > 0) {
					resp.sts = EntityType::STREET_HOUSE_NUMBER;
					temp_responses.push_back(resp);
				}
			}
		}
		else if(tok.type == EntityType::CITY){
			MkrGeoCoderResponseResult resp;
			int retV_Size = getDB().readGeometryInfo(token, resp, EntityType::ADMIN_BOUNDARIES);
			if (retV_Size > 0) {
				resp.sts = EntityType::ADMIN_BOUNDARIES;
				temp_responses.push_back(resp);
			}
		}
	}
	int rsize = temp_responses.size();
	if (rsize >0)
	{
		uint64_t isinid = 0;
		if (rsize >= 2)
		{
			if (temp_responses[1].sts == EntityType::ADMIN_BOUNDARIES)
			{
				isinid = temp_responses[1].objs[0].nameid;
			}
		}
		bool match_found = false;
		if (isinid > 0)
		{
			for (auto &xx : temp_responses[0].objs)
			{
				if (xx.isinid == isinid)
				{
					
					/*std::vector<uint64_t> vv;
					vv.push_back(xx.nameid);*/
					if (houseNumber.empty())
						findLocationAndGenerateResult(/*vv*/xx.nameid, isinid, 10);
					else
						findClosestBuildingAndGenerateResult(/*vv*/xx.nameid, isinid, houseNumber);

					match_found = true;
					return true;
				}
			}
		}
		else 
		{
			m_map_result_t streets;
			GeoResponse gResp;
			gResp.set_sts(1);
			gResp.set_reqstr(addrToFind);
			int size = temp_responses[0].objs.size();
			for (int i=0;i<size;i++)
			{
				auto *xx = &temp_responses[0].objs[i];
				/*std::string*/ xx->isinStr=getDB().read_isinStr(xx->isinid);
				std::vector<GeometryInfo> geomInfos;
				int rv = getDB().readGeometryInfo(/*nameidV*/xx->nameid, xx->isinid, xx->type, geomInfos, 1);
				if (rv>0)
				{
					for (auto &zz : geomInfos)
					{
						auto &gg = zz.second;
						if (xx->type == EntityType::GEO_PLACES_FUEL || xx->type == EntityType::GEO_PLACE_BOUNDARIES)
						{
							addResponsePlaceItem(gResp, gg, *xx, xx->isinStr);
						}
						else if (xx->type == EntityType::STREET_INTERPOLATION_TYPE || xx->type == EntityType::STREET_BOUNDARIES)
						{
							bool rv = m_res_find(xx->nameid, streets);
							if (!rv) {
								m_map_geo_obj_t m1;
								m1[xx->isinid] = xx;
								streets[xx->nameid] = m1;
							}
							else {
								auto& m1 = streets[xx->nameid];
								rv = m_obj_find(xx->isinid, m1);
								if (!rv)
								{
									m1[xx->isinid] = xx;
								}
							}
						}
					}
				}
			}
			for (auto &vv : streets)
			{
				auto &vv2 = vv.second;
				for (auto& zz : vv2)
				{
					auto& zz2 = zz.second;
					//addResponsePlaceItem(gResp, gg, *xx, xx->isinStr);
				}
			}
			*resultStr = gResp.SerializeAsString();
			return true;
		}
	}

#if 0	

	if (retV != -1)
	{
		//printGeoResponse(gResp,geodb);
	}
	if (retV == 0)
	{
		  // we have a list of possible addresses
		bool rv=generateListPossibleAddresses(resp,v);
		return rv;
	}
	else
	if (retV == 1)
	{
		int size = resp.objs.size();
		int size1 = resp.isinidV.size();
		//there must be several cases
		//1. size=1  and isinidV.size()=1       e.g Lauderdale Dr, Vaughan

		
		if (size == 1 && size1 == 1)
		{
			uint64_t nameid = resp.objs[0].nameid,
				isinid = resp.isinidV[0];
			
			std::vector<uint64_t> vv;
			vv.emplace_back(nameid);
			if (!houseNumber.empty())
			{
				bool rv = findClosestBuildingAndGenerateResult(vv, isinid, houseNumber);
				if (rv)
					return true;
			}
			else {
				// the house number was not given
				//return location of the street
				bool rv=findLocationAndGenerateResult(vv, isinid);
				if (rv)
					return true;
			}
			
			
		}
		else if (size > 1 && size1 == 1)   //2. size>1  and isinidV.size()=1       e.g Yonge St, Toronto
		{
			uint64_t isinid = resp.isinidV[0];
			std::vector<uint64_t> vv;
			for (auto &nn : resp.objs)
			{
				vv.emplace_back(nn.nameid);
			}
			if (!houseNumber.empty())
			{
				bool rv = findClosestBuildingAndGenerateResult(vv, isinid, houseNumber);
				if (rv)
					return true;
			}
			else {
				// the house number was not given
				bool rv=generateListPossibleAddresses(resp,v);
				return rv;
			}
		}
	}
#endif
	return false;
}

bool CartofUi_BaseMapView_Geocoder(const char *addrToFind, std::string *resultStr)
{
	MkrGeoCoder geoc(addrToFind, resultStr);
	if (!geoc.isDbInitialized())
		return false;
	std::string data = addrToFind;
	std::string queryStringLC = boost::algorithm::to_lower_copy(data);//toUtf8String(toLower(toUniString(addrToFind)));
	std::string safeQueryString = boost::replace_all_copy(boost::replace_all_copy(queryStringLC, "%", ""), "_", "");
	boost::trim(safeQueryString);

	//TokenList tokenList = TokenList::build(safeQueryString);

	//std::regex regex{ R"([\s,]+)" }; // split on space and comma
	//std::regex regex{ R"([;]+)" }; // split on space and comma
	//std::sregex_token_iterator it{ safeQueryString.begin(), safeQueryString.end(), regex, -1 };
	//std::vector<std::string> v = std::vector<std::string>{ it, {} };

	std::vector<std::string> v = split(safeQueryString, ',', false);

	std::vector<MkrToken> tokens;
	if (v.size() > 0)
	{
		for (int i = 0; i < v.size(); i++)
		{
			MkrToken token;
			token.token = v.at(i);
			if (i == 0)
			{   // it might be a street address
				token.type = EntityType::STREET_HOUSE_NUMBER;
			}
			else if (i == 1)
			{
				token.type = EntityType::CITY;
			}
			boost::trim(token.token);
			tokens.push_back(token);
		}
	}
	else {
		// the address query string does not contain separators -> tokenize it by space
		v = split(safeQueryString, ' ', false);
		if (v.size() > 0)
		{

		}
	}

	bool rv1 = geoc.processGeocoderRequest(tokens);
	if (rv1)
	{
		if (resultStr->size() > 0)
		{

		}
		return true;
	}
	return false;
}


// this request comes from java level when selecting the item from a possible address list
bool RequestGetAddressOriginatedFromAddresesList(uint64_t nameid,uint64_t isinid, std::string *resultStr)
{
	MkrGeoCoder geoc("", resultStr);
	if (!geoc.isDbInitialized())
		return false;
	std::string name = "";
	std::string isinStr = "";

	if (isinid != 0)
	{
		name = geoc.getDB().readGeometryInfo(nameid, isinid, &isinStr);
	}
	else {
		name = geoc.getDB().readGeometryInfo(nameid);
	}


	geoc.setAddrToFind(name + ", " + isinStr);
	/*std::vector<uint64_t> vv;
	vv.emplace_back(nameid);*/
	std::string houseNumber = "";
	if (!houseNumber.empty())
	{
		bool rv = geoc.findClosestBuildingAndGenerateResult(nameid, isinid, houseNumber);
		if (rv)
			return true;
	}
	else {
		// the house number was not given
		//return location of the street
		try {
			bool rv = geoc.findLocationAndGenerateResult(nameid, isinid,10);
			if (rv)
				return true;
		}
		catch (std::exception &ex)
		{
			printf("ex: %s\r\n", ex.what());
		}
	}
	return false;
}



#if 1
void printResult(std::string &resultStr)
{
#if 1
	GeoResponse gResp;
	if (resultStr.size() > 0)
	{
		gResp.ParseFromString(resultStr);
		
		//if (gResp.sts() == 1)
		{
			int size1 = gResp.respil_size();
			printf("resultStr=size=%d sts=%d\r\n", size1, gResp.sts());
			if (size1 > 0)
			{
				for (int i = 0; i < size1; i++)
				{
					GeoRespItem *ri = gResp.mutable_respil(i);
					GeoName gn = ri->geoname();
					std::string name = gn.name();
					printf("\r\nname=%s nameid=%lld\r\n", name.c_str(), gn.nameid());
					
					{
						
							GeoName gn1 = ri->isinl();
							std::string name1 = gn1.name();
							printf("name1=%s nameid=%lld\r\n", name1.c_str(),gn1.nameid());
						
					}
				}
			}
		}
	}
#endif
}
void test_geocoder()
{
	/*const char* reqStr = "Yonge St, toronto";*/
	const char* reqStr = "Lauderdale";
	//const char* reqStr = "Petro-Canada";
	//const char* reqStr = "180 Lauderdale Drive,Vaughan, ON";  // explicit name 
	std::string resultStr;

	//uint64_t nameid = 21436;
	//uint64_t isinid = 4;

	////printf("nativeSearchAddress.. flag=%d %lld %lld", 1, nameid, isinid);

	//bool rv=RequestGetAddressOriginatedFromAddresesList(nameid, isinid, &resultStr);
	//if (rv)
	//{
	//	if (resultStr.size() > 0)
	//	{
	//		printResult(resultStr);
	//	}
	//}

	//bool rv1 = CartofUi_BaseMapView_Geocoder("180 Lauderdale Dr, Vaughan", resultStr);
	//bool rv1 = CartofUi_BaseMapView_Geocoder("1000 yonge st, toronto", resultStr); 
	bool rv1 = CartofUi_BaseMapView_Geocoder(reqStr, &resultStr);
	//bool rv1 = CartofUi_BaseMapView_Geocoder("yonge st", resultStr);
	if (rv1)
	{
		//std::string resultStr = geoc.getResultStr();
		printResult(resultStr);
	}

}
#else
//bool CartofUi_BaseMapView_Geocoder(const char *addrToFind, std::string *resultStr);
//bool RequestGetAddressOriginatedFromAddresesList(uint64_t nameid, uint64_t isinid, std::string *resultStr);
//
//void test_geocoder()
//{
//	//1 
//	std::string resultStr;
//	std::string addr_to_find = "Lauderdale Dr";
//	bool rv = CartofUi_BaseMapView_Geocoder(addr_to_find.c_str(), &resultStr);
//	GeoResponse gResp;
//	if (rv)
//	{
//		gResp.ParseFromString(resultStr);
//		if (gResp.sts() == 1)
//		{
//
//		}
//	}
//
//	//2
//	uint64_t nameid = 24912,     //"Lauderdale Dr"
//		isinid = 22655;  //'Vaughan'
//	if (nameid != 0 && isinid != 0)
//	{
//		rv = RequestGetAddressOriginatedFromAddresesList(nameid, isinid, &resultStr);
//
//	}
//	if (rv)
//	{
//		gResp.ParseFromString(resultStr);
//		if (gResp.sts() == 1)
//		{
//			int size = gResp.lla_size();
//			float lat = gResp.lla(0)/100000.0;
//			float lon = gResp.lla(1) / 100000.0;
//			if (lat != 0 && lon != 0)
//			{
//
//			}
//		}
//	}
//}

#endif