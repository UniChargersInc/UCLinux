#include "MqttCommHandler.h"
#include "ClsMqtt.h"

#include "db/FleetEmployeeDB.h"
#include "db/FleetAssetsDB.h"



void process_mqtt_channel_message(ClsMqtt *mqtt,string did, unsigned char *payload, int len)
{
	string req_str = string((char*)payload, len);
	CommPacket commP;
	commP.ParseFromString(req_str);
	switch (commP.req())
	{
	case HttpRequestNumber::HBReq:
		/*try {
			process_hb(commP, 0);
		}
		catch (exception const &e)
		{
			const char *desc=e.what();
			to_log_file(desc);
		}
		catch (SQLiteException ex)
		{
			string desc = ex.GetString();
			to_log_file(desc);
		}*/
		break;
	case HttpRequestNumber::SendTimeLogs:
		//ReceiveTimeLogs(commP);
		break;
	case HttpRequestNumber::SendInspectionReports:
		//ReceiveInsReports(commP);
		break;
	case HttpRequestNumber::Send4VeicleInfo:
		//ReceiveVehicleReports(commP);
		break;
	case HttpRequestNumber::GetActivities:
		//GetWorkOrdersFromDB(mqtt, commP);
		break;
	/*case HttpRequestNumber::UpdateActivites:
		
		break;*/
	case HttpRequestNumber::Emails:
		
		break;
	case HttpRequestNumber::SendEmpRow:
		//SendEmpProfileFromMobile(mqtt, commP);
		break;
	}
}