#include "route_manager.hpp"


#include <cstdlib>
#include <ctime>
#include <cassert>
#include <memory>
#include <list>
#include <tuple>

#include <math.h>

#include "native_map_view.hpp"
#ifdef WIN32
#include "nav_native_manager_win.hpp"
#else
#include "nav_native_manager.hpp"
#endif
#include "vectorelements/Marker.h"


#include <valhalla/midgard/pointll.h>
#include <valhalla/midgard/constants.h>

#ifdef WIN32
#include "LogFile/CartoMkrLog.h"
void setTitle(std::string str);
#endif
#if defined (__APPLE__)
#define FLT_MAX 345678901.0
#endif
#include "MapUtils.h"

#include <thread>


#define POSITION_TOLERANCE	60


int main_run_route(const std::shared_ptr<carto::RoutingRequest>& request, std::shared_ptr<carto::RoutingResult>& result);

namespace cartodb {
namespace routing {

	

	
RouteManager::RouteManager(mapview::NativeMapView *_nativeMapView)
    : nativeMapView(_nativeMapView) {
   
	voiceRouter = new VoiceRouter(this);
	
}

RouteManager::~RouteManager() {
    
	if (voiceRouter)
		delete voiceRouter;
	voiceRouter = 0;
    


}

//
//void RouteManager::createRoutePointTitleDesc(carto::RoutingInstruction &instr,std::string &title,std::string &desc, std::string &imgN)
//{
//	carto::RoutingAction::RoutingAction action = instr.getAction();
//	imgN = "direction_up.png";
//		switch (action) {
//		case carto::RoutingAction::RoutingAction::ROUTING_ACTION_HEAD_ON:
//			title = "head on";
//				break;
//		case carto::RoutingAction::RoutingAction::ROUTING_ACTION_FINISH:
//			title = "finish";
//				break;
//		case carto::RoutingAction::RoutingAction::ROUTING_ACTION_TURN_LEFT:
//			imgN = "direction_upthenleft.png";
//			title = "turn left";
//				break;
//		case carto::RoutingAction::RoutingAction::ROUTING_ACTION_TURN_RIGHT:
//			imgN = "direction_upthenright.png";
//			title = "turn right";
//				break;
//		case carto::RoutingAction::RoutingAction::ROUTING_ACTION_UTURN:
//			title = "u turn";
//				break;
//		case carto::RoutingAction::RoutingAction::ROUTING_ACTION_NO_TURN:
//		case carto::RoutingAction::RoutingAction::ROUTING_ACTION_GO_STRAIGHT:
//			title = "continue";
//				break;
//		case carto::RoutingAction::RoutingAction::ROUTING_ACTION_REACH_VIA_LOCATION:
//			title = "stopover";
//				break;
//		case carto::RoutingAction::RoutingAction::ROUTING_ACTION_ENTER_AGAINST_ALLOWED_DIRECTION:
//			title = "enter against allowed direction";
//				break;
//		case carto::RoutingAction::RoutingAction::ROUTING_ACTION_LEAVE_AGAINST_ALLOWED_DIRECTION:
//			break;
//		case carto::RoutingAction::RoutingAction::ROUTING_ACTION_ENTER_ROUNDABOUT:
//			title = "enter roundabout";
//				break;
//		case carto::RoutingAction::RoutingAction::ROUTING_ACTION_STAY_ON_ROUNDABOUT:
//			title = "stay on roundabout";
//				break;
//		case carto::RoutingAction::RoutingAction::ROUTING_ACTION_LEAVE_ROUNDABOUT:
//			title = "leave roundabout";
//				break;
//		case carto::RoutingAction::RoutingAction::ROUTING_ACTION_START_AT_END_OF_STREET:
//			title = "start at end of street";
//				break;
//		}
//
//		char buf[256] = { 0 };
//		sprintf(buf, "%s \r\nazimuth:  %.1f deg\r\ndistance : %.0f m\r\ntime : %.0f sec\r\nTurnAngle : %.0f deg", instr.getStreetName().c_str(), instr.getAzimuth(), instr.getDistance(), instr.getTime(), instr.getTurnAngle());
//	
//		desc = std::string(buf);
//
//	// filter out some instructions which would be too noisy for the map
//	//if ([str isEqualToString : @"continue"] or [str isEqualToString : @"enter roundabout"]){
//	//	return nil;
//
//}

void RouteManager::recalculateRouteInBackground()
{
	std::lock_guard<std::mutex> lock(r_mutex);

	carto::MapPos p0(lastFixedLocation.lon, lastFixedLocation.lat);
	carto::MapPos finalLocation;
	bool rv = r_route->getFinalRoutePoint(finalLocation);


	//MKR_LOGI_LL("recalc stopTBT");
	stopTBT();
	//MKR_LOGI_LL("recalc r_route=0 here");
	
	clearAllRouteAnnotation();  // clear all annotations on the map
	
	std::vector</*carto::MapPos*/valhalla::LatLng> v;
	valhalla::LatLng vLL0;
	vLL0.set_lat(p0.getY());
	vLL0.set_lng(p0.getX());
	v.push_back(vLL0);

	valhalla::LatLng vLL1;
	vLL1.set_lat(finalLocation.getY());
	vLL1.set_lng(finalLocation.getX());

	v.push_back(vLL1);
	PRouteObj* rr=createRoute(v);
	std::string code = rr->code();
	if (code == "1")
	{
		int rv1 = startRoute(true);
	}
	else {
		MKR_LOGI_LL("recalculateRouteInBk_Err %s",code.c_str());
		delete rr;
	}
	
}

// this is a thread function
void RouteManager::do_recalculateRouteInBackground(void* _this)
{
	RouteManager *rm = (RouteManager*)_this;
	//RouteManager *rm = dynamic_cast<RouteManager*> (_this);  //dynamic_cast from void* is illegal - the type casted from must be polymorphic - contain at least one virtual function (virtual destructor counts too).
	if (rm)
	{
		
		rm->recalculateRouteInBackground();
		//MKR_LOGI_LL("rm->recalculateRouteInBackground()...exit");
	}
}


void RouteManager::startTBT(bool from_recalcThread)
{
	int size = r_route->getStepsSize();
	
	if (size > 0)
	{
		
#ifdef WIN32
		std::string dir = "C:\\my\\CartoDB\\map_data/";
#else
		std::string dir = "nav_imgs/";
#endif	
		float rotation = 0;
		PStepO *step=r_route->getStepOAt(0);
		
		carto::MapPos pnt;
		/*PPointLL *pp = */ r_route->getRoutePointAt(step->/*pointindex*/point_start_index(),pnt);
		carto::MapPos mpos0 = r_route->getProjection()->fromWgs84(pnt) , mpos1;
		//MKR_LOGI_LL("startTBT() size:%d  pInd=%d", size, step->/*pointindex*/point_start_index());
		for (int i=1;i<size;i++)
		{
			step = r_route->getStepOAt(i);
			
			r_route->getRoutePointAt(step->/*pointindex*/point_start_index(),pnt);
			carto::MapPos mpos= r_route->getProjection()->fromWgs84(pnt);
			if (i == 1)
				mpos1 = mpos;
			std::string imgN="", title = "", desc = "";
			//createRoutePointTitleDesc(instr, title, desc, imgN);
#ifdef WIN32
			nativeMapView->addPoint(mpos, 0xff0000ff,title,desc, imgN.empty()?"": dir + imgN);
#else
			nativeMapView->addPoint(mpos, 0xff0000ff, title, desc, imgN.empty() ? "" : dir+ imgN);

			//MKR_LOGI_LL("startTBT() lon=%f lat=%f\r\n", mpos.getX(), mpos.getY());
#endif
		}

		
		//setup the navigation icon 
		//1. angle calculation 
		//float tan = (mpos1.getY() - mpos0.getY()) / (mpos1.getX() - mpos0.getX());
		//float angle = atan2((mpos1.getY() - mpos0.getY()), (mpos1.getX() - mpos0.getX()));
		//rotation = -1.0*angle*(180 / M_PI) - 90;
		

		rotation =  getAngle(mpos0, mpos1);

		//MyLocation_T myloc = Global::Get()->getCurrentLocation();
		////MKR_LOGI("startTBT::setPos.. lon=%f lat=%f ", myloc.lon, myloc.lat);
		//carto::MapPos curpos(myloc.lon, myloc.lat);
		//carto::MapPos mpos = nativeMapView->getProjection()->fromWgs84(curpos);

		//MKR_LOGI_LL("addNavMarker  lon=%f lat=%f rot=%f\r\n", mpos0.getX(),mpos0.getY(), rotation);

		navMarker=nativeMapView->addNavMarker(mpos0, rotation);
		//MKR_LOGI_LL("startTBT_1\r\n");
		nativeMapView->setMapRotation(rotation, 1);
		//MKR_LOGI_LL("startTBT_2\r\n");
		if (voiceRouter)voiceRouter->updateRouteInformation(0, rotation, from_recalcThread);
		//MKR_LOGI_LL("startTBT_End\r\n");
	}
	sm_nav_state = TBTNavStates::STARTED;

	//MKR_LOGI("RouteManager::startTBT()_4");
}


float RouteManager::getAngle(carto::MapPos &_pos1, carto::MapPos &pos2)
{
	auto proj=nativeMapView->getProjection();
	
	carto::MapPos pos1 = proj->toWgs84(_pos1);
	pos2 = proj->toWgs84(pos2);

	return MapUtils::Heading(pos1, pos2);

}
void RouteManager::stopTBT()
{
	r_route.reset();//r_route.~shared_ptr();  // that should destroy  std::shared_ptr<carto::RoutingResult> r_route
	r_route = nullptr;
	sm_nav_state = TBTNavStates::NONE;
}

bool RouteManager::isRouteValid()
{
	if (this->r_route == nullptr)
		return false;
	return true;
}

//#include <valhalla\midgard\pointll.h>
void RouteManager::updateLocation()
{
	if (sm_nav_state == TBTNavStates::NONE)
		return;

	MyLocation_T &mloc = Global::Get()->getCurrentLocation();
	
	if (isRouteValid())
	{
		std::lock_guard<std::mutex> lock(r_mutex);
		processStateMachine(mloc);
	}
	
}
void RouteManager::updateNavMarkerOnMap(float lat, float lon, float bearing)
{
	if (nativeMapView != nullptr && navMarker != nullptr)
	{
		//MKR_LOGI("updateLocation..xo=%f yo=%f ", mloc.lon, mloc.lat);
		carto::MapPos curpos(lon, lat);
		carto::MapPos mpos = nativeMapView->getProjection()->fromWgs84(curpos);
		//nativeMapView->setMarkerPos(navMarker, mpos);
		float cog = 360.0f - bearing;
		navMarker->setPosAndRotation(mpos, cog);

		nativeMapView->getMapV()->setFocusPos(mpos, 0);
		nativeMapView->setMapRotation(cog, 1);

	}
}

void RouteManager::showRouteLine()
{
	this->nativeMapView->showRouteLine(r_route);
}

PRouteObj* RouteManager::createRoute(std::vector</*carto::MapPos*/ valhalla::LatLng> &v)
{
	PRouteObj* resV = NULL;
	auto proj = nativeMapView->getMapV()->getOptions()->getBaseProjection();
	std::shared_ptr<carto::RoutingRequest> request = std::make_shared<carto::RoutingRequest>(proj, v);
	std::shared_ptr<carto::RoutingResult> result=nullptr;
	
	int rv = main_run_route(request, result);
	//MKR_LOGI(" createRoute_0.. rv=%d",rv);
	if (rv == 0 && result != nullptr)
	{
		//MKR_LOGI(" createRoute_0..0");
		resV = result->getRouteObj().get();
		if (resV->code() == "1")
		{
			
			this->r_route = result;
			//if (voiceRouter)voiceRouter->setRoute(result); // reference has been increased two times !!!
			//MKR_LOGI_LL(" createRoute_OK..\r\n");
			showRouteLine();
			//	printRoute();  // remove it after test
		}
		
	}
	else {
			this->r_route = nullptr;   // !!!!!!!
			resV = new PRouteObj();   // create object just to pass error code to java thru jni   ( delete it in jni function)
			std::string code = std::to_string(rv);
			resV->set_code(code);
			MKR_LOGI_LL(" createRoute_ERR..%d\r\n",rv);
		
	}
	//MKR_LOGI(" createRoute..OK %s\r\n",resV->code().c_str());
	return resV;
}

int RouteManager::printRoute() // for test
{
#if 0
	std::vector<carto::MapPos> v = r_route->getPoints();
	auto instrs = r_route->getInstructions();

	int index = 0;
	
	for (int i=0;i<v.size();i++)
	{
		
		if (i == 0)
		{
			carto::MapPos p = v.at(i);
			double x = p.getX(), y = p.getY();
			x += 0.005, y += 0.007;
			p.setX(x), p.setY(y);
			float cog=MapUtils::Heading(p, v[0]);
			MKR_LOGI_LL("{%f , %f , %f}, //----ind=%d", p.getX(), p.getY(),cog,index++);

			p = v.at(i);
			MKR_LOGI_LL("{%f , %f , %f}, //----ind=%d", p.getX(), p.getY(), cog,index++);
			continue;
		}
		
		carto::MapPos p0 = v.at(i-1);
		carto::MapPos p1 = v.at(i);
		float cog = MapUtils::Heading(p0, p1);

		
		carto::MapPos px((p0.getX() + p1.getX()) / 2, (p0.getY() + p1.getY()) / 2);  //= p0.MidPoint(p1);
		MKR_LOGI_LL("{%f , %f , %f}, //-----ind=%d", px.getX(), px.getY(),cog,index++);

		MKR_LOGI_LL("{%f , %f , %f}, //-----ind=%d", p1.getX(), p1.getY(), cog, index++);
		
		
	}
#endif
	return 0;
}

int RouteManager::startRoute(bool from_recalcThread )
{
	if (!isRouteValid())
		return 0;
	//MKR_LOGI("RouteManager::startRoute() ");
	if (voiceRouter)voiceRouter->reset();
	startTBT(from_recalcThread);
	return 1;
}
void RouteManager::clearAllRouteAnnotation()
{
	//clear all routing layers
	nativeMapView->clearRouteAnnotationLayer();
}

void RouteManager::processStateMachine(MyLocation_T &mloc)
{

	switch (sm_nav_state)
	{
		case TBTNavStates::NONE:
			break;
		case TBTNavStates::STARTED:
		{
			// the car is on the route line
			//MKR_LOGI_LL(">1");
			bool finished=updateCurrentRouteStatus(mloc);
			if (finished)
				sm_nav_state = TBTNavStates::FINISHED;
			//MKR_LOGI_LL(">2");
			process_routing(mloc);
			//MKR_LOGI_LL(">3");

			return;

		}
		break;  //case TBTNavStates::STARTED:
	} //switch (sm_nav_state)
}

//int zzzz = 0;
void RouteManager::process_routing(MyLocation_T &mloc)
{
	bool calculateRoute = false;
	
	/*zzzz++;
	if (zzzz > 1)
	{
		calculateRoute = true;
	}*/

	//PRouteO *rO = r_route->getRouteO(0);
	//int route_nodes_size = rO->geo_points_size();


	int currentRoute = r_route->currentRoute;
	
	MyLocation_T locationProjection = mloc;
	carto::MapPos currentLocation(mloc.lon, mloc.lat);
	
	// 2. Analyze if we need to recalculate route
	// >120m off current route (sideways)
	if (currentRoute > 0) {
		carto::MapPos p1, p2;
		bool rv1 = r_route->getRoutePointAt(currentRoute - 1, p1);
		bool rv2 = r_route->getRoutePointAt(currentRoute, p2);
		float dist = MapUtils::getOrthogonalDistance(currentLocation,p1,p2);
		if (dist > 2 * POSITION_TOLERANCE) {
			
			calculateRoute = true;
		}
	}
	// 3. Identify wrong movement direction (very similar to 2?)
	carto::MapPos next;
	bool wrongMovementDirection = false;
	bool next_f = r_route->getNextRouteLocation(next);
	if (next_f)
	{
		wrongMovementDirection = checkWrongMovementDirection(mloc, next);
		
		carto::MapPos p1;
		bool rv1 = r_route->getRoutePointAt(currentRoute, p1);
		if (wrongMovementDirection && MapUtils::distance(currentLocation,p1) > 2 * POSITION_TOLERANCE) {
			//log.info("Recalculate route, because wrong movement direction: " + currentLocation.distanceTo(routeNodes.get(currentRoute))); //$NON-NLS-1$
			calculateRoute = true;
		}
	}
	// 4. Identify if UTurn is needed
	bool uTurnIsNeeded = false;// identifyUTurnIsNeeded(currentLocation);
	// 5. Update Voice router
	if (calculateRoute == false || uTurnIsNeeded == true) {
		if(voiceRouter)voiceRouter->updateVoiceRouterStatus(mloc, wrongMovementDirection, uTurnIsNeeded);
	}

	// calculate projection of current location
#if 0   // svk ??? why we need this
	if (currentRoute > 0) {
		double dist = MapUtils::getOrthogonalDistance(currentLocation, routeNodes.at(currentRoute - 1), routeNodes.at(currentRoute));
		double projectDist = /*mode == ApplicationMode.CAR ?*/ POSITION_TOLERANCE /*: POSITION_TOLERANCE / 2*/;
		
		
		if (dist < projectDist) {
			carto::MapPos nextLocation = routeNodes.at(currentRoute);
			carto::MapPos project;
			MapUtils::getProjection(currentLocation, routeNodes.at(currentRoute - 1), routeNodes.at(currentRoute),project);

			locationProjection.lat=project.getY();
			locationProjection.lon=project.getX();
			// we need to update bearing too
			if (locationProjection.hasBearing()) {
				carto::MapPos p(locationProjection.lon, locationProjection.lat);
				if (p != nextLocation)
				{
					float bearingTo = MapUtils::Heading(p, nextLocation);

					locationProjection.setBearing(bearingTo);
				}
			}
		}
	}
#endif
	lastFixedLocation = locationProjection;
		

	if (calculateRoute) {
			if (voiceRouter)voiceRouter->updateRouteInformation("***","Recalculate the route",0,0);

			std::thread t1(do_recalculateRouteInBackground,this);
			//t1.join();// it stops the current thread until recalculation is done !! Not a good solution here
			t1.detach();
			//MKR_LOGI_LL("do_recalculateRouteInBackground...exit");
			return;  // added recently
	}
	
	if (lastFixedLocation.hasBearing())
	{
		updateNavMarkerOnMap(lastFixedLocation.lat, lastFixedLocation.lon, lastFixedLocation.getBearing());
	}
}
/**
	 * Wrong movement direction is considered when between
	 * current location bearing (determines by 2 last fixed position or provided)
	 * and bearing from currentLocation to next (current) point
	 * the difference is more than 90 degrees
	 */
bool RouteManager::checkWrongMovementDirection(MyLocation_T &currentLocation, carto::MapPos & nextRouteLocation) {
	if ((currentLocation.hasBearing() || lastFixedLocation.hasBearing()) ) {
		float bearingMotion = currentLocation.hasBearing() ? currentLocation.getBearing() : MapUtils::Heading(lastFixedLocation,currentLocation);
		float bearingToRoute = MapUtils::Heading(currentLocation,nextRouteLocation);
		double diff = MapUtils::degreesDiff(bearingMotion, bearingToRoute);
		// 6. Suppress turn prompt if prescribed direction of motion is between 45 and 135 degrees off
		if (abs(diff) > 60.0f) {
			return true;
		}
	}
	return false;
}
void RouteManager::test1()
{
#if DO_TEST1	
	//double _x = -8849626.0167975128, _y = 5442343.4923960557;  // point in Margarita Rd
	/*double _x = -8849605.6782446131, _y = 5442285.0598358382 ;
	carto::MapPos orig(_x, _y);

	auto proj = nativeMapView->getMapV()->getOptions()->getBaseProjection();
	carto::MapPos P = proj->toWgs84(orig);
*/
	
	carto::MapPos P(-79.500557-0.01, 43.850101);

	carto::MapPos PA(-79.500557, 43.850101);

	carto::MapPos PB(-79.494728, 43.851421);

	float d = MapUtils::getOrthogonalDistance(P, PA, PB);//MapUtils::getDistanceToLineSegment(PA, PB, P);
	if (d > 0)
	{
		d = d;
	}

	d = MapUtils::getDistanceToLineSegment(PA, PB, P);
	if (d > 0)
	{

	}

	//-------------------------------------------------
	valhalla::midgard::PointLL p(-79.500557 - 0.01, 43.850101);
	std::vector<valhalla::midgard::PointLL> pnts ;

	pnts.push_back(valhalla::midgard::PointLL(-79.500557, 43.850101));
	pnts.push_back(valhalla::midgard::PointLL(-79.494728, 43.851421));
	

	std::tuple<valhalla::midgard::PointLL, float, int> resCP = p.ClosestPoint(pnts);

	valhalla::midgard::PointLL ClosestP = std::get<0>(resCP);
	float distToClosestPoint = std::get<1>(resCP);
	int indexOnTheLine = std::get<2>(resCP);

	if (distToClosestPoint < 20)
	{
	}

#endif
}
//---------------------------------------------------------------
int RouteManager::lookAheadFindMinOrthogonalDistance(carto::MapPos &loc, PRouteO *rO, int currentRoute, int iterations) {
	float newDist;
	float dist = FLT_MAX;// DBL_MAX;
	int index = currentRoute;
	PLegO* leg = r_route->getLegO(0);
	int route_nodes_size = leg->geo_points_size();
	while (iterations > 0 && currentRoute + 1 < route_nodes_size) {
		carto::MapPos p1, p2;
		bool rv1 = r_route->getRoutePointAt(currentRoute , p1);
		bool rv2 = r_route->getRoutePointAt(currentRoute+1, p2);
		newDist = MapUtils::getOrthogonalDistance(loc, p1, p2);
		if (newDist < dist) {
			index = currentRoute;
			dist = newDist;
		}
		currentRoute++;
		iterations--;
	}
	return index;
}

bool RouteManager::updateCurrentRouteStatus(MyLocation_T &mloc)
{
	carto::MapPos curLoc(mloc.lon, mloc.lat);
	int currentRoute = r_route->currentRoute;
	// 1. Try to proceed to next point using orthogonal distance (finding minimum orthogonal dist)
	PRouteO *rO=r_route->getRouteO(0);
	PLegO* leg = r_route->getLegO(0);
	int route_nodes_size = leg->geo_points_size();
	while (currentRoute + 1 < route_nodes_size) {
		
		float dist = 0.0;
		carto::MapPos p1, p2;
		bool rv2 = r_route->getRoutePointAt(currentRoute, p2);
		if (currentRoute > 0) {
			bool rv1 = r_route->getRoutePointAt(currentRoute - 1, p1);
			dist = MapUtils::getOrthogonalDistance(curLoc, p1, p2);
		}
		else {
			dist = MapUtils::distance(curLoc, p2);
		}
		bool processed = false;
		// if we are still too far try to proceed many points
		// if not then look ahead only 3 in order to catch sharp turns
		bool longDistance = dist >= 250;
		int newCurrentRoute = lookAheadFindMinOrthogonalDistance(curLoc, rO, currentRoute, longDistance ? 15 : 8);
		bool rv1 = r_route->getRoutePointAt(newCurrentRoute + 1, p1);
		float newDist = MapUtils::getOrthogonalDistance(curLoc, p2,p1);
		if (longDistance) {
			if (newDist < dist) {
				processed = true;
			}
		}
		else if (newDist < dist || newDist < 10) {
			// newDist < 10 (avoid distance 0 till next turn)
			if (dist > POSITION_TOLERANCE) {
				processed = true;
				
			}
			else {
				// case if you are getting close to the next point after turn
				// but you have not yet turned (could be checked bearing)
				if (mloc.hasBearing() || lastFixedLocation.hasBearing() /*mloc.flags==3*/) {
					float bearingToRoute = MapUtils::Heading(mloc,p2);
					if (bearingToRoute != 0.0)
					{
						bool rv1 = r_route->getRoutePointAt(newCurrentRoute , p1);
						bool rv2 = r_route->getRoutePointAt(newCurrentRoute + 1, p2);
						float bearingRouteNext = MapUtils::Heading(p1, p2);
						float bearingMotion = mloc.hasBearing() ? mloc.getBearing() : MapUtils::Heading(lastFixedLocation,mloc);
						double diff = abs(MapUtils::degreesDiff(bearingMotion, bearingToRoute));
						double diffToNext = abs(MapUtils::degreesDiff(bearingMotion, bearingRouteNext));
						if (diff > diffToNext) {

							processed = true;
						}
					}
				}
			}
		}
		if (processed) {
			// that node already passed
			r_route->updateCurrentRoute(newCurrentRoute + 1);
			currentRoute = newCurrentRoute + 1;
		}
		else {
			break;
		}
	}

	// 2. check if destination found
	carto::MapPos lastPoint;
	r_route->getFinalRoutePoint(lastPoint);
	if (currentRoute > route_nodes_size - 3 && MapUtils::distance(mloc,lastPoint) < POSITION_TOLERANCE) {
		//showMessage(context.getString(R.string.arrived_at_destination));
		//voiceRouter.arrivedDestinationPoint();
		//clearCurrentRoute(null);
		if (voiceRouter)voiceRouter->updateRouteInformation("You have arrived at your destination");
		
		return true;
	}
	return false;

}

//----------------------------------------


}//namespace routing
}//namespace cartodb


