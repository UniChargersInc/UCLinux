
#include <cstdio>
#include <cstring>
//#include "../network/NetComm.h"

#include "CommonUtils/my_utils.h"
#include "ClsMqtt.h"
//#include "WorkOrderDB.h"

using namespace std;

#ifdef WIN32
// fake implementation
void JniApi_PostReceivedMsgProto(int req, int par, void *payload, int payload_len) {}
#endif
void process_mqtt_channel_message(ClsMqtt *mqtt,string did, unsigned char *payload, int len);
void process_publish_callback(int mid) {}


mqtt_client::mqtt_client()
{
}

mqtt_client::~mqtt_client()
{
}

mqtt_client::mqtt_client(const char *id) : mosquittopp(id)
{
}
int mqtt_client::mqtt_connect(const char *host, int port)
{

	int keepalive = 60;

	/* Connect immediately. This could also be done by calling
	* mqtt_client->connect(). */
	LOGI("mqtt_client.. connect %d\n", keepalive);
	connect(host, port, keepalive);
	return 0;
}

void mqtt_client::on_connect(int rc)
{
	owner->OnConnect(rc);
}
void mqtt_client::on_disconnect(int rc)
{
	owner->OnDisconnect(rc);
	//LOGI("On DISCONNECT,,,\r\n");
}


void mqtt_client::on_message(const struct mosquitto_message *message)
{
	
	string topic = string(message->topic);
	//string cur_did = Global::Get()->GetDriverID();

	LOGI("Message Received Topic=%s \r\n", message->topic);//LOGI("%s\n", str);
	vector<string> v = split(topic, '/');
	string s = v[0];
	bool done = false;
	if (v.size() >= 3)
	{
		string s2=v[2], s1 = v[1];
		if (s == "SI" && !s1.empty() && !s2.empty())
		{
			if (s2 == "Q")
			{
				string did = v[1];
				LOGI("Login message for %s\n", s2.c_str());
				//process_mqtt_channel_message(owner,did,(unsigned char*)message->payload, message->payloadlen);
				done = true;
			}
			else if (s == "S")
			{
				// this is a state from mobile ( 1 - on line , 0 - off line)
				//string msg = System.Text.Encoding.UTF8.GetString(e.Message);
				//MTcpLLog.mqtt_log("State Received..topic=" + e.Topic + " msg:" + msg);
				done = true;
			}
			else {
				LOGI("OCPP message %s %s len:%d  pld:%s\n", s1.c_str(), s2.c_str(), message->payloadlen,message->payload);
				void processGrizzleMsgFromMobile(int len, void *data);
				processGrizzleMsgFromMobile(message->payloadlen, message->payload);
				
				done = true;
			}
		}
		
		
	}
	if(!done)
	{
		LOGI("Invalid message on topic %s\r\n", topic.c_str());
	}
	
}

void mqtt_client::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
	owner->OnSubscribe(mid, qos_count, granted_qos);

}

void mqtt_client::on_publish(int mid)
{
	LOGI("on_publish succeeded. mid=%d\n",mid);
	process_publish_callback(mid);
}

