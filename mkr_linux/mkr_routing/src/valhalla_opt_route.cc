#include <boost/format.hpp>
#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <queue>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "baldr/connectivity_map.h"
#include "baldr/graphreader.h"
#include "baldr/pathlocation.h"
#include "baldr/tilehierarchy.h"
#include "loki/search.h"
#include "loki/worker.h"
#include "midgard/distanceapproximator.h"
#include "midgard/encoded.h"
#include "midgard/logging.h"
#include "odin/directionsbuilder.h"
#include "odin/enhancedtrippath.h"
#include "odin/util.h"
#include "sif/costfactory.h"
#include "thor/astar.h"
#include "thor/attributes_controller.h"
#include "thor/bidirectional_astar.h"
#include "thor/multimodal.h"
#include "thor/route_matcher.h"
#include "thor/timedep.h"
#include "thor/triplegbuilder.h"
#include "worker.h"

#include "thor/isochrone.h"
#include "thor/worker.h"
#include "tyr/actor.h"

#include "odin/worker.h"
#include "tyr/serializers.h"




//#include <valhalla/proto/directions.pb.h>
//#include <valhalla/proto/options.pb.h>
//#include <valhalla/proto/trip.pb.h>

#include "config.h"
#ifdef WIN32
#include "win_valhalla.h"   // to read config file
#elif defined MKR_LINUX
#include "linux_valhalla.h"   // to read config file
#endif

#include "projections/Projection.h"
#include "projections/EPSG3857.h"
#include "m_routing/RoutingRequest.h"
#include "m_routing/RoutingResult.h"
#include "utils/Const.h"

#ifdef ANDROID
#include "MkrHeader.h" //"Dbg.h"
#endif
#ifdef WIN32 
#include <algorithm>
#include "my_lib/my_lib.h"
#endif

#ifdef MKR_LINUX
#include "my_lib/my_lib.h"
#endif

using namespace valhalla;
using namespace valhalla::tyr;
using namespace valhalla::midgard;
using namespace valhalla::baldr;
using namespace valhalla::loki;
using namespace valhalla::odin;
using namespace valhalla::sif;
using namespace valhalla::thor;
using namespace valhalla::meili;

namespace bpo = boost::program_options;



float CalculateTurnAngle(const std::vector<carto::MapPos>& epsg3857Points, int pointIndex);
bool TranslateManeuverType(int maneuverType, carto::RoutingAction::RoutingAction& action);


namespace {

	// Default maximum distance between locations to choose a time dependent path algorithm
	const float kDefaultMaxTimeDependentDistance = 500000.0f;

	
} // namespace

static int process_vleg(const valhalla::DirectionsLeg &leg_directions, PRouteO *rO, PLegO *rLeg,const valhalla::Options &options)
{
	//----------------------------------------------------------------------------------------------

	//int points_size = 0;
	/*std::vector<carto::MapPos> epsg3857Points;
	carto::EPSG3857 epsg3857;*/

	std::vector<valhalla::midgard::PointLL> shape = valhalla::midgard::decode<std::vector<PointLL> >(leg_directions.shape());
	//points.reserve(points.size() + shape.size());
	//epsg3857Points.reserve(epsg3857Points.size() + shape.size());
	//MKR_LOGI("getRoute_6_1 maneuver_size()=%d  shape_size=%d", trip_directions.maneuver_size(), shape.size());
	for (int i = 0; i < shape.size(); i++)
	{
		const valhalla::midgard::PointLL& point = shape.at(i);
		float lat = point.lat(), lon = point.lng();
		PPointLL *p = rLeg->add_geo_points();
		p->set_lon(lon);
		p->set_lat(lat);
	}
	for (int i = 0; i < leg_directions.maneuver_size(); i++) {
		const /*valhalla::odin::TripDirections_Maneuver*/auto& maneuver = leg_directions.maneuver(i);

		PStepO *rStep = rLeg->add_steps(); // add step

		carto::RoutingAction::RoutingAction action = carto::RoutingAction::ROUTING_ACTION_NO_TURN;
		//MKR_LOGI("getRoute_6_1 type=%d", maneuver.type());
		TranslateManeuverType(static_cast<int>(maneuver.type()), action);
		//MKR_LOGI("getRoute_6_1 manInd=%d action=%d",i, action);
		//std::size_t pointIndex = points_size;// points.size();

		rStep->set_point_start_index(maneuver.begin_shape_index());
		rStep->set_point_end_index(maneuver.end_shape_index());
		
		//float turnAngle = CalculateTurnAngle(epsg3857Points, pointIndex);
		int  s_size = maneuver.street_name_size();
		std::string name = s_size>0 ? maneuver.street_name(0).value() : std::string("");
		if (s_size >= 2)
		{
			std::string s = maneuver.street_name(1).value();
			if (s.size() > name.size())
				name = s;
		}
		//MKR_LOGI("getRoute_6_1 Index=%d man name=%s act=%d head=%d instr=%s vpts=%s length=%f time=%d", i, name.c_str(),action, maneuver.begin_heading(), maneuver.text_instruction().c_str(), maneuver.verbal_post_transition_instruction().c_str(), maneuver.length(), maneuver.time());
		PManeuverO *rMnv = new PManeuverO();
		rMnv->set_action(action); // required field
		rMnv->set_type("");
		rMnv->set_modifier("");
		rMnv->set_azimuth(maneuver.begin_heading());
		rMnv->set_textinstr(maneuver.text_instruction());
		rStep->set_allocated_mnv(rMnv);

		rStep->set_postverinst(maneuver.verbal_post_transition_instruction());
		//rStep->set_pointindex(pointIndex);
		rStep->set_distance(maneuver.length() * 1000.0);
		rStep->set_name(name);
		rStep->set_time(maneuver.time());
		//MKR_LOGI("getRoute_6_1 LAST Index=%d man name=%s", i, name.c_str());
	}

	auto trip_time = leg_directions.summary().time();
	auto trip_length = leg_directions.summary().length() * (options.units() == valhalla::Options_Units::Options_Units_kilometers ? 1000.0f : 1609.344f);

	//LOG_INFO("trip_time (secs)::" + std::to_string(trip_time));
	//LOG_INFO("trip_length (meters)::" + std::to_string(trip_length));
	/*data.setSuccess("success");*/

	// make sure all required fields 
	rLeg->set_summary("Summary");
	rLeg->set_distance(trip_time);
	rLeg->set_duration(trip_length);

	//----------------------------------------------------------------------------------------------
	//MKR_LOGI("getRoute_6_1 RETURN");
	return EXIT_SUCCESS;
}

static int getIndexInOriginalLocs(int k,const ::valhalla::LatLng& ll, const std::vector</*carto::MapPos*/valhalla::LatLng>& pnts)
{
	/*carto::MapPos*/const valhalla::LatLng &pos = pnts[k];
	float x0 = pos.lng(), y0 = pos.lat();
	float x = ll.lng(), y = ll.lat();
	if (x0 == x && y0 == y)
		return k;
	// if not find the match
	for (int j = 0; j < pnts.size(); j++)
	{
		const valhalla::LatLng &p = pnts[j];
		x0 = p.lng(), y0 = p.lat();
		if (x0 == x && y0 == y)
			return j;

	}
	return -1;
}

static int __main_run_route(const std::shared_ptr<carto::RoutingRequest>& r_request, std::shared_ptr<carto::RoutingResult>& result)
//static int __main_run_route()
{
	int rv = -1;

	bool multi_run = false;
	bool match_test = false;
	uint32_t iterations=0;

	

	valhalla::Api request;
	valhalla::ParseApi("{}", valhalla::Options::route, request);
	valhalla::Options* pOpt = request.mutable_options();
   
	const std::vector</*carto::MapPos*/valhalla::LatLng>& pnts = (std::vector</*carto::MapPos*/valhalla::LatLng>&) r_request->getPoints();

	for (auto &p : pnts)
	{
		
		valhalla::Location *pLoc =pOpt->add_locations();
		valhalla::LatLng *pLL = new valhalla::LatLng();
		pLL->set_lat(/*p.getY()*/p.lat());
		pLL->set_lng(/*p.getX()*/p.lng());
		
		pLoc->set_allocated_ll(pLL);

		pLoc->set_node_snap_tolerance(5);
		pLoc->set_search_cutoff(35000);
		pLoc->set_heading_tolerance(60);
		pLoc->set_street_side_tolerance(5);
	}
	pOpt->set_costing(valhalla::Costing::auto_);
	pOpt->set_units(valhalla::Options_Units::Options_Units_kilometers);
	//pOpt->set_search_radius(10);
	const auto& options = request.options();



	// Get type of route - this provides the costing method to use.
	std::string routetype = valhalla::Costing_Name(options.costing());
	
	// Locations
	auto locations = valhalla::baldr::PathLocation::fromPBF(options.locations());
	if (locations.size() < 2) {
		throw;
	}

	// parse the config
	boost::property_tree::ptree pt;
	{
		std::stringstream stream;
#ifdef WIN32
		std::string config = std::string((const char*)win_valhalla_bytes, win_valhalla_len);
#elif defined MKR_LINUX
		std::string config = std::string((const char*)linux_valhalla_bytes, linux_valhalla_len);
#endif
		stream << config;
		rapidjson::read_json(stream, pt);
	}
	

	//LOG_INFO("getRoute_1: ");

	// Something to hold the statistics
	uint32_t n = locations.size() - 1;
	
	// Crow flies distance between locations (km)
	float d1 = 0.0f;
	for (uint32_t i = 0; i < n; i++) {
		d1 += locations[i].latlng_.Distance(locations[i + 1].latlng_) * kKmPerMeter;
	}

	// Get something we can use to fetch tiles
	valhalla::baldr::GraphReader reader(pt.get_child("mjolnir"));

	// Get the maximum distance for time dependent routes
	float max_timedep_distance =
		pt.get<float>("service_limits.max_timedep_distance", kDefaultMaxTimeDependentDistance);

	
	// Construct costing
	CostFactory<DynamicCost> factory;
	factory.RegisterStandardCostingModels();

	// Get the costing method - pass the JSON configuration
	TravelMode mode;
	std::shared_ptr<DynamicCost> mode_costing[4];
	if (routetype == "multimodal") {
		// Create array of costing methods per mode and set initial mode to
		// pedestrian
		mode_costing[0] = factory.Create(valhalla::Costing::auto_, options);
		mode_costing[1] = factory.Create(valhalla::Costing::pedestrian, options);
		mode_costing[2] = factory.Create(valhalla::Costing::bicycle, options);
		mode_costing[3] = factory.Create(valhalla::Costing::transit, options);
		mode = TravelMode::kPedestrian;
	}
	else {
		// Assign costing method, override any config options that are in the
		// json request
		std::shared_ptr<DynamicCost> cost = factory.Create(options.costing(), options);
		mode = cost->travel_mode();
		mode_costing[static_cast<uint32_t>(mode)] = cost;
	}

	
	// Find path locations (loki) for sources and targets
	//printf("start loki_worker_t lw(pt);\r\n");
	loki_worker_t lw(pt);
	lw.matrix(request);//lw.route(request);
	//printf("start thor_worker_t thor_worker(pt);\r\n");
	thor_worker_t thor_worker(pt);
	thor_worker.optimized_route(request);

	//printf("odin_worker_t odin_worker(pt)\r\n");
	odin_worker_t odin_worker(pt);
	// narrate them and serialize them along
	odin_worker.narrate(request);
	//auto response = tyr::serializeDirections(request);

	//printf("odin_worker_t odin_worker(pt)...DONE\r\n");
#if 0
	//just for test
	{
		for (auto &p : pnts)
		{
			printf("Or p lon=%f lat=%f\r\n", p.getX(), p.getY());
		}


		/* options.locations
		The specified array of lat/lngs from the input request. The first and last locations in the array will remain the same as the input request. 
		The intermediate locations may be returned reordered in the response. Due to the reordering of the intermediate locations, an original_index is also part of the locations object within the response. 
		This is an identifier of the location index that will allow a user to easily correlate input locations with output locations.
		*/

		int len=options.locations_size();
		for (int k = 0; k < len; k++) {
			::valhalla::Location* pLL = pOpt->mutable_locations(k);   //.mutable_locations(k);
			const ::valhalla::LatLng& ll = pLL->ll();
			printf("Opt lon=%f lat=%f\r\n", ll.lng(), ll.lat());
		}
	}
#endif

	std::shared_ptr<PRouteObj> rObj = std::make_shared<PRouteObj>();

	//fill the waypoints
	int len = options.locations_size();
	for (int k = 0; k < len; k++) {
		::valhalla::Location* pLL = pOpt->mutable_locations(k);   //.mutable_locations(k);
		const ::valhalla::LatLng& ll = pLL->ll();

		int index = getIndexInOriginalLocs(k,ll, pnts);

		PWayPoint *wp = rObj->add_waypoints();
		wp->set_name(std::to_string(index));
		PPointLL *pP = new PPointLL();
		pP->set_lat(ll.lat());
		pP->set_lon(ll.lng());
		wp->set_allocated_wploc(pP);
	}


	int rsize = request.directions().routes_size();
	if (rsize > 0)
	{
		for (int j = 0; j < rsize; j++)
		{
			const valhalla::DirectionsRoute &dir_route = request.directions().routes(j);  // get valhalla dir route
			PRouteO *rO = rObj->add_routes();    // create mkr route
			int size = dir_route.legs_size();
			if (size > 0)
			{
				for (int i = 0; i < size; i++)
				{
					const auto& vleg = dir_route.legs(i);  // get valhalla leg
					PLegO *rLeg = rO->add_legs();
					bool rv = process_vleg(vleg, rO, rLeg, options);

				}
			}

		}
		/*std::string rStr = "Route dist:" + std::to_string(trip_length) + " time:" + std::to_string(trip_time);
		rObj->set_code(rStr);
		rO->set_distance(trip_length);
		rO->set_duration(trip_time);*/

		
			std::shared_ptr<carto::Projection> proj = r_request->getProjection();
			result = std::make_shared<carto::RoutingResult>(proj, rObj);
			rObj->set_code("1");
			rv = EXIT_SUCCESS;
		

	}
	else {
		rv = -6;
	}

	return rv;
}


//===================================================

static int main_run_opt_route(const std::shared_ptr<carto::RoutingRequest>& r_request, std::shared_ptr<carto::RoutingResult>& result)
{
	int rv = 0;
	try 
	{
		rv=__main_run_route(r_request, result);
	}
	catch (std::exception &e)
	{
		//m_printf("main_run_route..exc:%s",e.what());
		rv = -11;
	}
	catch (...)
	{
		//MKR_LOGI_LL("getRoute..exc: ...");
		rv = -10;
	}
	return rv;
}

#if (defined WIN32 || defined MKR_LINUX)



//::http://192.168.0.15:8002/optimized_route?json={%22locations%22:[{%22lat%22:43.849999,%22lon%22:-79.501430},
//{%22lat%22:43.750300,%22lon%22:-79.423706},
//{%22lat%22:43.750300,%22lon%22:-79.453706},
//{%22lat%22:43.770300,%22lon%22:-79.363706},
//{%22lat%22:43.760300,%22lon%22:-79.353706}],
//%22costing%22:%22auto%22,%22directions_options%22:{%22units%22:%22miles%22}}
bool createOptRoute(std::vector<float> &vIn, std::string& resPtr)
{
	//float sLat, float sLon, float destLat, float destLon,
	PRouteObj *resV=NULL ;

	std::vector</*carto::MapPos*/valhalla::LatLng> v;
#if 1
	/*carto::MapPos p0(sLon, sLat);
	v.push_back(p0);

	carto::MapPos p1(destLon, destLat);
	v.push_back(p1);*/
	int size = vIn.size();
	if (size < 4 || (size % 2))
		return false;

	for (int i = 0; i < size; i += 2)
	{
		double lat = vIn[i + 0];
		double lon = vIn[i + 1];

		//carto::MapPos p0(lon, lat);
		valhalla::LatLng p0;
		p0.set_lat(lat);
		p0.set_lng(lon);
		v.push_back(p0);
	}
	//add first point to return back
	v.push_back(v[0]);                   //!!!
#else

	carto::MapPos p0(-79.501430, 43.849999);//carto::MapPos p0(sLon, sLat); :43.849999,%22lon%22:-79.501430
	v.push_back(p0);

	carto::MapPos p1(-79.423706, 43.750300);	                                      //carto::MapPos p1(destLon, destLat);  43.750300,%22lon%22:-79.423706
	v.push_back(p1);

	/*carto::MapPos p2(-79.453706, 43.750300);	                                    
	v.push_back(p2);

	carto::MapPos p3(-79.363706, 43.770300);
	v.push_back(p3);

	carto::MapPos p4(-79.353706, 43.760300);
	v.push_back(p4);*/

	
#endif

	std::shared_ptr<carto::EPSG3857> proj = std::make_shared<carto::EPSG3857>();
	std::shared_ptr<carto::RoutingRequest> request = std::make_shared<carto::RoutingRequest>(proj, v);
	std::shared_ptr<carto::RoutingResult> result = nullptr;

	int rv = main_run_opt_route(request, result);
	//m_printf(" createOptRoute_0.. rv=%d",rv);
	if (rv == 0 && result != nullptr)
	{
		//m_printf(" createOptRoute_0..0");
		resV = result->getRouteObj().get();
		if (resV->code() == "1")
		{

			//this->r_route = result;
			//if (voiceRouter)voiceRouter->setRoute(result); // reference has been increased two times !!!
			//m_printf(" createRoute_OK..\r\n");
			
			////fill the waypoints
			//for (carto::MapPos &pos : v)
			//{
			//	PWayPoint *wp=resV->add_waypoints();
			//	wp->set_name("");
			//	PPointLL *pP = new PPointLL();
			//	pP->set_lat(pos.getY());
			//	pP->set_lon(pos.getX());
			//	wp->set_allocated_wploc(pP);
			//}

#ifdef WIN32
			/*void dumpRoute(PRouteObj *_rObj);
			dumpRoute(resV);*/
#endif
		}

	}
	else {
		//this->r_route = nullptr;   // !!!!!!!
		resV = new PRouteObj();   // create object just to pass error code to java thru jni   ( delete it in jni function)
		std::string code = std::to_string(rv);
		resV->set_code(code);
		//m_printf(" createOptRoute_ERR..%d code=%s\r\n", rv,code.c_str());

	}
	
	if (resV != NULL)
	{
		resPtr = resV->SerializeAsString();
		return true;
	}

	return false;
}

#if 0
//-j '{"locations":[{"lat":43.8499,"lon":-79.50193,"type":"break"},{"lat":43.7603,"lon":-79.3537,"type":"break"}],"costing":"auto","directions_options":{"units":"miles"}}' --config  D:\d_mb_tilemaker\valhalla\win_valhalla.json
int main_opt_run_route(int argc, char *argv[])
{
	std::string resPtr;
	
	std::vector<double> v;
	v.push_back(43.849999);  //-79.501430, 43.849999
	v.push_back(-79.501430);

	v.push_back(43.750300);   //-79.423706, 43.750300
	v.push_back(-79.423706);
	
	v.push_back(43.750300); //-79.453706, 43.750300
	v.push_back(-79.453706);

	bool rv = createOptRoute(v, resPtr);
	if(rv)
	{
		printf("res size=%d\r\n", resPtr.size());
	}
	return 0;
}
#endif

#endif