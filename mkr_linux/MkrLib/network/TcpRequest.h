#ifndef MKR_TCPREQUEST_H_INCLUDED
#define MKR_TCPREQUEST_H_INCLUDED


#include "Convert.h"
#ifndef WIN32
#include "MkrLib.h"
#endif
#include "proto/udorx_comm.pb.h"

#include  <functional>   //std::function

using namespace std;
using namespace udorx_pbf;


typedef unsigned char  U8;
typedef unsigned char  byte;
typedef unsigned int  uint;

class TcpRequestObj;
typedef void(*request_cb_t)(void *o);

class TcpRequestObj;
//typedef void(*request_cb_t)(void *o);
using FileDownloadResponse_CB = std::function<void(void *)>;
#define request_cb_t	FileDownloadResponse_CB

enum TcpResponseResultCode
{
	TCP_NONE = 0,
	TCP_SUCCESS = 1,
	TCP_ERROR_NO_CONNECTION = 2,
	TCP_ERROR_CONNECTION_INTERRUPTED = 3,

	TCP_FILE_DOWNLOAD_IN_CACHE = 200
};

////int file_type  1 - vehicle image, 2 - inspection image, 3 - mobile worker image
enum DownloadFileTypeEnum
{
	MEDIA_FILE_NONE = 0,
	MEDIA_FILE_VEHICLE_IMAGE = 1,
	MEDIA_FILE_INSPECTION_IMAGE = 2,
	MEDIA_FILE_MOBILEWORKER_IMAGE = 3,
	MEDIA_FILE_MSG_IMAGE = 4,
	MEDIA_FILE_MSG_AUDIO = 5,
	MEDIA_FILE_MSG_VIDEO = 6,

	BIG_MEDIA_FILE_DOWNLOAD = 100,

	BIG_MAP_TILE_FILE_DOWNLOAD = 101,
	BIG_ROUTE_TILE_FILE_DOWNLOAD = 102,
	BIG_GEOCODER_FILE_DOWNLOAD = 103,

	PTRIP_DOWNLOAD = 104,

	BIG_MEDIA_FILE_DOWNLOAD_FW_CPU=105,
	BIG_MEDIA_FILE_DOWNLOAD_FW_FRAPHICS=106,

	BIG_MEDIA_FILE_DOWNLOAD_FW_ESP32 = 107

};
typedef struct DownloadFileResponseStruct
{
	int result;       // result 1 - success, -1 - failure , etc
	int progress;     // download progress in %
	DownloadFileTypeEnum file_type;
	string filename;  //full path to the dowloded file 

	DownloadFileResponseStruct()
	{
		result = 0;
		progress = 0;
		file_type = DownloadFileTypeEnum::MEDIA_FILE_NONE;
		filename = "";
	}
};




enum HttpRequestNumber
{
	Login = 1,
	RegisterMD = 2,
	Logout=3,
	HBReq = 10,   // from mobile

	SendTLR = 20,
	SendTimeLogs = 21,
	SendEmpRow = 23,

	StoreTokenToServer = 40,

	Emails = 41,
	SearchRequest = 42,

	SendHosAlert = 43,
	SendHosRecap = 44,

	GetActivities = 50,
	UpdateActivitesStatusList = 51,
	SendMsgFromMobile = 53,
	ReadMsgFromServer = 54,
	MarkMsgAsSent = 55,

	GetWorkOrderTrip=56,

	GetAllHosEvents = 60,

	GetChargeStationsStatistics=70,
	SendOcppCommandRequest=71,

	RequestGetImagesFromServer=90,
	RequestDownloadFileFromServer = 91,

	Request4Categories = 80,
	Send4VeicleInfo = 81,
	SendTripInfo = 82,
	SendAttRequest = 83,
	SendAttRequest4CatIds = 84,
	Request4HosTimeLogs = 85,
	Request4HosInspReports = 86,
	SendInspectionReports = 100,   // from WebStreamRequest

	LoadMapTileData = 150,

	SendFCMData  = 160,

	SpecialCase=255
};


class TcpRequestObj
{
private:
	HttpRequestNumber reqN;
	request_cb_t cb;
	string request;
	string response;
public:
	void *parObj;
	TcpResponseResultCode result;
	
	TcpRequestObj(request_cb_t cb, HttpRequestNumber req, string &payload, int rid, int cid, string user)
	{
		this->cb = cb;
		this->response = "";
		this->parObj = 0;
		this->reqN = req;
		CommPacket commP;
		commP.set_req(req);
		commP.set_rid(rid);
		commP.set_cid(cid);
		commP.set_user(user);
		commP.set_payload(payload);
		this->request = commP.SerializeAsString();
	}
	~TcpRequestObj()
	{
		request.clear();
		response.clear();
	}
	void SetResponse(U8 *b, int len)
	{
		response = string((char*)b, len);
	}
	string &GetRequest() { return request; }
	string &GetResponse() { return response; }
	HttpRequestNumber GetReqN() { return reqN; }
	void ExecuteCB()
	{
		if (cb != 0)
			cb(this);
	}
};

#endif
