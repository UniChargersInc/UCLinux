// mbgl_test.cpp : Defines the entry point for the console application.
//


#include <string>
#include <cstdarg>
#include <sstream>
#include <vector>


#include <iostream>
#include <fstream>

#include "rapidjson/document.h"
//#include "RouteObj.h"

#include "udorx_common_lite.pb.h"

using namespace rapidjson;
using namespace std;

using namespace udorx_comm_lite;

namespace mkr {
namespace routing {
#if 1
	int decode(const std::string& encoded, /*std::vector<PointLL>& shape*/ PStepO **step);

//http://router.project-osrm.org/route/v1/driving/13.388860,52.517037;13.397634,52.529407;13.428555,52.523219?overview=false&steps=true

//https://github.com/Project-OSRM/osrm-backend/blob/master/docs/http.md

//void readBinaryFile(string fname, string &fdata)
//{
//	//ifstream in(fname.c_str(), ios::binary);
//	ifstream in(fname.c_str());
//	in.seekg(0, ios::end);
//	int iSize = in.tellg();
//	in.seekg(0, ios::beg);
//
//	fdata.resize(iSize);//std::vector<char> pBuff(iSize);
//	if (iSize > 0)
//		in.read((char*)fdata.data(), iSize);//in.read(&pBuff[0], iSize);
//	in.close();
//}


int route_json_parse(const std::string& json, PRouteObj& rObj )
{
	//string json;
	//readBinaryFile("route1.json", json);
	int size = json.size();
	if (size > 0)
	{
		//printf("%s", json.c_str());

		//http://rapidjson.org/md_doc_tutorial.html
		/*rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> document;
		document.Parse<0>(json.c_str());*/

		Document document;
		document.Parse(json.c_str());

		assert(document.IsObject());

		assert(document.HasMember("code"));
		assert(document["code"].IsString());
		rObj.set_code(document["code"].GetString());
		//printf("code = %s\n", document["code"].GetString());

		
		const Value& wp1 = document["waypoints"];
		assert(wp1.IsArray());
		for (auto& v : wp1.GetArray())
		{
			//WayPointO wp;
			PWayPoint *wp=rObj.add_waypoints();
			wp->set_name(v["name"].GetString());//printf("name = %s\n", v["name"].GetString());
			const Value& L = v["location"];
			assert(L.IsArray());
			if (L.Size() >= 2)            // location : A  [longitude, latitude] 
			{
				PPointLL *p=wp->mutable_wploc();
				p->set_lon ( L[0].GetDouble() );
				p->set_lat ( L[1].GetDouble() );
				//printf("Location lat= %f lon=%f\n", L[1].GetDouble(), L[0].GetDouble());
				//rObj.waypoints.emplace_back(wp);
				
			}
		}


		const Value& rr = document["routes"];
		assert(rr.IsArray());
		for (auto& v : rr.GetArray())
		{
			PRouteO *ro = rObj.add_routes();
			//printf("route %d ", v.GetType());
			

			ro->set_distance ( v["distance"].GetDouble() );//printf("distance = %f\n", v["distance"].GetDouble());
			ro->set_duration ( v["duration"].GetDouble() );//printf("duration = %f\n", v["duration"].GetDouble());

			/*A RouteLeg object defines a single leg of a route between two waypoints. 
			If the overall route has only two waypoints, it has a single RouteLeg object that covers the entire route. 
			The route leg object includes information about the leg, such as its name, distance, and expected travel time. 
			Depending on the criteria used to calculate the route, the route leg object may also include detailed turn-by-turn instructions.
			*/

			const Value& Legs = v["legs"];
			assert(Legs.IsArray());
			for (auto& ll : Legs.GetArray())
			{
				PLegO *leg=ro->add_legs();
				leg->set_distance ( ll["distance"].GetDouble() );
				leg->set_duration ( ll["duration"].GetDouble() );
				leg->set_summary ( ll["summary"].GetString() );
				//printf("leg distance = %f duration=%f \n", ll["distance"].GetDouble(), ll["duration"].GetDouble());
				//printf("leg summary = %s\n", ll["summary"].GetString());

				
				const Value& steps = ll["steps"];
				assert(steps.IsArray());
				for (auto& ss : steps.GetArray())
				{
					PStepO *step=leg->add_steps();
					string geometry = ss["geometry"].GetString();
					//printf("steps : geometry:%s\r\n", geometry.c_str());

					decode(geometry, &step);
					/*if (p_points.size() > 0)
					{
						for (int i = 0; i<p_points.size(); i++)
						{
							PointLL pp = p_points[i];
							printf("geometry:lat=%f lon=%f\r\n", pp.lat, pp.lon);
						}
					}*/


					step->set_distance ( ss["distance"].GetDouble() );
					step->set_name( ss["name"].GetString() );
					//printf("distance = %f\n", ss["distance"].GetDouble());
					//printf("name = %s\n", ss["name"].GetString());

					const Value& manv = ss["maneuver"];
					PManeuverO *mnv = step->mutable_mnv();
					mnv->set_type ( ""), mnv->set_modifier ( "" );
					if (manv.HasMember("type"))
						mnv->set_type ( manv["type"].GetString() );
					if (manv.HasMember("modifier"))
						mnv->set_modifier ( manv["modifier"].GetString() );
					//printf("maneuver = %s %s\r\n", type.c_str(), mm.c_str());
					const Value& Loc = manv["location"];
					assert(Loc.IsArray());
					if (Loc.Size() >= 2)
					{
						//PPointLL *p = ro->mutable_geo_points();
						////printf("manv lat= %f lon=%f\n", Loc[1].GetDouble(), Loc[0].GetDouble());
						//p->set_lon ( Loc[0].GetDouble() );
						//p->set_lat ( Loc[1].GetDouble() );
					}
					
				}
				
			}
			
		}

	}

	return 0;
}



//============================Polyline parser======================
constexpr double kPolylinePrecision = 1E5;
constexpr double kInvPolylinePrecision = 1.0 / kPolylinePrecision;

int decode(const std::string& encoded, /*std::vector<PointLL>& shape*/PStepO **step) {
	int i = 0;     // what byte are we looking at

					  // Handy lambda to turn a few bytes of an encoded string into an integer
	auto deserialize = [&encoded, &i](const int previous) {
		// Grab each 5 bits and mask it in where it belongs using the shift
		int byte, shift = 0, result = 0;
		do {
			byte = static_cast<int>(encoded[i++]) - 63;
			result |= (byte & 0x1f) << shift;
			shift += 5;
		} while (byte >= 0x20);
		// Undo the left shift from above or the bit flipping and add to previous
		// since its an offset
		return previous + (result & 1 ? ~(result >> 1) : (result >> 1));
	};

	// Iterate over all characters in the encoded string
	//std::vector<PointLL> shape;
	int last_lon = 0, last_lat = 0;
	while (i < encoded.length()) {
		// Decode the coordinates, lat first for some reason

		int lat = deserialize(last_lat);
		int lon = deserialize(last_lon);

		PStepO *s = *step;
		PPointLL *p = s->add_step_points();
		p->set_lat(static_cast<float>(static_cast<double>(lat) * kInvPolylinePrecision));
		p->set_lon(static_cast<float>(static_cast<double>(lon) * kInvPolylinePrecision));

		// Shift the decimal point 5 places to the left
		/*shape.emplace_back(static_cast<float>(static_cast<double>(lat) *
			kInvPolylinePrecision),
			static_cast<float>(static_cast<double>(lon) *
				kInvPolylinePrecision));
				*/

				// Remember the last one we encountered
		last_lon = lon;
		last_lat = lat;
	}
	return 1;
}
#endif
} // namespace routing
} // namespace mkr