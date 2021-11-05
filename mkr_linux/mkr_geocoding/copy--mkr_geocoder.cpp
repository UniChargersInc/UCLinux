
#include "mkr_geocoder.h"
//#include <boost/filesystem.hpp>

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
		{

			int lati = (int)(b.lat*10000000.0);
			int loni = (int)(b.lon*10000000.0);

			gResp.add_lla(lati);
			gResp.add_lla(loni);
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
		{
			valhalla::midgard::PointLL p = ls[0];
			int lati = (int)(p.lat()*10000000.0);
			int loni = (int)(p.lng()*10000000.0);

			gResp.add_lla(lati);
			gResp.add_lla(loni);
		}

		*resultStr = gResp.SerializeAsString();
		return true;
	}
	return false;
}

bool MkrGeoCoder::findClosestBuildingAndGenerateResult(std::vector<uint64_t> &vv, uint64_t isinid, int type , std::string houseNumber)
{
	std::vector<GeometryInfo> geomInfos;
	int rv = getDB().readGeometryInfo(vv, isinid, type, geomInfos);
	if (rv > 0)
	{
		GeoBuilding b = findClosestMatch(geomInfos, houseNumber);
		return generateFoundBuildingResponse(b);
	}
	return false;
}
bool MkrGeoCoder::findClosestBuildingAndGenerateResult(std::vector<uint64_t> &vv, uint64_t isinid, std::string houseNumber)
{
	
	{
		bool rv = findClosestBuildingAndGenerateResult(vv, isinid, EntityType::BUILDING_BOUNDARIES, houseNumber);
		if (rv)
			return true;
		else {
			// did noy find -> try another type
			rv = findClosestBuildingAndGenerateResult(vv, isinid, EntityType::STREET_INTERPOLATION_TYPE, houseNumber);
			if (rv)
				return true;
		}
	}
	return false;
}
bool MkrGeoCoder::findLocationAndGenerateResult(std::vector<uint64_t> &vv, uint64_t isinid, int numberOfRows)
{
	std::vector<GeometryInfo> geomInfos;
	int rv = getDB().readGeometryInfo(vv, isinid, STREET_INTERPOLATION_TYPE, geomInfos, numberOfRows);
	if (rv > 0)
	{
		GeoBuilding b(0,0,"");
		rv = findLocationMatch4Building(geomInfos,b);
		return generateFoundBuildingResponse(b);
	}
	else {
		// did not find -> try another type   TODO optimize the code to do it in one shot !!!!!!!!
		rv = getDB().readGeometryInfo(vv, isinid, STREET_BOUNDARIES, geomInfos, numberOfRows);
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
	string path_dir = "D:\\d_mb_tilemaker\\GeoCoder\\Build\\tiles_zz\\";
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
		this->geodb.open(/*&sharedData.outputFile*/db_name);
	}
	catch (sqlite_exception &ex)
	{
		int code = ex.get_code();
		db_initialized = false;
	}
}
bool MkrGeoCoder::generateListPossibleAddresses(MkrGeoCoderResponseResult& resp, std::vector<std::string> &v)
{
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
	return true;
}
bool MkrGeoCoder::processGeocoderRequest()
{
	
	std::vector<std::string> v=split(addrToFind, ',', true);
	if (v.size() == 0)
		return false;
	std::vector<std::string> v1 = split(v[0], ' ', true);  // to see if there is a house number in the request
	string houseNumber = "";
	if (v1.size() > 1)
	{
		houseNumber = v1[0];
		if (!houseNumber.empty() && verifyHouseNumvber(houseNumber))
		{
			google::protobuf::GlobalReplaceSubstring(houseNumber, "", &v[0]);     // remove house number from request vector
			boost::algorithm::trim(v[0]);
		}
		else {
			houseNumber = "";
		}
	}

	MkrGeoCoderResponseResult resp;
	int retV= getDB().readGeometryInfo(v,resp);

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

	return false;
}

bool CartofUi_BaseMapView_Geocoder(const char *addrToFind, std::string *resultStr)
{
	MkrGeoCoder geoc(addrToFind, resultStr);
	if (!geoc.isDbInitialized())
		return false;
	bool rv1 = geoc.processGeocoderRequest();
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
	std::vector<uint64_t> vv;
	vv.emplace_back(nameid);
	std::string houseNumber = "";
	if (!houseNumber.empty())
	{
		bool rv = geoc.findClosestBuildingAndGenerateResult(vv, isinid, houseNumber);
		if (rv)
			return true;
	}
	else {
		// the house number was not given
		//return location of the street
		try {
			bool rv = geoc.findLocationAndGenerateResult(vv, isinid,10);
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
					int size2 = ri->isinl_size();
					printf("size2=%d \r\n", size2);
					if (size2 > 0)
					{
						for (int j = 0; j < size2; j++)
						{
							GeoName *gn1 = ri->mutable_isinl(j);
							std::string name1 = gn1->name();
							printf("name1=%s nameid=%lld\r\n", name1.c_str(),gn1->nameid());
						}
					}
				}
			}
		}
	}
}
void test_geocoder()
{
	/*const char* reqStr = "Yonge St, toronto";*/
	//const char* reqStr = "Yonge St";
	const char* reqStr = "Petro-Canada";
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