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

#include <valhalla/proto/directions.pb.h>
#include <valhalla/proto/options.pb.h>
#include <valhalla/proto/trip.pb.h>

#include "config.h"
#ifdef WIN32
#include "win_valhalla.h"   // to read config file
#include "my_lib/my_lib.h"
#elif defined MKR_LINUX
#include "linux_valhalla.h"   // to read config file
#elif defined ANDROID || defined (__APPLE__)
#include "android_valhalla.h"   // to read config file
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
#define m_printf printf
#endif

#ifdef MKR_LINUX
#include "my_lib/my_lib.h"
#endif

using namespace valhalla::midgard;
using namespace valhalla::baldr;
using namespace valhalla::loki;
using namespace valhalla::odin;
using namespace valhalla::sif;
using namespace valhalla::thor;
using namespace valhalla::meili;

namespace bpo = boost::program_options;


#define VALHALLA_VERSION "3.0.6"
//#include "components/Exceptions.h"
float CalculateTurnAngle(const std::vector<carto::MapPos>& epsg3857Points, int pointIndex);
bool TranslateManeuverType(int maneuverType, carto::RoutingAction::RoutingAction& action);


namespace {

	// Default maximum distance between locations to choose a time dependent path algorithm
	const float kDefaultMaxTimeDependentDistance = 500000.0f;

	
} // namespace

/**
 * Test a single path from origin to destination.
 */
const valhalla::TripLeg* PathTest(GraphReader& reader,
	valhalla::Location& origin,
	valhalla::Location& dest,
	PathAlgorithm* pathalgorithm,
	const std::shared_ptr<DynamicCost>* mode_costing,
	const TravelMode mode,
	/*PathStatistics&*/void* data,
	bool multi_run,
	uint32_t iterations,
	bool using_astar,
	bool using_bd,
	bool match_test,
	const std::string& routetype,
	valhalla::Api& request) {
	
	auto paths = pathalgorithm->GetBestPath(origin, dest, reader, mode_costing, mode);
	cost_ptr_t cost = mode_costing[static_cast<uint32_t>(mode)];

	// If bidirectional A*, disable use of destination only edges on the first pass.
	// If there is a failure, we allow them on the second pass.
	if (using_bd) {
		cost->set_allow_destination_only(false);
	}

	cost->set_pass(0);
	
	if (paths.empty() || (routetype == "pedestrian" && pathalgorithm->has_ferry())) {
		if (cost->AllowMultiPass()) {
			LOG_INFO("Try again with relaxed hierarchy limits");
			cost->set_pass(1);
			pathalgorithm->Clear();
			float relax_factor = (using_astar) ? 16.0f : 8.0f;
			float expansion_within_factor = (using_astar) ? 4.0f : 2.0f;
			cost->RelaxHierarchyLimits(using_astar, expansion_within_factor);
			cost->set_allow_destination_only(true);
			paths = pathalgorithm->GetBestPath(origin, dest, reader, mode_costing, mode);
			
		}
	}
	if (paths.empty()) {
		// Return an empty trip path
		return nullptr;
	}
	const auto& pathedges = paths.front();

	
	// Form trip path
	
	AttributesController controller;
	auto& trip_path = *request.mutable_trip()->mutable_routes()->Add()->mutable_legs()->Add();
	TripLegBuilder::Build(controller, reader, mode_costing, pathedges.begin(), pathedges.end(), origin,
		dest, std::list<valhalla::Location>{}, trip_path);
	

	// Time how long it takes to clear the path
	
	pathalgorithm->Clear();
	
	// Test RouteMatcher
	if (match_test) {
		LOG_INFO("Testing RouteMatcher");

		// Get shape
		std::vector<PointLL> shape = decode<std::vector<PointLL>>(trip_path.shape());
		std::vector<Measurement> trace;
		trace.reserve(shape.size());
		std::transform(shape.begin(), shape.end(), std::back_inserter(trace), [](const PointLL& p) {
			return Measurement{ p, 0, 0 };
			});

		// Use the shape to form a single edge correlation at the start and end of
		// the shape (using heading).
		std::vector<valhalla::baldr::Location> locations{ shape.front(), shape.back() };
		locations.front().heading_ = std::round(PointLL::HeadingAlongPolyline(shape, 30.f));
		locations.back().heading_ = std::round(PointLL::HeadingAtEndOfPolyline(shape, 30.f));

		std::shared_ptr<DynamicCost> cost = mode_costing[static_cast<uint32_t>(mode)];
		const auto projections = Search(locations, reader, cost->GetEdgeFilter(), cost->GetNodeFilter());
		std::vector<PathLocation> path_location;
		valhalla::Options options;
		for (const auto& loc : locations) {
			path_location.push_back(projections.at(loc));
			PathLocation::toPBF(path_location.back(), options.mutable_locations()->Add(), reader);
		}
		std::vector<PathInfo> path;
		bool ret =
			RouteMatcher::FormPath(mode_costing, mode, reader, trace, false, options.locations(), path);
		if (ret) {
			LOG_INFO("RouteMatcher succeeded");
		}
		else {
			LOG_ERROR("RouteMatcher failed");
		}
	}

	// Run again to see benefits of caching
	if (multi_run) {
		uint32_t totalms = 0;
		for (uint32_t i = 0; i < iterations; i++) {
			
			paths = pathalgorithm->GetBestPath(origin, dest, reader, mode_costing, mode);
			
			pathalgorithm->Clear();
		}
		
	}
	return &request.trip().routes(0).legs(0);
}

namespace std {
#if 0
	// TODO: maybe move this into location.h if its actually useful elsewhere than here?
	std::string to_string(const valhalla::baldr::Location& l) {
		std::string s;
		for (auto address : { &l.name_, &l.street_, &l.city_, &l.state_, &l.zip_, &l.country_ }) {
			s.append(*address);
			s.push_back(',');
		}
		s.erase(s.end() - 1);
		return s;
	}

	// TODO: maybe move this into location.h if its actually useful elsewhere than here?
	std::string to_json(const valhalla::baldr::Location& l) {
		std::string json = "{";
		json += "\"lat\":";
		json += std::to_string(l.latlng_.lat());

		json += ",\"lon\":";
		json += std::to_string(l.latlng_.lng());

		json += ",\"type\":\"";
		json += (l.stoptype_ == valhalla::baldr::Location::StopType::THROUGH) ? "through" : "break";
		json += "\"";

		if (l.heading_) {
			json += ",\"heading\":";
			json += *l.heading_;
		}

		if (!l.name_.empty()) {
			json += ",\"name\":\"";
			json += l.name_;
			json += "\"";
		}

		if (!l.street_.empty()) {
			json += ",\"street\":\"";
			json += l.street_;
			json += "\"";
		}

		if (!l.city_.empty()) {
			json += ",\"city\":\"";
			json += l.city_;
			json += "\"";
		}

		if (!l.state_.empty()) {
			json += ",\"state\":\"";
			json += l.state_;
			json += "\"";
		}

		if (!l.zip_.empty()) {
			json += ",\"postal_code\":\"";
			json += l.zip_;
			json += "\"";
		}

		if (!l.country_.empty()) {
			json += ",\"country\":\"";
			json += l.country_;
			json += "\"";
		}

		json += "}";

		return json;
	}
#endif
} // namespace std
#if 0
std::string GetFormattedTime(uint32_t seconds) {
	uint32_t hours = (uint32_t)seconds / 3600;
	uint32_t minutes = ((uint32_t)(seconds / 60)) % 60;
	std::string formattedTime = "";
	// Hours
	if (hours > 0) {
		formattedTime += std::to_string(hours);
		formattedTime += (hours == 1) ? " hour" : " hours";
		if (minutes > 0) {
			formattedTime += ", ";
		}
	}
	// Minutes
	if (minutes > 0) {
		formattedTime += std::to_string(minutes);
		formattedTime += (minutes == 1) ? " minute" : " minutes";
	}
	return formattedTime;
}
#endif

valhalla::DirectionsLeg DirectionsTest(valhalla::Api& api,
	valhalla::Location& orig,
	valhalla::Location& dest,
	/*PathStatistics& */ void* data) 
{
#if 0
	// TEMPORARY? Change to PathLocation...
	const PathLocation& origin = PathLocation::fromPBF(orig);
	const PathLocation& destination = PathLocation::fromPBF(dest);
#endif
	DirectionsBuilder::Build(api);
	const auto& trip_directions = api.directions().routes(0).legs(0);
#if 0
	EnhancedTripLeg etl(*api.mutable_trip()->mutable_routes(0)->mutable_legs(0));
	std::string units = (api.options().units() == valhalla::Options::kilometers ? "km" : "mi");
	int m = 1;
	valhalla::midgard::logging::Log("From: " + std::to_string(origin), " [NARRATIVE] ");
	valhalla::midgard::logging::Log("To: " + std::to_string(destination), " [NARRATIVE] ");
	valhalla::midgard::logging::Log("==============================================", " [NARRATIVE] ");
	for (int i = 0; i < trip_directions.maneuver_size(); ++i) {
		const auto& maneuver = trip_directions.maneuver(i);

		// Depart instruction
		if (maneuver.has_depart_instruction()) {
			valhalla::midgard::logging::Log((boost::format("   %s") % maneuver.depart_instruction()).str(),
				" [NARRATIVE] ");
		}

		// Verbal depart instruction
		if (maneuver.has_verbal_depart_instruction()) {
			valhalla::midgard::logging::Log((boost::format("   VERBAL_DEPART: %s") %
				maneuver.verbal_depart_instruction())
				.str(),
				" [NARRATIVE] ");
		}

		// Instruction
		valhalla::midgard::logging::Log((boost::format("%d: %s | %.1f %s") % m %
			maneuver.text_instruction() % maneuver.length() % units)
			.str(),
			" [NARRATIVE] ");

		// Turn lanes
		// Only for driving and no start/end maneuvers
		if ((maneuver.travel_mode() == valhalla::DirectionsLeg_TravelMode_kDrive) &&
			!((maneuver.type() == valhalla::DirectionsLeg_Maneuver_Type_kStart) ||
			(maneuver.type() == valhalla::DirectionsLeg_Maneuver_Type_kStartRight) ||
				(maneuver.type() == valhalla::DirectionsLeg_Maneuver_Type_kStartLeft) ||
				(maneuver.type() == valhalla::DirectionsLeg_Maneuver_Type_kDestination) ||
				(maneuver.type() == valhalla::DirectionsLeg_Maneuver_Type_kDestinationRight) ||
				(maneuver.type() == valhalla::DirectionsLeg_Maneuver_Type_kDestinationLeft))) {
			auto prev_edge = etl.GetPrevEdge(maneuver.begin_path_index());
			if (prev_edge && (prev_edge->turn_lanes_size() > 0)) {
				std::string turn_lane_status = "ACTIVE_TURN_LANES";
				if (prev_edge->HasNonDirectionalTurnLane()) {
					turn_lane_status = "NON_DIRECTIONAL_TURN_LANES";
				}
				else if (!prev_edge->HasActiveTurnLane()) {
					turn_lane_status = "NO_ACTIVE_TURN_LANES";
				}
				valhalla::midgard::logging::Log((boost::format("   %d: TURN_LANES: %s %s") % m %
					prev_edge->TurnLanesToString(prev_edge->turn_lanes()) %
					turn_lane_status)
					.str(),
					" [NARRATIVE] ");
			}
		}

		// Verbal transition alert instruction
		if (maneuver.has_verbal_transition_alert_instruction()) {
			valhalla::midgard::logging::Log((boost::format("   VERBAL_ALERT: %s") %
				maneuver.verbal_transition_alert_instruction())
				.str(),
				" [NARRATIVE] ");
		}

		// Verbal pre transition instruction
		if (maneuver.has_verbal_pre_transition_instruction()) {
			valhalla::midgard::logging::Log((boost::format("   VERBAL_PRE: %s") %
				maneuver.verbal_pre_transition_instruction())
				.str(),
				" [NARRATIVE] ");
		}

		// Verbal post transition instruction
		if (maneuver.has_verbal_post_transition_instruction()) {
			valhalla::midgard::logging::Log((boost::format("   VERBAL_POST: %s") %
				maneuver.verbal_post_transition_instruction())
				.str(),
				" [NARRATIVE] ");
		}

		// Arrive instruction
		if (maneuver.has_arrive_instruction()) {
			valhalla::midgard::logging::Log((boost::format("   %s") % maneuver.arrive_instruction()).str(),
				" [NARRATIVE] ");
		}

		// Verbal arrive instruction
		if (maneuver.has_verbal_arrive_instruction()) {
			valhalla::midgard::logging::Log((boost::format("   VERBAL_ARRIVE: %s") %
				maneuver.verbal_arrive_instruction())
				.str(),
				" [NARRATIVE] ");
		}

		if (i < trip_directions.maneuver_size() - 1) {
			valhalla::midgard::logging::Log("----------------------------------------------",
				" [NARRATIVE] ");
		}

		// Increment maneuver number
		++m;
	}
	valhalla::midgard::logging::Log("==============================================", " [NARRATIVE] ");
	valhalla::midgard::logging::Log("Total time: " + GetFormattedTime(trip_directions.summary().time()),
		" [NARRATIVE] ");
	valhalla::midgard::logging::Log((boost::format("Total length: %.1f %s") %
		trip_directions.summary().length() % units)
		.str(),
		" [NARRATIVE] ");
	if (origin.date_time_) {
		valhalla::midgard::logging::Log("Departed at: " + *origin.date_time_, " [NARRATIVE] ");
	}
	if (destination.date_time_) {
		valhalla::midgard::logging::Log("Arrived at: " + *destination.date_time_, " [NARRATIVE] ");
	}
	/*data.setTripTime(trip_directions.summary().time());
	data.setTripDist(trip_directions.summary().length());
	data.setManuevers(trip_directions.maneuver_size());*/
#endif
	return trip_directions;
}

#if 1

static int process_trip(const valhalla::DirectionsLeg &trip_directions, PRouteO *rO, PLegO *rLeg)
{
	//----------------------------------------------------------------------------------------------

	//int points_size = 0;
	/*std::vector<carto::MapPos> epsg3857Points;
	carto::EPSG3857 epsg3857;*/

	std::vector<valhalla::midgard::PointLL> shape = valhalla::midgard::decode<std::vector<PointLL> >(trip_directions.shape());
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
	for (int i = 0; i < trip_directions.maneuver_size(); i++) {
		const /*valhalla::odin::TripDirections_Maneuver*/auto& maneuver = trip_directions.maneuver(i);

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

	//----------------------------------------------------------------------------------------------
	//MKR_LOGI("getRoute_6_1 RETURN");
	return EXIT_SUCCESS;
}
#endif
static int __main_run_route(const std::shared_ptr<carto::RoutingRequest>& r_request, std::shared_ptr<carto::RoutingResult>& result)
//static int __main_run_route()
{
	int rv = -1;

	bool multi_run = false;
	bool match_test = false;
	uint32_t iterations=0;

	//std::vector<Location> locs;
	/*for (auto &p : r_points)
	{
		valhalla::baldr::Location loc({ p.getX(),p.getY() });
		locs.push_back(loc);
	}*/
	//auto& pnts = r_request->getPoints();
	
#if 0
	// Grab the directions options, if they exist
	valhalla::Api request;
	
	
	std::string json;
	//json = "{\"locations\":[{\"lat\":43.8499,\"lon\":-79.50193,\"type\":\"break\"},{\"lat\":43.7603,\"lon\":-79.3537,\"type\":\"break\"}],\"costing\":\"auto\",\"directions_options\":{\"units\":\"miles\"}}";
	//std::string json = "{\"costing\":\"auto\"}"; // "{\"costing\":\"auto\",\"directions_options\":{\"units\":\"miles\"}}";
	float lat= 43.8499, lon= -79.50193, lat1= 43.7603, lon1= -79.3537;
	//float lat = pnts[0].getY(), lon=pnts[0].getX(), lat1=pnts[1].getY(),lon1= pnts[1].getX();
	{
		char buf[1024] = { 0 };
		//sprintf(buf, "{\"locations\":[{\"lat\":%f,\"lon\":%f,\"type\":\"break\"},{\"lat\":%f,\"lon\":%f,\"type\":\"break\"}],\"costing\":\"auto\"}",lat,lon,lat1,lon1);
		//{"locations":[{"lat":43.849899, "lon" : -79.501930, "type" : "break"}, { "lat":43.760300,"lon" : -79.353699,"type" : "break" }], "costing" : "auto"}
		sprintf(buf, "{\"locations\":[{\"lat\":43.849899, \"lon\":-79.501930, \"type\":\"break\"}, {\"lat\":43.760300,\"lon\":-79.353699,\"type\":\"break\" }],\"costing\":\"auto\"}");
		json = std::string(buf);
	}
	//json = "{\"locations\":[{\"lat\":43.849899, \"lon\" : -79.501930, \"type\" : \"break\"}, { \"lat\":43.760300,\"lon\" : -79.353699,\"type\" : \"break\" }], \"costing\" : \"auto\"}";
	MKR_LOGI("json=%s", json.c_str());

	valhalla::ParseApi(json, valhalla::Options::route, request);
	const auto& options = request.options();

#else
	valhalla::Api request;
	valhalla::ParseApi("{}", valhalla::Options::route, request);
	valhalla::Options* pOpt = request.mutable_options();
   
	std::vector</*carto::MapPos*/valhalla::LatLng>& pnts = (std::vector</*carto::MapPos*/valhalla::LatLng>&) r_request->getPoints();

	for (auto &p : pnts)
	{
		
		valhalla::Location *pLoc =pOpt->add_locations();
		valhalla::LatLng *pLL = new valhalla::LatLng();
		pLL->set_lat(p.lat());
		pLL->set_lng(p.lng());
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
#endif


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
#elif defined ANDROID || defined (__APPLE__)
		std::string config = std::string((const char*)android_valhalla_bytes, android_valhalla_len);
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

	//MKR_LOGI_LL("getRoute_2: %f %f -- %f %f ", pnts[0].getY(), pnts[0].getX(), pnts[1].getY(), pnts[1].getX());
	// Find path locations (loki) for sources and targets
	
	loki_worker_t lw(pt);
	lw.route(request);
	
	//LOG_INFO("getRoute_3: ");
	// Get the route
	AStarPathAlgorithm astar;
	BidirectionalAStar bd;
	MultiModalPathAlgorithm mm;
	TimeDepForward timedep_forward;
	TimeDepReverse timedep_reverse;
	for (uint32_t i = 0; i < n; i++) {
		// Set origin and destination for this segment
		valhalla::Location origin = options.locations(i);
		valhalla::Location dest = options.locations(i + 1);

		PointLL ll1(origin.ll().lng(), origin.ll().lat());
		PointLL ll2(dest.ll().lng(), dest.ll().lat());

		// Choose path algorithm
		PathAlgorithm* pathalgorithm;
		if (routetype == "multimodal") {
			pathalgorithm = &mm;
		}
		else {
			// Use time dependent algorithms if date time is present
			// TODO - this isn't really correct for multipoint routes but should allow
			// simple testing.
			if (options.has_date_time() && ll1.Distance(ll2) < max_timedep_distance &&
				(options.date_time_type() == valhalla::Options_DateTimeType_depart_at ||
					options.date_time_type() == valhalla::Options_DateTimeType_current)) {
				pathalgorithm = &timedep_forward;
			}
			else if (options.date_time_type() == valhalla::Options_DateTimeType_arrive_by &&
				ll1.Distance(ll2) < max_timedep_distance) {
				pathalgorithm = &timedep_reverse;
			}
			else {
				// Use bidirectional except for trivial cases (same edge or connected edges)
				pathalgorithm = &bd;
				for (auto& edge1 : origin.path_edges()) {
					for (auto& edge2 : dest.path_edges()) {
						if (edge1.graph_id() == edge2.graph_id() ||
							reader.AreEdgesConnected(GraphId(edge1.graph_id()), GraphId(edge2.graph_id()))) {
							pathalgorithm = &astar;
						}
					}
				}
			}
		}
		bool using_astar = (pathalgorithm == &astar || pathalgorithm == &timedep_forward ||
			pathalgorithm == &timedep_reverse);
		bool using_bd = pathalgorithm == &bd;

		// Get the best path
		const valhalla::TripLeg* trip_path = nullptr;
		try {
			trip_path = PathTest(reader, origin, dest, pathalgorithm, mode_costing, mode, NULL, multi_run,
				iterations, using_astar, using_bd, match_test, routetype, request);
		}
		catch (std::runtime_error& rte) { LOG_ERROR("trip_path not found"); }
		//LOG_INFO("getRoute_4: ");
		std::shared_ptr<PRouteObj> rObj = std::make_shared<PRouteObj>();
		// If successful get directions
		if (trip_path && trip_path->node_size() != 0) {
			PRouteO *rO = rObj->add_routes();
			PLegO *rLeg = rO->add_legs();  // in most cases there could be ONLY one route leg
			//LOG_INFO("getRoute_5: ");
			// Try the the directions
			const auto& trip_directions = DirectionsTest(request, origin, dest, NULL);
			//LOG_INFO("getRoute_6: ");
			rv=process_trip(trip_directions, rO, rLeg); 
			//LOG_INFO("getRoute_7: ");
			auto trip_time = trip_directions.summary().time();
			auto trip_length = trip_directions.summary().length() * (options.units()== valhalla::Options_Units::Options_Units_kilometers?1000.0f : 1609.344f);
			
			//LOG_INFO("trip_time (secs)::" + std::to_string(trip_time));
			//LOG_INFO("trip_length (meters)::" + std::to_string(trip_length));
			/*data.setSuccess("success");*/

			// make sure all required fields 

			int steps_size = rLeg->steps_size();
			std::string fromStr = rLeg->mutable_steps(0)->name();
			std::string toStr = rLeg->mutable_steps(steps_size-2)->name();
			std::string summary = std::string("From: ")+fromStr + std::string("\r\nTo: ") + toStr;
			rLeg->set_summary(/*"Summary:"*/summary);
			rLeg->set_distance(trip_time);
			rLeg->set_duration(trip_length);

			std::string rStr = "Route dist:" + std::to_string(trip_length) + " time:" + std::to_string(trip_time);
			rObj->set_code(rStr);
			rO->set_distance(trip_length);
			rO->set_duration(trip_time);

			//MKR_LOGI("getRoute_6_3-> Success");
			try {
				std::shared_ptr<carto::Projection> proj = r_request->getProjection();
				result = std::make_shared<carto::RoutingResult>(proj, rObj);
				rObj->set_code("1");
				rv = EXIT_SUCCESS;
			}
			catch (/*carto::NullArgumentException &e*/...) {

				rv = -7;
			}
		}
		else {
			// Route was unsuccessful
			/*data.setSuccess("fail_no_route");*/
			rv = -6;
		}
	}

	// Set the arc distance. Convert to miles if needed
	if (options.units() == valhalla::Options::miles) {
		d1 *= kMilePerKm;
	}
	/*data.setArcDist(d1);*/

	
	return rv;
}

//***************************************************
#if 1
float CalculateTurnAngle(const std::vector<carto::MapPos>& epsg3857Points, int pointIndex) {
	int pointIndex0 = pointIndex;
	while (--pointIndex0 >= 0) {
		if (epsg3857Points.at(pointIndex0) != epsg3857Points.at(pointIndex)) {
			break;
		}
	}
	int pointIndex1 = pointIndex;
	while (++pointIndex1 < static_cast<int>(epsg3857Points.size())) {
		if (epsg3857Points.at(pointIndex1) != epsg3857Points.at(pointIndex)) {
			break;
		}
	}

	float turnAngle = 0;
	if (pointIndex0 >= 0 && pointIndex1 < static_cast<int>(epsg3857Points.size())) {
		const carto::MapPos& p0 = epsg3857Points.at(pointIndex0);
		const carto::MapPos& p1 = epsg3857Points.at(pointIndex);
		const carto::MapPos& p2 = epsg3857Points.at(pointIndex1);
		carto::MapVec v1 = p1 - p0;
		carto::MapVec v2 = p2 - p1;
		double dot = v1.dotProduct(v2) / v1.length() / v2.length();
		turnAngle = static_cast<float>(std::acos(std::max(-1.0, std::min(1.0, dot))) * carto::Const::RAD_TO_DEG);
	}
	return turnAngle;
}

const std::unordered_map<int, carto::RoutingAction::RoutingAction> maneuver_types = {
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kNone),             carto::RoutingAction::ROUTING_ACTION_NO_TURN },//NoTurn = 0,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kContinue),         carto::RoutingAction::ROUTING_ACTION_GO_STRAIGHT },//GoStraight,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kBecomes),          carto::RoutingAction::ROUTING_ACTION_GO_STRAIGHT },//GoStraight,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kRampStraight),     carto::RoutingAction::ROUTING_ACTION_GO_STRAIGHT },//GoStraight,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kStayStraight),     carto::RoutingAction::ROUTING_ACTION_GO_STRAIGHT },//GoStraight,
	{ static_cast<int>(valhalla::DirectionsLeg_Maneuver_Type_kMerge),            carto::RoutingAction::ROUTING_ACTION_MERGE },//Merge,
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
#endif
//===================================================

int main_run_route(const std::shared_ptr<carto::RoutingRequest>& r_request, std::shared_ptr<carto::RoutingResult>& result)
{
	int rv = 0;
	try {
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

#ifdef WIN32
void dumpRoute(PRouteObj *_rObj)
{
	int size = _rObj->routes_size();
	if (size == 0 )
		return;
	for (int i = 0; i < size; i++)
	{
		PRouteO *rO= _rObj->mutable_routes(i);
		int lSise = rO->legs_size();
		for (int j = 0; j < lSise; j++)
		{
			PLegO *leg = rO->mutable_legs(j);
			if (leg)
			{
				int gpSize=leg->geo_points_size();
				for (int k = 0; k < gpSize; k++)
				{
					PPointLL *p = leg->mutable_geo_points(k);
					carto::MapPos pos = carto::MapPos(p->lon(), p->lat());
				}

				int sSize = leg->steps_size();
				for (int n = 0; n < sSize; n++)
				{
					PStepO *step= leg->mutable_steps(n);
					PManeuverO* mn = step->mutable_mnv();
					std::string *instr = mn->mutable_textinstr();
					printf("instr %s \r\n", instr->c_str());
				}

			}

		}
	}

	
}
#endif

bool createRoute(float sLat,float sLon,float destLat,float destLon,std::string& resPtr)
{
	PRouteObj *resV=NULL ;

	std::vector</*carto::MapPos*/valhalla::LatLng> v;

	//carto::MapPos p0(sLon, sLat);
	valhalla::LatLng p0;
	p0.set_lat(sLat); p0.set_lng(sLon);
	v.push_back(p0);

	//carto::MapPos p1(destLon, destLat);
	valhalla::LatLng p1;
	p1.set_lat(destLat); p1.set_lng(destLon);
	v.push_back(p1);


	std::shared_ptr<carto::EPSG3857> proj = std::make_shared<carto::EPSG3857>();
	std::shared_ptr<carto::RoutingRequest> request = std::make_shared<carto::RoutingRequest>(proj, v);
	std::shared_ptr<carto::RoutingResult> result = nullptr;

	int rv = main_run_route(request, result);
	//m_printf(" createRoute_0.. rv=%d",rv);
	if (rv == 0 && result != nullptr)
	{
		//m_printf(" createRoute_0..0");
		resV = result->getRouteObj().get();
		if (resV->code() == "1")
		{

			//this->r_route = result;
			//if (voiceRouter)voiceRouter->setRoute(result); // reference has been increased two times !!!
			//m_printf(" createRoute_OK..\r\n");
			
#ifdef WIN32
			dumpRoute(resV);
#endif
		}

	}
	else {
		//this->r_route = nullptr;   // !!!!!!!
		resV = new PRouteObj();   // create object just to pass error code to java thru jni   ( delete it in jni function)
		std::string code = std::to_string(rv);
		resV->set_code(code);
		m_printf(" createRoute_ERR..%d code=%s\r\n", rv,code.c_str());

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
int main_run_route(int argc, char *argv[])
{
	std::string resPtr;
	//float sLat= 43.8499,  sLon= -79.50193,  destLat= 43.7603,  destLon= -79.3537;

	//var rReq = "43.849899" + "," + "-79.501930" + "," + "43.760300" + "," + "-79.353699";
	float sLat = 43.849899, sLon = -79.501930, destLat = 43.760300, destLon = -79.353699;

	bool rv = createRoute(sLat, sLon, destLat, destLon, resPtr);
	if(rv)
	{
		printf("res size=%d\r\n", resPtr.size());
	}
	return 0;
}
#endif
#endif
