
#include "VoiceRouter.h"

#ifndef WIN32
#include "nav_native_manager.hpp"

#else
#include "nav_native_manager_win.hpp"
#include "LogFile/CartoMkrLog.h"

void setTitle(std::string str);
#endif
#include <algorithm>
void iosSpeakText(const char *str);
namespace cartodb {
	namespace routing {


		void VoiceRouter::updateRouteInformation(int instructionIndex, float angle, bool fromRecalcThread )
		{
			if (!rm)
				return;
			std::shared_ptr<carto::RoutingResult> route=rm->getR_Route();
			if (route == nullptr)
				return;
			std::shared_ptr<PRouteObj> rObj = route->getRouteObj();
			if (rObj == nullptr) return;
			PRouteO *ro = rObj->mutable_routes(0);
			if (ro == NULL) return;
			PLegO *leg = ro->mutable_legs(0);
			if (leg == NULL) return;
			//MKR_LOGI_LL("updateRouteInformation_1\r\n");
			PStepO *instr = leg->mutable_steps(instructionIndex);//instrs.at(indexInstr);
			if (instr == NULL) return;
			int action = instr->mnv().action();
			std::string sName = instr->mnv().textinstr();
			//MKR_LOGI_LL("updateRouteInformation_2\r\n");
#ifdef ANDROID
			android::NavNativeManager::Get()->updateRouteBar(sName.c_str(), action, angle, fromRecalcThread?1:0);
			android::NavNativeManager::Get()->speakText(sName.c_str());
			//MKR_LOGI_LL("updateRouteInformation_3\r\n");
#elif defined (__APPLE__)
			iosSpeakText(sName.c_str());
#elif WIN32
			setTitle(sName);

#endif
		}
		void VoiceRouter::updateRouteInformation(std::string msg, std::string textToSpeak, int action,float angle)
		{

#ifdef ANDROID
			android::NavNativeManager::Get()->updateRouteBar(msg.c_str(), action, angle, 0);
			if(!textToSpeak.empty())
				android::NavNativeManager::Get()->speakText(textToSpeak.c_str());
#elif defined (__APPLE__)
			iosSpeakText(textToSpeak.c_str());
#elif WIN32
			setTitle(msg);
#endif
		}


		bool VoiceRouter::playGoAheadToDestination()
		{
			updateRouteInformation("Go ahead to destination");
			return true;
		}
		bool VoiceRouter::playGoAhead(float dist)
		{
			updateRouteInformation("Go ahead..");
			return true;
		}
		
		//void VoiceRouter::playMakeTurnIn(carto::RoutingInstruction & next, int dist, carto::RoutingInstruction  *pronounceNextNext)
		//{
		//	std::string msg;
		//	std::string name = next.getStreetName();
		//	if (name.empty())
		//	{
		//		//msg = next.toString();
		//	}
		//	else {
		//		msg = msg + " " + name;
		//	}

		//	updateRouteInformation(msg);
		//}
		//void VoiceRouter::playMakeTurn(carto::RoutingInstruction & next, carto::RoutingInstruction *nextNext)
		//{
		//	std::string msg = "make single turn";
		//	/*if (nextNext)
		//	{
		//		std::string name = nextNext->getStreetName();
		//		msg = msg + " " + name;
		//	}*/

		//	std::string name = next.getStreetName();
		//	if (name.empty())
		//	{
		//		//msg = next.toString();
		//	}
		//	else {
		//		msg = msg + " " + name;
		//	}
		//	

		//	updateRouteInformation(msg);
		//}
		bool VoiceRouter::playMakeUTwp()
		{
			updateRouteInformation("make u-turn turn");
			return true;
		}

		bool VoiceRouter::isDistanceLess(float currentSpeed, double dist, double etalon) {
			if (dist < etalon || ((dist / currentSpeed) < (etalon / DEFAULT_SPEED))) {
				return true;
			}
			return false;
		}

		bool VoiceRouter::isDistanceLess(float currentSpeed, double dist, double etalon, double defSpeed) {
			if (dist < etalon || ((dist / currentSpeed) < (etalon / defSpeed))) {
				return true;
			}
			return false;
		}

		int VoiceRouter::calculateImminent(float dist, MyLocation_T &mloc) {
			float speed = DEFAULT_SPEED;
			if ( mloc.hasSpeed()) {
				speed = mloc.getSpeed();
			}
			if (isDistanceLess(speed, dist, TURN_IN_DISTANCE_END)) {
				return 0;
			}
			else if (dist <= PREPARE_DISTANCE) {
				return 1;
			}
			else if (dist <= PREPARE_LONG_DISTANCE) {
				return 2;
			}
			else {
				return -1;
			}
		}


		void VoiceRouter::nextStatusAfter(int previousStatus) {
			//STATUS_UNKNOWN=0 -> STATUS_LONG_PREPARE=1 -> STATUS_PREPARE=2 -> STATUS_TURN_IN=3 -> STATUS_TURN=4 -> STATUS_TOLD=5
			if (previousStatus != STATUS_TOLD) {
				this->currentStatus = (VRStatus)(previousStatus + 1);
			}
			else {
				this->currentStatus = (VRStatus)previousStatus;
			}
		}

		bool VoiceRouter::statusNotPassed(int statusToCheck) {
			return currentStatus <= statusToCheck;
		}



	void VoiceRouter::updateVoiceRouterStatus(MyLocation_T &mloc, bool wrongMovementDirection, bool makeUturnWhenPossible)
	{
#if 0
		float speed = DEFAULT_SPEED;
		if (mloc.hasSpeed()) {
			speed = std::max(mloc.getSpeed(), speed);
		}

		// Mechanism via STATUS_UTWP_TOLD: Until turn in the right direction, or route is re-calculated in forward direction
		if (makeUturnWhenPossible == true) {
			if (currentStatus != STATUS_UTWP_TOLD) {
				if (playMakeUTwp()) {
					currentStatus = STATUS_UTWP_TOLD;
					playGoAheadDist = 0;
				}
			}
			return;
		}

#endif
		
		if (!rm)
			return;
		std::shared_ptr<carto::RoutingResult> route = rm->getR_Route();
		if (route == nullptr)
			return;

		carto::MapPos currentLocation(mloc.lon, mloc.lat);
		float dist; int ind;
		PStepO *nextInfo=route->getNextRouteDirectionInfo(currentLocation, dist,ind);

		if (nextInfo == nullptr)
			return;

#if 0
		if (!nextInfo.processed || nextInfo.getDistance()<=0.001)
		{
			if (currentStatus <= STATUS_UNKNOWN) {
				if (playGoAheadToDestination()) {
					currentStatus = STATUS_TOLD;
					playGoAheadDist = 0;
				}
			}
			return;
		}
			
		//float d = info.getDistance();
		float dist = nextInfo.getDistanceTo();
		//int ind = info.getPointIndex();
		int ind1 = nextInfo.get_directionInfoInd();
			
			
		// if routing is changed update status to unknown
		if (ind1 != nextRouteDirectionIndex) {
			nextRouteDirectionIndex = ind1;
			currentStatus = STATUS_UNKNOWN;
			playGoAheadDist = 0;
		}

		if (dist <= 0.0001 || currentStatus == STATUS_TOLD) {
			// nothing said possibly that's wrong case we should say before that
			// however it should be checked manually ?
			return;
		}

		// say how much to go if there is next turn is a bit far
		if (currentStatus == STATUS_UNKNOWN) {
			if (!isDistanceLess(speed, dist, TURN_IN_DISTANCE * 1.3)) {
				playGoAheadDist = dist - 80;
			}
			// say long distance message only for long distances > 10 km
			// if (dist >= PREPARE_LONG_DISTANCE && !isDistanceLess(speed, dist, PREPARE_LONG_DISTANCE)) {
			if (dist > PREPARE_LONG_DISTANCE + 300) {
				nextStatusAfter(STATUS_UNKNOWN);
			}
			else if (dist > PREPARE_DISTANCE + 300) {
				// say prepare message if it is far enough and don't say preare long distance
				nextStatusAfter(STATUS_LONG_PREPARE);
			}
			else {
				// don't say even prepare message
				nextStatusAfter(STATUS_PREPARE);
			}
		}


		carto::RoutingInstruction nextNextInfo;
		route->getNextRouteDirectionInfoAfter(nextInfo, nextNextInfo, false);
		if (statusNotPassed(STATUS_TURN) && isDistanceLess(speed, dist, TURN_DISTANCE, TURN_DEFAULT_SPEED)) {
			if (nextInfo.getDistance() < TURN_IN_DISTANCE_END && nextNextInfo.processed != false) {
				playMakeTurn(nextInfo, &nextNextInfo);
			}
			else {
				playMakeTurn(nextInfo, nullptr);
			}
			nextStatusAfter(STATUS_TURN);
		}
		else if (statusNotPassed(STATUS_TURN_IN) && isDistanceLess(speed, dist, TURN_IN_DISTANCE)) {
			if (dist >= TURN_IN_DISTANCE_END) {
				if ((isDistanceLess(speed, nextInfo.getDistance(), TURN_DISTANCE) || nextInfo.getDistance() < TURN_IN_DISTANCE_END) &&
					nextNextInfo.processed != false) {
					playMakeTurnIn(nextInfo, dist, &nextNextInfo);
				}
				else {
					playMakeTurnIn(nextInfo, dist, nullptr);
				}
			}
			nextStatusAfter(STATUS_TURN_IN);
			// } else if (statusNotPassed(STATUS_PREPARE) && isDistanceLess(speed, dist, PREPARE_DISTANCE)) {
		}
		else if (statusNotPassed(STATUS_PREPARE) && (dist <= PREPARE_DISTANCE)) {
			if (dist >= PREPARE_DISTANCE_END) {
				//if (nextInfo.getTurnType().keepLeft() || nextInfo.getTurnType().keepRight()) {
				if (nextInfo.getAction()==carto::RoutingAction::ROUTING_ACTION_KEEP_LEFT || nextInfo.getAction() == carto::RoutingAction::ROUTING_ACTION_KEEP_RIGHT) {
					// do not play prepare for keep left/right
				}
				else {
					playPrepareTurn(nextInfo, dist);
				}
			}
			nextStatusAfter(STATUS_PREPARE);
			// } else if (statusNotPassed(STATUS_LONG_PREPARE) && isDistanceLess(speed, dist, PREPARE_LONG_DISTANCE)){
		}
		else if (statusNotPassed(STATUS_LONG_PREPARE) && (dist <= PREPARE_LONG_DISTANCE)) {
			if (dist >= PREPARE_LONG_DISTANCE_END) {
				playPrepareTurn(nextInfo, dist);
			}
			nextStatusAfter(STATUS_LONG_PREPARE);
		}
		else if (statusNotPassed(STATUS_UNKNOWN)) {
			// strange how we get here but
			nextStatusAfter(STATUS_UNKNOWN);
		}
		else if (statusNotPassed(STATUS_TURN_IN) && dist < playGoAheadDist) {
			playGoAheadDist = 0;
			playGoAhead(dist);
		}

#else
		auto t2 = std::chrono::system_clock::now();
		if (currentStatus != STATUS_PREPARE &&
			currentStatus != VRStatus::STATUS_TOLD &&
			dist < 250.0)
		{
#ifdef ANDROID || defined (__APPLE__)
			long msecs = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - lastVoiceNotif).count();
			if (msecs > 2000)
			{
				lastVoiceNotif = t2;
				playPrepareTurn(nextInfo, dist);
				currentStatus = STATUS_PREPARE;
			}
#endif
		}
		if (currentStatus != VRStatus::STATUS_TOLD && dist < 60.0)
		{
			//updateRouteInformation(nextInfo.get_directionInfoInd(), 0);
			playMakeTurn_1(ind);
			currentStatus = VRStatus::STATUS_TOLD;
#ifdef ANDROID || defined (__APPLE__)
			lastVoiceNotif = t2;
#endif
		}
			
		if (prevRouteDirectionIndex != ind)
		{
			// make sure we are moving to this direction otherwise DO NOT PLAY !!!
			if (!wrongMovementDirection )  // distance to the next maneuver > 500 metwrs
			{
#ifdef ANDROID || defined (__APPLE__)
				long msecs = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - lastVoiceNotif).count();
				if (dist>500.0 && msecs > 1000)
				{
					playPastVerbalInstruction(prevRouteDirectionIndex);
					lastVoiceNotif = t2;
				}
#endif
				prevRouteDirectionIndex = ind;
				currentStatus = VRStatus::STATUS_UNKNOWN;
			}
			
		}
#endif		
			

	}

	void VoiceRouter::playPastVerbalInstruction(int indexInstr)
	{
		if (!rm)
			return;
		std::shared_ptr<carto::RoutingResult> route = rm->getR_Route();
		if (route == nullptr)
			return;

		//auto instrs = route->getInstructions();
		std::shared_ptr<PRouteObj> rObj = route->getRouteObj();
		PRouteO *ro = rObj->mutable_routes(0);
		PStepO *instr = ro->mutable_legs(0)->mutable_steps(indexInstr);//instrs.at(indexInstr);

		//std::string s_name = instr.getStreetName();

		std::string msg = instr->postverinst();   //instr.toString();

		//s_name += (" " + msg);
		if (!msg.empty())
		{
			PManeuverO mnvO = instr->mnv();
			updateRouteInformation(msg, msg, mnvO.action(), mnvO.azimuth());
		}
	}
	void VoiceRouter::playMakeTurn_1(int indexInstr)
	{
		if (!rm)
			return;
		std::shared_ptr<carto::RoutingResult> route = rm->getR_Route();
		if (route == nullptr)
			return;

		std::shared_ptr<PRouteObj> rObj = route->getRouteObj();
		PRouteO *ro = rObj->mutable_routes(0);
		PStepO *instr = ro->mutable_legs(0)->mutable_steps(indexInstr);//instrs.at(indexInstr);
		

		PManeuverO mnvO = instr->mnv();
		std::string msg = mnvO.textinstr();   //instr.toString();
		
		std::string name = instr->name();
		if (!name.empty())
		{
			updateRouteInformation(name,msg, mnvO.action(), mnvO.azimuth());
		}
		else if (!msg.empty()) {
			updateRouteInformation(msg,msg, mnvO.action(), mnvO.azimuth());
		}
		
	}

	bool VoiceRouter::playPrepareTurn(PStepO *next, float dist)
	{
		std::string msg = "Prepare in " + std::to_string((int)dist) + " meters";
		std::string name = next->mnv().textinstr();
		msg = msg + " " + name;
		updateRouteInformation("", msg, 0, 0);
		return true;
	}

}
}