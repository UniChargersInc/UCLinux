// mbgl_test.cpp : Defines the entry point for the console application.
//


#include <string>
#include <cstdarg>
#include <sstream>
#include <vector>

#include <unordered_map>
#include <iostream>
#include <fstream>

#include "rapidjson/document.h"
//#include "RouteObj.h"

#include "udorx_common_lite.pb.h"


#include "projections/Projection.h"
#include "projections/EPSG3857.h"
#include "m_routing/RoutingRequest.h"
#include "m_routing/RoutingResult.h"
#include "utils/Const.h"



#include "midgard/encoded.h"
#include "baldr/graphreader.h"
#include "baldr/tilehierarchy.h"
#include "baldr/pathlocation.h"
#include "baldr/connectivity_map.h"
#include "loki/search.h"
#include "sif/costfactory.h"
#include "odin/directionsbuilder.h"
#include "odin/util.h"
//#include "proto/trippath.pb.h"
//#include "proto/tripdirections.pb.h"
//#include "proto/directions_options.pb.h"
#include "midgard/logging.h"
#include "midgard/distanceapproximator.h"
#include "thor/astar.h"
#include "thor/bidirectional_astar.h"
#include "thor/multimodal.h"
//#include "thor/trippathbuilder.h"
#include "thor/attributes_controller.h"
#include "thor/route_matcher.h"

using namespace valhalla::midgard;
using namespace valhalla::baldr;
using namespace valhalla::loki;
using namespace valhalla::odin;
using namespace valhalla::sif;
using namespace valhalla::thor;
using namespace valhalla::meili;


//struct PointLL {
//	float lat;
//	float lon;
//
//	PointLL() {}
//	PointLL(float lat, float lon)
//	{
//		this->lat = lat;
//		this->lon = lon;
//	}
//};


using namespace rapidjson;
using namespace std;

using namespace udorx_comm_lite;

namespace mkr {
namespace routing {
#if 1
	int decode(const std::string& encoded, std::vector<valhalla::midgard::PointLL>& shape);

//http://router.project-osrm.org/route/v1/driving/13.388860,52.517037;13.397634,52.529407;13.428555,52.523219?overview=false&steps=true

//https://github.com/Project-OSRM/osrm-backend/blob/master/docs/http.md

void readBinaryFile(string fname, string &fdata)
{
	//ifstream in(fname.c_str(), ios::binary);
	ifstream in(fname.c_str());
	in.seekg(0, ios::end);
	int iSize = in.tellg();
	in.seekg(0, ios::beg);

	fdata.resize(iSize);//std::vector<char> pBuff(iSize);
	if (iSize > 0)
		in.read((char*)fdata.data(), iSize);//in.read(&pBuff[0], iSize);
	in.close();
}

//*************************************************************************************************************
const std::unordered_map<int, carto::RoutingAction::RoutingAction> maneuver_types = {
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kNone),             carto::RoutingAction::ROUTING_ACTION_NO_TURN },//NoTurn = 0,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kContinue),         carto::RoutingAction::ROUTING_ACTION_GO_STRAIGHT },//GoStraight,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kBecomes),          carto::RoutingAction::ROUTING_ACTION_GO_STRAIGHT },//GoStraight,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kRampStraight),     carto::RoutingAction::ROUTING_ACTION_GO_STRAIGHT },//GoStraight,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kStayStraight),     carto::RoutingAction::ROUTING_ACTION_GO_STRAIGHT },//GoStraight,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kMerge),            carto::RoutingAction::ROUTING_ACTION_GO_STRAIGHT },//GoStraight,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kFerryEnter),       carto::RoutingAction::ROUTING_ACTION_GO_STRAIGHT },//GoStraight,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kFerryExit),        carto::RoutingAction::ROUTING_ACTION_GO_STRAIGHT },//GoStraight,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kSlightRight),      carto::RoutingAction::ROUTING_ACTION_TURN_SLIGHT_RIGHT },//TurnSlightRight,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kRight),            carto::RoutingAction::ROUTING_ACTION_TURN_RIGHT },//TurnRight,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kRampRight),        carto::RoutingAction::ROUTING_ACTION_TURN_RIGHT },//TurnRight,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kExitRight),        carto::RoutingAction::ROUTING_ACTION_TURN_RIGHT },//TurnRight,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kStayRight),        carto::RoutingAction::ROUTING_ACTION_KEEP_RIGHT },//TurnRight,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kSharpRight),       carto::RoutingAction::ROUTING_ACTION_TURN_SHARP_RIGHT },//TurnSharpRight,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kUturnLeft),        carto::RoutingAction::ROUTING_ACTION_UTURN },//UTurn,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kUturnRight),       carto::RoutingAction::ROUTING_ACTION_UTURN_RIGHT },//UTurn,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kSharpLeft),        carto::RoutingAction::ROUTING_ACTION_TURN_SHARP_LEFT },//TurnSharpLeft,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kLeft),             carto::RoutingAction::ROUTING_ACTION_TURN_LEFT },//TurnLeft,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kRampLeft),         carto::RoutingAction::ROUTING_ACTION_TURN_LEFT },//TurnLeft,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kExitLeft),         carto::RoutingAction::ROUTING_ACTION_TURN_LEFT },//TurnLeft,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kStayLeft),         carto::RoutingAction::ROUTING_ACTION_KEEP_LEFT },//TurnLeft,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kSlightLeft),       carto::RoutingAction::ROUTING_ACTION_TURN_SLIGHT_LEFT },//TurnSlightLeft,
																																		 //{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_k),               carto::RoutingAction::ROUTING_ACTION_REACH_VIA_LOCATION },//ReachViaLocation,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kRoundaboutEnter),  carto::RoutingAction::ROUTING_ACTION_ENTER_ROUNDABOUT },//EnterRoundAbout,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kRoundaboutExit),   carto::RoutingAction::ROUTING_ACTION_LEAVE_ROUNDABOUT },//LeaveRoundAbout,
																																				//{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_k),               carto::RoutingAction::ROUTING_ACTION_STAY_ON_ROUNDABOUT },//StayOnRoundAbout,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kStart),            carto::RoutingAction::ROUTING_ACTION_START_AT_END_OF_STREET },//StartAtEndOfStreet,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kStartRight),       carto::RoutingAction::ROUTING_ACTION_START_AT_END_OF_STREET },//StartAtEndOfStreet,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kStartLeft),        carto::RoutingAction::ROUTING_ACTION_START_AT_END_OF_STREET },//StartAtEndOfStreet,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kDestination),      carto::RoutingAction::ROUTING_ACTION_FINISH },//ReachedYourDestination,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kDestinationRight), carto::RoutingAction::ROUTING_ACTION_FINISH },//ReachedYourDestination,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kDestinationLeft),  carto::RoutingAction::ROUTING_ACTION_FINISH },//ReachedYourDestination,
																																	  //{ static_cast<int>valhalla::DirectionsLeg_Maneuver_Type_k),                carto::RoutingAction::ROUTING_ACTION_ENTER_AGAINST_ALLOWED_DIRECTION, },//EnterAgainstAllowedDirection,
																																	  //{ static_cast<int>valhalla::DirectionsLeg_Maneuver_Type_k),                carto::RoutingAction::ROUTING_ACTION_LEAVE_AGAINST_ALLOWED_DIRECTION },//LeaveAgainstAllowedDirection
};

bool TranslateManeuverType(int maneuverType, carto::RoutingAction::RoutingAction& action) {
	auto it = maneuver_types.find(maneuverType);
	if (it != maneuver_types.end()) {
		action = it->second;
		return true;
	}

	//Log::Infof("ValhallaRoutingProxy::TranslateManeuverType: ignoring maneuver %d", maneuverType);
	return false;
}
//*************************************************************************************************************

int route_json_parse(const std::string& json1, PRouteObj& rObj )
{
	string json;
	readBinaryFile("D:\\d_mb_tilemaker\\valhalla\\optimized_route_response.json", json);
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

		assert(document.HasMember("trip"));
		const Value& trip = document["trip"];

		assert(trip["status"].IsInt());
		int status = trip["status"].GetInt();
		rObj.set_code(status==0?"0":"1");
		
		const Value& summary = trip["summary"];

		PRouteO *ro = rObj.add_routes();
		ro->set_distance(summary["length"].GetDouble());//printf("distance = %f\n", v["distance"].GetDouble());
		ro->set_duration(summary["time"].GetDouble());//printf("duration = %f\n", v["duration"].GetDouble());



		const Value& legs = trip["legs"];
		assert(legs.IsArray());
		for (auto& vl : legs.GetArray())
		{
			PLegO *leg=ro->add_legs();
			const Value& svl = vl["summary"];
			double length=svl["length"].GetDouble();
			double time=svl["time"].GetDouble();

			string geometry = vl["shape"].GetString();
			std::vector<PointLL> shape;
			decode(geometry, shape);

			int points_size = 0;
			/*std::vector<carto::MapPos> epsg3857Points;
			carto::EPSG3857 epsg3857;*/

			const Value& steps = vl["maneuvers"];
			assert(steps.IsArray());
			for (auto& stp : steps.GetArray())
			{
				
					PStepO *step=leg->add_steps();

					int maneuver_type= stp["type"].GetInt();

					carto::RoutingAction::RoutingAction action = carto::RoutingAction::ROUTING_ACTION_NO_TURN;
					TranslateManeuverType(static_cast<int>(maneuver_type), action);
					
					int ind1 = stp["begin_shape_index"].GetInt(); 
					int ind2 = stp["end_shape_index"].GetInt();

					std::string instr= stp["instruction"].GetString();
					
					
					for (int j = ind1; j <= ind2; j++) {   // we have a duplicated points here !!!
						const PointLL& point = shape.at(j);
						float lat = point.lat(), lon = point.lng();
						//carto::MapPos pp1 = epsg3857.fromLatLong(lat, lon);
						////carto::MapPos pp(point.lng(), point.lat());

						//{
						//	epsg3857Points.push_back(pp1);
						//	//points.push_back(pp);
						//}
						/*PPointLL *p = ro->add_geo_points();
						p->set_lon(lon);
						p->set_lat(lat);

						p = step->add_step_points();
						p->set_lon(lon);
						p->set_lat(lat);*/

						points_size++;
					}



					
					
				
				
			}
			
		}

	}

	return 0;
}



//============================Polyline parser======================
constexpr double kPolylinePrecision = 1E6;
constexpr double kInvPolylinePrecision = 1.0 / kPolylinePrecision;

int decode(const std::string& encoded, std::vector<valhalla::midgard::PointLL>& shape) {
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

		
		
		// Shift the decimal point 5 places to the left
		valhalla::midgard::PointLL pp;
		pp.set_x(static_cast<float>(static_cast<double>(lon) *	kInvPolylinePrecision));
		pp.set_y(static_cast<float>(static_cast<double>(lat) *	kInvPolylinePrecision));
		shape.emplace_back(pp);
				
		
		// Remember the last one we encountered
		last_lon = lon;
		last_lat = lat;
	}
	return 1;
}
#endif
} // namespace routing
} // namespace mkr

int route_json_parse_test()
{
	PRouteObj rObj;
	int rv = mkr::routing::route_json_parse("", rObj);
	return rv;

}