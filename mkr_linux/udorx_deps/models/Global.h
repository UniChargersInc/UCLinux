#ifndef MKR_GLOBAL_H_
#define MKR_GLOBAL_H_
#include <stdio.h>
#include <stdlib.h>

#include "MkrLib/CommonUtils/Dbg.h"
#include "employee_row.h"
#include "TimeUtil.h"

#define PVehicle void*
#define PVehicleList void*

using namespace HosEngine;

enum MessagesCBEnums
{
	MsgCB_GetActivities = 50,
	MsgCB_GetAllHosEvents = 60,
	MsgCB_UpdateObdBleMessage = 70,
	MsgCB_HosEngine_Update = 1000,
	MsgCB_ChatLoginMessage     = 1001,
	MsgCB_ChatMessage      = 1002,
	MsgCB_ChatOcppMessage = 2002
};


enum MkrMapOptions  // corresponds to Map Settings in MkrNavConst.java
{
	MAP_SET_MYLOC = 1,
	MAP_SET_COMPASS = 2,
	MAP_SET_NORTHUP = 4,
	MAP_SET_VOICE = 8,
	MAP_SET_SPEEDCAM = 16,
	MAP_SET_USE_WIFI_ONLY = (1 << 5),  // bit 5

	GEN_SET_USE_VOICE_ASSISTANT = (1 << 8),
	GEN_SET_USE_OFFLINE_MODE = (1 << 9),

	HOS_SET_USE_MANUAL_CHANGE = (1 << 16),
	HOS_SET_USE_VOICE_NOTIF = (1 << 17),
	HOS_SET_LOCK_SCREEN = (1 << 18)
};


#include "mylocation.h"

class MsgUserCls
{
public:
    string senderId="";
    string senderName="";
    int64_t ts;
    MsgUserCls(/*string did,string name*/PMsg *pm);
    
};


class Global
{
	static Global *_instance;
	Global() {}
	//ClsLoginMD *pL=0;
	int did = 0;
	int cid = 0;
	int bkgMode = -1,      // 0 - foreground , 1 - background
		network_flags=0;   // 1 - wifi only, 2 - cellullar only, 3 - both
    /*PVehicle*/void *selectedVeh=0;//int64_t selectedVId=0;
	string DrName = "";
	EmployeeRow *prof=0;
    
	MyLocation_T mloc = {};

    //======messagingUsers========
    vector<MsgUserCls> msg_users;
    int selectedUserIndex=0;

	unsigned int map_options=0;
public:

	static Global *Get()
	{
		if (_instance == 0)
			_instance = new Global();
		return _instance;
	}
	void SetLogin(ClsLoginMD *pL);
	void SetRegister(PEmpl *pE);
	string GetDriverID();
    int64_t GenerateUID();
    string GenerateTextUID();
	int GetDID();
	int GetCID();
	string GetDriverName();
	void CleanUp();

	void setBkgMode(int m){ bkgMode = m; }
	bool isBkgMode() { return bkgMode==1; }

	void setNetworkFlags(int m) { network_flags = m; }
	int getNetworkFlags() { return network_flags; }

	EmployeeRow *GetCurrentProfile();
	void SetCurrentProfile(EmployeeRow *prof);
	int UpdateCurrentProfile(EmployeeRow *);
    
    void    setSelectedVehicle(string vid);
    string getSelectedVehicleID();
    PVehicle *getSelectedVehicle();
    
    MyLocation_T &getCurrentLocation();
	void setLocation(double lat, double lon,float speed, float cog, int flag);
    //=========messaging Users==============
    bool addMsgUser(MsgUserCls &user);
    bool getMsgUserById(string did,MsgUserCls &user);
    bool getSenderIdByMid(string mid,string &senderID,int64_t &ts);
    std::vector<MsgUserCls>& getMsgUsers(){return msg_users;}
    void setSelectedUserIndex(int ind){selectedUserIndex=ind;}
    int getSelectedUserIndex(){return selectedUserIndex;}
    std::string getUserInitials(std::string name);

	//---map options-----
	unsigned int getMapOptions() { return map_options; }
	void setMapOptions(unsigned int opt) { map_options= opt; }
};


MyLocation_T GetMyLocation();





#endif
