#ifdef _CARTO_ROUTING_SUPPORT

#include "RoutingResult.h"
//#include "components/Exceptions.h"

#include <numeric>
#include <functional>
#include <utility>
#include <iomanip>
#include <sstream>

#include "MapUtils.h"

namespace carto {

    RoutingResult::RoutingResult(const std::shared_ptr<Projection>& projection,/* const std::vector<MapPos>& points,*/ const std::shared_ptr<PRouteObj> rObj) :
        _projection(projection),
        //_points(points),
        _rObj(rObj)
    {
		
		PLegO *pO = getLegO(0);
		
		int geo_points_size = pO==NULL?0:pO->geo_points_size();
		
        if (!projection ) {
           // throw NullArgumentException("RoutingResult exc");
        } 
		//MKR_LOGI("RoutingResult..0");
		//update listDistance
		_listDistance.resize(geo_points_size);
		_listDistance[geo_points_size - 1]= 0;
	
		for (int i = geo_points_size - 1; i > 0; i--) {
			PPointLL *pp1=getRoutePointAt(i-1);
			PPointLL *pp2= getRoutePointAt(i);
			carto::MapPos p1(pp1->lon(), pp1->lat());
			carto::MapPos p2(pp2->lon(), pp2->lat());
			float dist = MapUtils::distance(p1,p2);
			/*_listDistance[i - 1] = dist;
			_listDistance[i - 1] += _listDistance[i];
			*/
			dist += _listDistance[i];
			_listDistance[i - 1] = dist;
		}
    }

    RoutingResult::~RoutingResult() {
		_listDistance.clear();
		//_instructions.clear();
		//_points.clear();
    }

    const std::shared_ptr<Projection>& RoutingResult::getProjection() const {
        return _projection;
    }


    double RoutingResult::getTotalDistance()  {
        /*return std::accumulate(_instructions.begin(), _instructions.end(), 0.0, [](double time, const RoutingInstruction& instruction) {
            return time + instruction.getDistance();
        });*/
		return this->getRouteO(0)->distance();
    }
    
    double RoutingResult::getTotalTime()  {
       /* return std::accumulate(_instructions.begin(), _instructions.end(), 0.0, [](double time, const RoutingInstruction& instruction) {
            return time + instruction.getTime();
        });*/
		return this->getRouteO(0)->duration();
    }

    std::string RoutingResult::toString() const {
        std::stringstream ss;
       /* ss << std::setiosflags(std::ios::fixed);
        ss << "RoutingResult [";
        ss << "instructions=" << _instructions.size() << ", ";
        ss << "totalDistance=" << getTotalDistance() << ", ";
        ss << "totalTime=" << getTotalTime();
        ss << "]";*/
        return ss.str();
    }

	//extention
	void RoutingResult::updateCurrentRoute(int currentRoute) {
		this->currentRoute = currentRoute;
		PRouteO *ro = getRouteO(0);
		PLegO *leg = getLegO(0);
		int instructions_size = leg->steps_size();
		PStepO *step = nullptr;
		while (currentDirectionInfo < instructions_size - 1 && 
			(step = getStepOAt(currentDirectionInfo + 1))!=nullptr &&
			step->/*pointindex*/point_start_index() < currentRoute)
		{
			currentDirectionInfo++;
		}
		
	}

	//NextDirectionInfo getNextRouteDirectionInfo(NextDirectionInfo info, Location fromLoc, boolean toSpeak) 
	PStepO* RoutingResult::getNextRouteDirectionInfo(carto::MapPos &fromLoc, float &Dist, int &index)
	{
		PStepO *rv = nullptr;
#if 1
		int dirInfo = currentDirectionInfo;
		PRouteO *ro = getRouteO(0);
		PLegO *leg = getLegO(0);
		int instructions_size = ro->mutable_legs(0)->steps_size();
		if (dirInfo < instructions_size) {
			float dist = _listDistance[currentRoute];
			int nextInd = dirInfo + 1;
			
			auto pp = leg->mutable_geo_points(currentRoute);
			carto::MapPos p2(pp->lon(), pp->lat());
			dist += MapUtils::distance(fromLoc,p2);
			
			if (nextInd < instructions_size) {
				rv = leg->mutable_steps(nextInd);
				dist -= _listDistance[rv->/*pointindex*/point_start_index()];
				Dist=dist;
				index=nextInd;
				
			}
		}
		return rv;
#endif		
	}
	/*void RoutingResult::getNextRouteDirectionInfoAfter(PStepO &prev, PStepO &next, bool toSpeak)
	{
		next.processed = false;
		int dirInfo = prev.get_directionInfoInd();
		if (dirInfo < _instructions.size() && prev.processed != false) {
			int dist = _listDistance[prev.getPointIndex()];
			int nextInd = dirInfo + 1;
			
			if (nextInd < _instructions.size()) {
				next = _instructions.at(nextInd);
				dist -= _listDistance[_instructions.at(nextInd).getPointIndex()];
				next.setDistanceTo(dist);
				next.set_directionInfoInd(nextInd);
				next.processed = true;
			}
			
		}
		
	}*/


}

#endif
