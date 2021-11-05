#pragma once

#include "m_routing/RoutingResult.h"

#ifdef WIN32
#include "../Models/Global_win.h"
#elif ANDROID

#include "models/Global.h"
#endif

namespace cartodb {
namespace routing {

	enum VRStatus
	{
		STATUS_UTWP_TOLD=-1,
		STATUS_UNKNOWN=0,
		
		STATUS_LONG_PREPARE ,
		STATUS_PREPARE,
		STATUS_TURN_IN ,
		STATUS_TURN,
		STATUS_TOLD 
	};

//#define DEFAULT_SPEED	10

	class RouteManager;

		class VoiceRouter
		{
			//std::shared_ptr<carto::RoutingResult> route;
			VRStatus currentStatus = VRStatus::STATUS_UNKNOWN;
			float playGoAheadDist = 0;
			RouteManager *rm;

			int nextRouteDirectionIndex = 0;
			int prevRouteDirectionIndex = 0;
#ifdef ANDROID
			std::chrono::time_point<std::chrono::system_clock> lastVoiceNotif;
#endif

			bool playGoAheadToDestination();
			bool playGoAhead(float dist);
			bool playMakeUTwp();
			bool playPrepareTurn(PStepO *next,float dist);
			//void playMakeTurnIn(carto::RoutingInstruction & next, int dist, carto::RoutingInstruction* pronounceNextNext);
			//void playMakeTurn(carto::RoutingInstruction & next, carto::RoutingInstruction* nextNext);

			void playMakeTurn_1(int indexInstr);
			void playPastVerbalInstruction(int indexInstr);

		protected:
			
			void nextStatusAfter(int previousStatus);
			bool statusNotPassed(int statusToCheck);

			bool isDistanceLess(float currentSpeed, double dist, double etalon, double defSpeed);
			bool isDistanceLess(float currentSpeed, double dist, double etalon);
			float DEFAULT_SPEED = 12;
			float TURN_DEFAULT_SPEED = 5;

			int PREPARE_LONG_DISTANCE = 0;
			int PREPARE_LONG_DISTANCE_END = 0;
			int PREPARE_DISTANCE = 0;
			int PREPARE_DISTANCE_END = 0;
			int TURN_IN_DISTANCE = 0;
			int TURN_IN_DISTANCE_END = 0;
			int TURN_DISTANCE = 0;
		public:
			VoiceRouter(RouteManager *m)
			{
				this->rm = m;

				reset();
			}
			void reset()
			{
				PREPARE_DISTANCE = 1500;    //(125 sec)
				PREPARE_DISTANCE_END = 1200;//(100 sec)
				TURN_IN_DISTANCE = 390;     //  30 sec
				TURN_IN_DISTANCE_END = 182; //  14 sec
				TURN_DISTANCE = 50;         //  7 sec
				TURN_DEFAULT_SPEED = 7.0f; 	//  25 km/h
				DEFAULT_SPEED = 13;         //  48 km/h

				nextRouteDirectionIndex = 0;
				prevRouteDirectionIndex = 0;
				
				currentStatus = VRStatus::STATUS_UNKNOWN;
				playGoAheadDist = 0;
			}
			/*void setRoute(std::shared_ptr<carto::RoutingResult> route)
			{
				this->route = route;
			}*/
			void updateVoiceRouterStatus(MyLocation_T &mloc, bool wrongMovementDirection,bool uTurnIsNeeded);
			int calculateImminent(float dist, MyLocation_T &mloc);

			void updateRouteInformation(int index, float angle,bool fromRecalcThread=false);
			void updateRouteInformation(std::string msg, std::string textToSpeak="", int action=0,float angle=0.0);
		};

}
}
