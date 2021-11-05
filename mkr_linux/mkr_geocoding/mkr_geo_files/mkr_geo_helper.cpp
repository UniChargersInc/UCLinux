
#include "mkr_geocoder.h"


// trim from start (in place)
void ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
		std::not1(std::ptr_fun<int, int>(std::isspace))));
}

// trim from end (in place)
void rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(),
		std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

// trim from both ends (in place)
void trim(std::string &s) {
	ltrim(s);
	rtrim(s);
}






//============================Tests=============================================
//==============================================================================
#define M_PI       3.14159265358979323846   // pi

//******************************************************************************
double measure(double lat1, double lon1, double lat2, double lon2) {  // generally used geo measurement function
	double R = 6378.137; // Radius of earth in KM
	lat1 = lat1 * M_PI / 180.0;
	lat2 = lat2 * M_PI / 180.0;
	double dLat = (lat2 - lat1);
	double dLon = (lon2 - lon1) * M_PI / 180.0;
	double a = sin(dLat / 2) * sin(dLat / 2) +	cos(lat1 ) * cos(lat2 ) *	sin(dLon / 2) * sin(dLon / 2);
	double c = 2 * atan2(sqrt(a), sqrt(1 - a));
	double d = R * c;
	return d * 1000; // meters
}

std::tuple<int, int, int> calcPointTile(double lng, double lat, int zoom) {
	uint tilex = lon2tilex(lng, zoom);
	uint tiley = lat2tiley(lat, zoom);
	return std::tuple<int, int, int> { zoom, tilex, tiley };
}
uint calcTileQuadIndex(int zoom, uint xt, uint yt) {
	return (uint)((xt << 16) + yt);
}



static bool split(const std::string &s, char delim, std::vector<std::string> &elems, bool do_trim)
{
	std::stringstream ss(s);
	std::string item;
	while (getline(ss, item, delim)) {
		if (do_trim)
			/*boost::algorithm::*/trim(item);               ////https://stackoverflow.com/questions/1798112/removing-leading-and-trailing-spaces-from-a-string
		elems.push_back(item);
	}
	return true;
}


std::vector<std::string> split(const std::string &s, char delim, bool do_trim) {
	std::vector<std::string> elems;
	split(s, delim, elems, do_trim);
	return std::move(elems);
}

void printGeoResponse(GeoResponse& gResp, GeoDbCls& geodb)
{
#if 0
	// for test
	uint64_t nameid = 0, isinid = 0;
	{
		int size = gResp.respil_size();
		printf("size=%d\r\n", size);
		for (int j = 0; j < size; j++)
		{
			printf("**********\r\n");
			GeoRespItem *item = gResp.mutable_respil(j);
			const GeoName &gn = item->geoname();
			std::string str = item->geoname().name();
			if (nameid == 0 && str == "Lauderdale Dr")
			{
				nameid = item->geoname().nameid();
			}
			printf("geoname name=%s nameid=%lld\r\n", str.c_str(), item->geoname().nameid());
			int sizeIsInL = item->isinl_size();
			printf("isinL size=%d\r\n", sizeIsInL);
			for (int i = 0; i < sizeIsInL; i++)
			{
				GeoName *gN = item->mutable_isinl(i);
				printf("gN name=%s nameid=%lld\r\n", gN->name().c_str(), gN->nameid());
				if (nameid!=0 && isinid == 0 && i == 0)
				{
					isinid = gN->nameid();
				}
			}
			printf("===========\r\n");
		}
	}
#endif
#if 0
	if (nameid != 0 && isinid != 0)
	{
		std::vector<GeometryInfo> geomInfos;
		int rv = geodb.readGeometryInfo(nameid, isinid, EntityType::STREET_INTERPOLATION_TYPE, geomInfos);
		if (rv==0)
		{
			rv = geodb.readGeometryInfo(nameid, isinid, EntityType::ADMIN_BOUNDARIES, geomInfos);
			if (rv > 0)
			{
				
			}
		}
		else {
			// preapare response
			GeoResponse gResp;
			gResp.set_sts(1);
			gResp.set_reqstr("Lauderdale Drive, Toronto");

			std::shared_ptr<GeoEntity> ent = geomInfos[0].second;
			auto streetI = std::dynamic_pointer_cast<GeoStreetInterpolation>(ent);

			int latlonSize = streetI->buildings.size();

			if (latlonSize>0)
			{
				auto &b = streetI->buildings[0];
				int lati =(int) (b.lat*10000000.0);
				int loni = (int)(b.lon*10000000.0);
				
				gResp.add_lla(lati);
				gResp.add_lla(loni);
			}
			int sizeXX = gResp.lla_size();
			if (sizeXX >= 2)
			{
				printf("lat=%d lon=%d \r\n", gResp.lla(0), gResp.lla(1));
			}
		}
	}
#endif
}


//==========================================
int convertStr2Int(std::string str)
{
	int rv = -1;
	if (!str.empty())
	{
		try {
			rv = std::stoi(str);
		}
		catch (...) {}

	}
	return rv;
}