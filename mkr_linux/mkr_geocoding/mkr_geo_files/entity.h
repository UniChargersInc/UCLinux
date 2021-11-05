#ifndef MKR_GEO_PARSER_ENTITY_OUTPUT_OBJECT_H_
#define MKR_GEO_PARSER_ENTITY_OUTPUT_OBJECT_H_

/*
	OutputObject - any object (node, linestring, polygon) to be outputted to tiles

	Possible future improvements to save memory:
	- use a global dictionary for attribute key/values
*/

#include <vector>
#include <string>
#include <map>

#include "geo_coordinates.h"

#include "MyEncodingStream.hpp"

#define PROCESS_SHAPEFILE	0

using namespace std;

#include "midgard/distanceapproximator.h"
using namespace valhalla::midgard;

//struct MyLatLon {
//	double lat, lon;
//	MyLatLon(double lat, double lon)
//	{
//		this->lat = lat;
//		this-> lon = lon;
//	}
//};

typedef std::vector</*MyLatLon*/PointLL> MyLinestring;

enum EntityType {
	PLACE_NONE=20,
	COUNTRY, 
	STATE,
	CITY, 
	TOWN, 
	VILLAGE,
	SUBURB,
	HAMLET,
	NEIGHBOURHOOD,
	ISLAND,
	STREET_HOUSE_NUMBER,

	STREET_INTERPOLATION_TYPE =31,
	ADMIN_BOUNDARIES=32,
	STREET_BOUNDARIES = 33,
	BUILDING_BOUNDARIES=34,
	GEO_PLACE_BOUNDARIES=35,
	GEO_PLACES_FUEL = 36
	
};


class GeoEntity
{
public:
	string name;
	string city;      // same as is_in
	EntityType etype;
	uint64_t entid = 0;
	int nameid = -1,cityid=-1;
	int zoom = -1;

	int distance = 0x7fffffff;   // min distance between the Point{lon,lat} and the GeoEntity 

	string getName() {
		return name;
	}
	
	virtual string serializeGeometry() = 0;
	
};

class  GeoPlace  : public GeoEntity
{
public:	
	
	float latp,lon;
	
	GeoPlace(EntityType type)
	{
		this->etype = type;
	}
	string serializeGeometry() { return ""; }
};
struct GeoBuilding  : public GeoEntity
{
public:	
	float lat, lon;
	string houseNumber;
	
	int  distance = -1;
	bool isInsideTheRadius=false;

	std::map<std::string,std::string> additional_tags;  // we need to include additional info like contact:phone, amenity, name, cuisine , etc
	
	// need for geocoding parsing
	GeoBuilding(float lat, float lon, string  houseNumber)
	{
		this->lat = lat;
		this->lon = lon;
		this->houseNumber = houseNumber;
	}
	string serializeGeometry() { return ""; }
};


class GeoStreetInterpolation : public GeoEntity
{
public:
	int interp_type;
	// need only for geocoding paesing
	GeoStreetInterpolation(EntityType type)
	{
		this->etype = type;
	}
	//std::vector <Point> nodes_coord;
	std::vector <GeoBuilding> buildings;
	string serializeGeometry() { return ""; }
};



//enum OutputGeometryType { POINT, LINESTRING, POLYGON, CENTROID, CACHED_POINT, CACHED_LINESTRING, CACHED_POLYGON };
enum OutputGeometryType {NOPOLY, 
#if	BUILD_WIN32_GLSIMULATOR
	G_POINT,
#else
	POINT,
#endif
	LINESTRING, POLYGON, CENTROID, 
	CACHED_POINT, CACHED_LINESTRING, 
	CACHED_MPOLYGON ,   // cached multipolygon (several ways)
	CACHED_WMPOLYGON,    // cached multipolygon (ONLY ONE way)
	CACHED_MP_CENTROID, // this tells that the centroid is created from cached multipolygon
	CACHED_WP_CENTROID  // this tells that the centroid is created from cached ONLY one way polygon
};


class GeoBoundary : public GeoEntity
{
	
public:
	std::string house_number="";    //ONLY for type=EntityType::BUILDING_BOUNDARIES
	uint64_t bnameid = 0;
	MyLinestring  ls;
	GeoBoundary( EntityType type)
	{
		this->etype = type;
	}
	
	MyLinestring  &getLS() { return ls; }
	
	void put_is_in(std::string is_in)	{		this->city = is_in;	}
	string serializeGeometry() { return ""; }
};





#endif //_OUTPUT_OBJECT_H
