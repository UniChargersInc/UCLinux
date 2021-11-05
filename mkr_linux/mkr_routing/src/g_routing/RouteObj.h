#pragma once
#include <string>
#include <cstdarg>
#include <sstream>
#include <vector>
#include <stdio.h>

namespace mkr {
	namespace routing {

		struct PointLL {
			float lat;
			float lon;

			PointLL() {}
			PointLL(float lat, float lon)
			{
				this->lat = lat;
				this->lon = lon;
			}
		};

		struct WayPointO
		{
			std::string name;
			PointLL wpLoc;

		};
		struct ManeuverO
		{
			std::string type, modifier;
			PointLL mnLoc;
		};
		struct StepO
		{
			std::vector<PointLL> geo_points;
			float distance;
			std::string name;
			ManeuverO mnv;
			~StepO()
			{
				geo_points.clear();
			}
		};
		struct LegO
		{

			float distance;
			float duration;
			std::string summary;
			std::vector<StepO> steps;
			~LegO()
			{
				steps.clear();
			}
		};
		struct RouteO
		{

			float distance;
			float duration;
			std::vector<LegO> legs;
			~RouteO()
			{
				legs.clear();
			}
		};
		struct RouteObj
		{
			std::string code;
			std::vector<WayPointO> waypoints;
			std::vector<RouteO> routes;

			void cleanUp()
			{
				code = "";
				waypoints.clear();
				routes.clear();
			}
			RouteObj() {}
			~RouteObj()
			{
				cleanUp();
			}
		};

	} // namespace routing
} // namespace mkr

