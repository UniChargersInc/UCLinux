#pragma once

#include <string>


#include "native_map_view.hpp"

#include "m_routing/RoutingRequest.h"
#include "m_routing/RoutingResult.h"

#include "vectorelements/Marker.h" // it was creating compiler error in picojson.h until I changed this line 
//// SNPRINTF(buf, sizeof(buf), "%" PRId64, u_.int64_);  //svk here I had a compiler error  -> PRId64
#ifdef WIN32
#include "../Models/Global_win.h"
#elif ANDROID
using namespace cartodb::mapview;
#include "models/Global.h"
#endif

#include "VoiceRouter.h"
#include <mutex>

namespace cartodb {
namespace routing {

	

	enum TBTNavStates
	{
		NONE,
		STARTED,
		FINISHED
	};

	
	class NativeMapView;
	

class RouteManager {
public:
	
	RouteManager(mapview::NativeMapView *nativeMapView);
    virtual ~RouteManager();
	
	mapview::NativeMapView * getNativeMapView() { return nativeMapView; }
		
	void updateLocation();   // from location manager
	int startRoute(bool from_recalcThread = false);
	void clearAllRouteAnnotation();
	PRouteObj* createRoute(std::vector</*carto::MapPos*/valhalla::LatLng> &v);
	bool isInProgress() { return false; }
	void test1();
	
	std::shared_ptr<carto::RoutingResult> getR_Route() { return r_route; }
	
	void recalculateRouteInBackground();
	static void do_recalculateRouteInBackground(void* _this);
private:
	bool isRouteValid();
	int printRoute(); // for test
	void showRouteLine();

	//void createRoutePointTitleDesc(carto::RoutingInstruction &instr, std::string &title, std::string &desc, std::string &imgN);
	void startTBT(bool from_recalcThread=false);
	void stopTBT();
	float getAngle(carto::MapPos &ppos1, carto::MapPos &ppos2);

	void processStateMachine(MyLocation_T &mloc);
	bool updateCurrentRouteStatus(MyLocation_T &mloc);
	int lookAheadFindMinOrthogonalDistance(carto::MapPos &currentLocation,PRouteO *rO /*std::vector<carto::MapPos> &routeNodes*/, int currentRoute, int iterations);
	
	void process_routing(MyLocation_T &mloc);
	bool checkWrongMovementDirection(MyLocation_T &mloc, carto::MapPos & nextRouteLocation);

	
	void updateNavMarkerOnMap(float lat,float lon,float cog);
	
	
	
	 TBTNavStates	sm_nav_state = TBTNavStates::NONE;

	std::shared_ptr<carto::RoutingResult> r_route;
	//long nativeMapViewPtr = 0;
	mapview::NativeMapView *nativeMapView = nullptr;
	std::shared_ptr<carto::Marker> navMarker;

	VoiceRouter *voiceRouter=0;

	int currentRoute = 0;
	MyLocation_T  lastFixedLocation;
	//std::chrono::time_point<std::chrono::system_clock> lastVoiceNitif;

	std::mutex r_mutex;
};
}
}
