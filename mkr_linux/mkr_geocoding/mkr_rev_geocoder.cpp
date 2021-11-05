
 #include "mkr_geocoder.h"


//#include "midgard/distanceapproximator.h"
//using namespace valhalla::midgard;              -> moved to entity.h
double getClosestPoint(PointLL &point_ll,const std::vector<PointLL>& pts)
{
	auto closest = point_ll.ClosestPoint(pts);

	return std::get<1>(closest);
}

bool isWithinPoly(PointLL &point_ll, const std::vector<PointLL>& pts)
{
	return point_ll.WithinPolygon(pts);
}

//====================================================================
bool MkrRevGeoCoder::getNameAndIsInStr(std::shared_ptr <GeoEntity> &xx)
{
	if (xx->cityid != 0)
	{
		name = getDB().readGeometryInfo(xx->nameid, xx->cityid, &city);
	}
	else {
		name = getDB().readGeometryInfo(xx->nameid);
	}
	return true;
}
bool MkrRevGeoCoder::processRevGeoBuildingBoundary(std::shared_ptr < GeoEntity> &xx, std::vector<std::string> &addresses)
{
	auto ent = std::dynamic_pointer_cast<GeoBoundary>(xx);
	
	string address="",hNumber = "", buildingName = "";
	getNameAndIsInStr(xx);
	hNumber = ent->house_number;

	if (!hNumber.empty())
	{
		address = hNumber + " " + name + ", " + city;
	}
	else {
		address = name + ", " + city;
	}

	if (ent->bnameid != 0)
	{
		buildingName = getDB().readGeometryInfo(ent->bnameid);
	}


	if (!address.empty())
	{
		// add to addresses
		addresses.emplace_back(address);
		if(!buildingName.empty())
			addresses.emplace_back(buildingName);
		return true;
	}
	return false;
}
bool MkrRevGeoCoder::processRevGeoStreetBoundary(std::shared_ptr < GeoEntity> &xx, std::string &address)
{
	auto ent = std::dynamic_pointer_cast<GeoBoundary>(xx);

	
	string hNumber = "";

	getNameAndIsInStr(xx);
	hNumber = ent->house_number;

	if (!hNumber.empty())
	{
		address = hNumber + " " + name + ", " + city;
	}
	else {
		address = name + ", " + city;
	}
	
	return true;
}

bool MkrRevGeoCoder::processRevGeoBoundary(std::shared_ptr < GeoEntity> &xx, std::string &address)
{
	// admin and geo places
	auto ent = std::dynamic_pointer_cast<GeoBoundary>(xx);
	getNameAndIsInStr(xx);
	
	{
		address = name + ", " + city;
	}
	
	return true;
}

bool MkrRevGeoCoder::processRevGeoStreetInterpolation(std::shared_ptr < GeoEntity> &xx, std::string &address)
{
	auto ent = std::dynamic_pointer_cast<GeoStreetInterpolation>(xx);
	bool found = false;
	
	string hNumber = "";

	getNameAndIsInStr(xx);

	/*for (auto &bld : ent->buildings)
	{
		if (bld.isInsideTheRadius)
		{
			if (!bld.houseNumber.empty())
			{
				found = true;
				hNumber = bld.houseNumber;
				break;
			}
		}
	}*/

	auto& bb = ent->buildings;
	std::sort(bb.begin(), bb.end(), [](GeoBuilding &first, GeoBuilding& second) {
		return ((!first.houseNumber.empty() && !second.houseNumber.empty()) && (first.distance < second.distance));
		});
	auto &bld = bb[0];
	if (bld.isInsideTheRadius)
	{
		if (!bld.houseNumber.empty())
		{
			found = true;
			hNumber = bld.houseNumber;

		}
	}


	if (found)
	{
		address = hNumber + " " + name + ", " + city;
	}
	else {
		address = name + ", " + city;
	}
	
	return true;
}

std::vector<std::string> MkrRevGeoCoder::rev_geo_address_look_up(float radius)
{
	//double latp = lat2latp(lat);

	/*Length in meters of 1° of latitude = always 111.32 km
	  Length in meters of 1° of longitude = 40075 km * cos(latitude) / 360*/

	GeoDbCls &geodb = getDB();                                                           // lat = 43.8415031 , lon=-79.5244980  -> "139 Hawker Road;Glenside Dr, Vaughan"   Why??? ( The second WO)
	float lng = this->lon, lat = this->lat;

	double rad2LonDeg = radius / 111320.0;
	double rad2latDeg = radius / (40075 * 1000.0 * cos(lat) / 360.0);
	
	std::vector<std::shared_ptr<GeoEntity>> results;
	
	for (int level =/* MAX_LEVEL*/14; level >= 0; level--)
	{
		auto tile0 = calcPointTile(lng - rad2LonDeg, lat + rad2latDeg, level);
		auto tile1 = calcPointTile(lng + rad2LonDeg, lat - rad2latDeg, level);
		std::vector<uint> quadIndices;
		for (int yt = std::get<2>(tile0); yt <= std::get<2>(tile1); yt++) {
			for (int xt = std::get<1>(tile0); xt <= std::get<1>(tile1); xt++) {
				quadIndices.push_back(calcTileQuadIndex(level, xt, yt));
			}
		}
		int size = quadIndices.size();
		std::vector<GeometryInfo> res = geodb.readGeometryInfo(quadIndices/*, const PointConverter& converter*/);
		
		if (res.size() > 0)
		{
			for (auto &gi : res)
			{
				uint64_t entid = gi.first;
				auto entity = gi.second;
				entity->entid = entid;
				EntityType type = entity->etype;
				if (type == EntityType::STREET_INTERPOLATION_TYPE)
				{
					auto ent = std::dynamic_pointer_cast<GeoStreetInterpolation>(entity);
					bool is_inside_radius = false;
					for (auto &bld : ent->buildings)
					{
						int dist = (int)measure(bld.lat, bld.lon, lat, lng);
						if (dist < ent->distance)
							ent->distance = dist;

						if (dist < radius)
						{
							is_inside_radius = true;
							bld.isInsideTheRadius = true;
							bld.distance = dist;
						}
						else {
							//flag = false;
							bld.isInsideTheRadius = false;
						}
					}
					if (is_inside_radius)
					{
						results.emplace_back(entity);
					}
				}
				else if (type == EntityType::ADMIN_BOUNDARIES || type == EntityType::GEO_PLACE_BOUNDARIES)
				{
					auto ent = std::dynamic_pointer_cast<GeoBoundary>(entity);
					bool is_inside_radius = false;
					MyLinestring ls = ent->getLS();
					
					PointLL point_ll(lon, lat);
					/*std::vector<PointLL> pts;
					for (auto &point : ls)
					{
						PointLL p(point.lon, point.lat);
						pts.push_back(p);
					}*/

					is_inside_radius = isWithinPoly(point_ll, ls);
					if (is_inside_radius)
					{
						//getNameAndIsInStr(entity);
						if(type == EntityType::GEO_PLACE_BOUNDARIES)
							ent->distance = 0;  // fake it
						else
							ent->distance = (int)(radius*2);  // fake it

						results.emplace_back(entity);
					}
				}
				else if (type == EntityType::BUILDING_BOUNDARIES)
				{
					auto ent = std::dynamic_pointer_cast<GeoBoundary>(entity);
					bool is_inside_radius = false;
					MyLinestring ls = ent->getLS();
					for (auto &point : ls)
					{
						int dist = (int)measure(point.y(), point.x(), lat, lng);
						if (dist < ent->distance)
							ent->distance = dist;

						if (dist < radius)
						{
							is_inside_radius = true;
							/*bld.isInsideTheRadius = true;
							bld.distance = dist;*/
						}
						else {

							//bld.isInsideTheRadius = false;
						}
					}
					if (is_inside_radius)
					{
						//getNameAndIsInStr(entity);
						results.emplace_back(entity);
					}
				}
				else if (type == EntityType::STREET_BOUNDARIES)
				{
					auto ent = std::dynamic_pointer_cast<GeoBoundary>(entity);
					//bool is_inside_radius = false;
					MyLinestring &ls = ent->getLS();
					//for (auto &point : ls)
					//{
					//	int dist = (int)measure(point.lat, point.lon, lat, lng);
					//	if (dist < ent->distance)
					//		ent->distance = dist;

					//	if (dist < radius)
					//	{
					//		is_inside_radius = true;
					//		/*bld.isInsideTheRadius = true;
					//		bld.distance = dist;*/
					//	}
					//	else {

					//		//bld.isInsideTheRadius = false;
					//	}
					//}

				//	boost::geometry::distance(p, poly)
					
					PointLL point_ll(lon, lat);
					/*std::vector<PointLL> pts;
					for (auto &point : ls)
					{
						PointLL p(point.lon, point.lat);
						pts.push_back(p);
					}*/
					double dist = getClosestPoint(point_ll, ls);


					if (dist < radius*0.5)
					{
						//getNameAndIsInStr(entity);
						ent->distance = (int)round(dist);
						/*if (ent->distance == 0)
						{
							ent->distance = ent->distance;
						}*/
						results.emplace_back(entity);
						
					}
				}
				else {
					// it might be a place
					auto ent = std::dynamic_pointer_cast<GeoPlace>(entity);
					bool is_inside_radius = false;
				}

			}
		}
	}
	// compose address
	std::vector<std::string> addresses;
	std::string geo_b_name = "", address_name = "";
	bool geo_boundary_found = false,address_name_found=false;
	if (results.size() > 0)
	{
		std::sort(results.begin(), results.end(), [](std::shared_ptr<GeoEntity> &first, std::shared_ptr<GeoEntity>& second) {
			return (first->distance < second->distance);
			});
		
		for (auto &xx : results)
		{
			if (xx->distance >= 0)
			{
				
				if (!address_name_found && xx->etype == EntityType::STREET_INTERPOLATION_TYPE)
				{
					bool rv=processRevGeoStreetInterpolation(xx, address_name);
					if (rv) {
						address_name_found = true;
						break;
					}
				}
				else if (!address_name_found && xx->etype == EntityType::BUILDING_BOUNDARIES)
				{
					bool rv = processRevGeoBuildingBoundary(xx, addresses);
					if (rv) {
						/*geo_boundary_found = true, address_name_found = true;
						break;*/
						return std::move(addresses);
					}
				}
				else if (!address_name_found && xx->etype == EntityType::STREET_BOUNDARIES)
				{
					bool rv = processRevGeoStreetBoundary(xx, address_name);
					if (rv) {
						address_name_found = true;
						break;
					}
				}
				else if (!geo_boundary_found && xx->etype == EntityType::GEO_PLACE_BOUNDARIES)
				{
					bool rv = processRevGeoBoundary(xx, geo_b_name);
					if (rv)
						geo_boundary_found=true;
				}
				if (address_name_found && geo_boundary_found)
					break;
			}
		}//for (auto &xx : results)
		if ( !address_name_found && !geo_boundary_found )
		{
			// we did find -> try admin boundaries
			for (auto &xx : results)
			{
				if (xx->distance >= 0)
				{
					if (xx->etype == EntityType::ADMIN_BOUNDARIES)
					{
						bool rv = processRevGeoBoundary(xx, geo_b_name);
						if (rv) {
							geo_boundary_found = true;
							break;
						}
					}
				}
			}//for (auto &xx : results)

		}

	}
	
	if (address_name_found)
	{
		addresses.push_back(address_name);
		if (geo_boundary_found)
			addresses.push_back(geo_b_name);
	}
	else if (geo_boundary_found)
	{
		addresses.push_back("*");
		addresses.push_back(geo_b_name);
	}
	return std::move(addresses);
}


#if 1
void test_rev_geocoder()
{


	/*
	Node: 1090075991
City of Vaughan => Vaughan
Edited over 1 year ago by Matthew Darwin
Version #2 · Changeset #56452542
Location: 43.8501690, -79.5006044
	*/

	//double lat = 43.8501690;  //-79.501009 yo=43.849939   -> Lauderdale Dr
	//double lon = -79.5006044;

	//double lat = 43.849939;  //Mahogany
	//double lon = -79.501009;

	/* ON-400
		Bond Head, ON L0G 1B0
		44.117644, -79.635373    -> Between 9th Line and Road ref 88
	*/
	//double lat = 44.117644;  //Google shows Bond Head
	//double lon = -79.635373;



	//44.111678, -79.633734  ON-400 Bradford  Bradford, ON L3Z 2A5
	/*double lat = 44.111678;
	double lon = -79.633734;*/

	
	//43.850194, -79.502242   313-201 Lauderdale Dr	Vaughan, ON
	/*double lat = 43.850194;
	double lon = -79.502242;*/




	
		/*1000 Yonge St Suite 303, Toronto, ON M4W 2K2
		43.675675, -79.389340*/
		double lat = 43.675675;
		double lon = -79.389340;

	std::string result;
	bool rv=getRevGeoAddressOffline(lat, lon,result);

	//std::string rv = getRevGeoAddressOffline(lat = 43.850166, lon = -79.500946);   // 180 but return 168 Lauderdale drive
	if (rv)
	{
	}

}
#endif

bool getRevGeoAddressOffline(float lat, float lon, std::string &resultStr)
{
	MkrRevGeoCoder rgeo(lat, lon, &resultStr);
	if (!rgeo.isDbInitialized())
		return false;
	std::vector<std::string> rv = rgeo.rev_geo_address_look_up( 1000);
	if (rv.size() > 0)
	{
		/*for (auto& s : rv)
		{
			std::cout << s << std::endl;
		}*/

		resultStr = rv[0];
		if (rv.size() >= 2)
		{
			resultStr += ("|" + rv[1]);  // for buildings rv[1] will contain the building name
		}
		return true;
	}
	return false;
}


