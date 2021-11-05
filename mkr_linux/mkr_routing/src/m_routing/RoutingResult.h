/*
 * Copyright (c) 2016 CartoDB. All rights reserved.
 * Copying and using this code is allowed only according
 * to license terms, as given in https://cartodb.com/terms/
 */

#ifndef _CARTO_ROUTINGRESULT_H_
#define _CARTO_ROUTINGRESULT_H_

#ifdef _CARTO_ROUTING_SUPPORT

#include "core/MapPos.h"
#include <memory>
#include <vector>

#include "models/udorx_comm_lite.pb.h"
//#include "Dbg.h"

using namespace std;
using namespace udorx_comm_lite;


namespace carto {
    class Projection;

    /**
     * A class that defines list of routing actions and path geometry.
     */
    class RoutingResult {
    public:
        /**
         * Constructs a new RoutingResult instance from projection, points and instructions.
         * @param projection The projection of the routing result (same as the request).
         * @param points The point list defining the routing path. Instructions refer to this list.
         * @param instructions The turn-by-turn instruction list.
         */
        RoutingResult(const std::shared_ptr<Projection>& projection, /*const std::vector<MapPos>& points,*/ const std::shared_ptr<PRouteObj> rObj);
        virtual ~RoutingResult();

        /**
         * Returns the projection of the points in the result.
         * @return The projection of the result.
         */
        const std::shared_ptr<Projection>& getProjection() const;
       
        /**
         * Returns the total distance of the path.
         * @return The total distance in meters.
         */
        double getTotalDistance() ;
        /**
         * Returns the approximate total duration of the path.
         * @return The total duration in seconds.
         */
        double getTotalTime() ;

        /**
         * Creates a string representation of this routing request result, useful for logging.
         * @return The string representation of this routing result object.
         */
        std::string toString() const;
        
		//===============extended addition-------------
		int currentRoute = 0;  // public

		void updateCurrentRoute(int currentRoute);
		PStepO* getNextRouteDirectionInfo( carto::MapPos &fromLoc, float &dist, int &index);
		//void getNextRouteDirectionInfoAfter(PStepO &prev, PStepO &next, bool toSpeak);
		bool getNextRouteLocation(carto::MapPos& pos) {
			bool rv = false;
			
			/*PRouteO *ro = getRouteO(0);
			int p_size=ro->geo_points_size();
			if (currentRoute < p_size) {
				auto pp = ro->geo_points(currentRoute);
				pos= carto::MapPos(pp.lon(),pp.lat());
				rv = true;
			}*/

			PLegO *leg=getLegO(0);
			int p_size = leg?leg->geo_points_size():0;
			if (currentRoute < p_size) {
				auto pp = leg->geo_points(currentRoute);
				pos = carto::MapPos(pp.lon(), pp.lat());
				rv = true;
			}
			return rv;
		}

		std::shared_ptr<PRouteObj> &getRouteObj() { return _rObj; }
		PRouteO *getRouteO(int ind) 
		{ 
			int size = _rObj->routes_size();
			if (size == 0 || ind > (size - 1))
				return NULL;
			return _rObj->mutable_routes(ind); 
		};
		PLegO *getLegO(int ind) 
		{ 
			PRouteO *r=getRouteO(0);
			return r->mutable_legs(ind); 
		};
		PStepO *getStepOAt(int i)
		{
			PLegO *leg=getLegO(0);
			if (leg)
				return leg->mutable_steps(i);
			else
				return nullptr;
		}
		PPointLL *getRoutePointAt(int i)
		{
			/*PRouteO *r=getRouteO(0);
			if (r)
				return r->mutable_geo_points(i);*/
			PLegO *leg = getLegO(0);
			if (leg)
				return leg->mutable_geo_points(i);
			else
				return nullptr;
		}
		bool getRoutePointAt(int ind,carto::MapPos &pos)
		{
			bool rv = false;
			PLegO *leg = getLegO(0);
			if (leg)
			{
				PPointLL *p= leg->mutable_geo_points(ind);
				pos = carto::MapPos(p->lon(), p->lat());
			}
			return rv;
		}
		
		bool getFinalRoutePoint(carto::MapPos &pos)
		{
			bool rv = false;
			PLegO *leg = getLegO(0);
			if (leg)
			{
				PPointLL *p = leg->mutable_geo_points(leg->geo_points_size()-1);
				pos = carto::MapPos(p->lon(), p->lat());
			}
			return rv;
		}


		int getStepsSize() {
			int rv = 0;
			PLegO *leg=getLegO(0);
			if (leg)
				rv = leg->steps_size();
			return rv;
		}
    private:
        std::shared_ptr<Projection> _projection;
       // std::vector<MapPos> _points;
		std::shared_ptr<PRouteObj> _rObj;

		//extention
		int currentDirectionInfo = 0;
		std::vector<float> _listDistance;
    };
    

	namespace RoutingAction {
		/**
		* Routing action type.
		*/
		enum RoutingAction {
			/**
			* Head on, start the route.
			*/
			ROUTING_ACTION_HEAD_ON,
			/**
			* Finish the route.
			*/
			ROUTING_ACTION_FINISH,
			/**
			* Continue along the given street, do not turn.
			*/
			ROUTING_ACTION_NO_TURN,
			/**
			* Go straight.
			*/
			ROUTING_ACTION_GO_STRAIGHT,
			/**
			* Turn right.
			*/
			ROUTING_ACTION_TURN_RIGHT,
			ROUTING_ACTION_TURN_SLIGHT_RIGHT,  // svk added
			ROUTING_ACTION_TURN_SHARP_RIGHT,
			ROUTING_ACTION_KEEP_RIGHT,
			/**
			* Do an u-turn.
			*/
			ROUTING_ACTION_UTURN,   //svk left
			ROUTING_ACTION_UTURN_RIGHT,   //svk left
										  /**
										  * Turn left.
										  */
			ROUTING_ACTION_TURN_LEFT,
			ROUTING_ACTION_TURN_SLIGHT_LEFT, //svk added
			ROUTING_ACTION_TURN_SHARP_LEFT,
			ROUTING_ACTION_KEEP_LEFT,
			/**
			* Reached given via point. If this is the final point, FINISH action is used instead.
			*/
			ROUTING_ACTION_REACH_VIA_LOCATION,
			/**
			* Enter roundabout.
			*/
			ROUTING_ACTION_ENTER_ROUNDABOUT,
			/**
			* Leave roundabout.
			*/
			ROUTING_ACTION_LEAVE_ROUNDABOUT,
			/**
			* Continue along the roundabout.
			*/
			ROUTING_ACTION_STAY_ON_ROUNDABOUT,
			/**
			* Start at the end of the street.
			*/
			ROUTING_ACTION_START_AT_END_OF_STREET,
			/**
			* Enter street while moving against the allowed direction.
			*/
			ROUTING_ACTION_ENTER_AGAINST_ALLOWED_DIRECTION,
			/**
			* Leave the street while moving aginst the allowed direction.
			*/
			ROUTING_ACTION_LEAVE_AGAINST_ALLOWED_DIRECTION,

			ROUTING_ACTION_MERGE
		};

	}


}

#endif

#endif
