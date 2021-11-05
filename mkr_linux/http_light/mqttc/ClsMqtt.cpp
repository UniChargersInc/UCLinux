#include "ClsMqtt.h"

#include "udorx_server_config.h"

#ifdef WIN32
void usleep(int d) 
{
	Sleep(d / 1000);
}
#endif



ClsMqtt *ClsMqtt::this_instance = 0;


int ClsMqtt::Publish(const char* topic, const char* msg,int msg_size)
{
	if (!isConnected())
		return -1;
	/*char topic[128];
	::snprintf(topic,128,"WI/%s",did);*/
	int mid = 0;
	if(mclient)
		mclient->publish(&mid,topic, msg_size, msg);
	LOGI("sent2server...mid=%d topic:%s size:%d\n",mid, (char*)topic,msg_size);
	return mid;
}


void *ClsMqtt::doIt()
{

	int loop_ret=0,rc1=0;

	mosqpp::lib_init();
	string my_client = "mqtt-local";//+ Global::Get()->GetDriverID();
	mclient = new mqtt_client(my_client.c_str());
	mclient->setOwner(this);
	conn_state = MqttConnStates::WAITING_CONN_ACK;

	//int rv=mclient->tls_set(mkr_mbedtls_x509_crt_ca_file,0, mkr_mbedtls_x509_crt_cert_file, mkr_mbedtls_pk_cert_keyfile);
	//printf("svk-tls_set < %d >\r\n", rv);

	mclient->mqtt_connect(MKR_MQTT_IP_ADDR, MKR_MQTT_PORT);
		

	prev_time=mclient->get_mqtt_time();
	prev_hos_engine_time = prev_time;

	terminate_mqtt_service = false;
	while (!terminate_mqtt_service) {
#if 0		
		loop_ret = mclient->loop();
		if (loop_ret) {
			LOGI(" doIt..mclient->loop error %d.\n", loop_ret);
			mclient->reconnect();
		}
		
#else
		
		loop_ret = mclient->loop();
		if (loop_ret) {
		//	LOGI(" doIt..mclient->loop error %d.\n", loop_ret);
			usleep(1000 * 1000);
			//conn_state = MqttConnStates::ERROR_LOOP;
			if (conn_state == MqttConnStates::DISCONNECTED)
			{
				//LOGI(" doIt..Trying to Reconnect...\n");
				rc1 = mclient->reconnect();
				if (rc1 == 0)
				{
					conn_state = MqttConnStates::WAITING_CONN_ACK;
					LOGI(" doIt..MqttConnStates::WAITING_CONN_ACK...\n");
				}
				else {
					conn_state = MqttConnStates::ERROR_ON_RECONNECT;

				}
			}
			if (loop_ret == MOSQ_ERR_NO_CONN || loop_ret == MOSQ_ERR_CONN_REFUSED || loop_ret == MOSQ_ERR_CONN_LOST)
			{
				conn_state = MqttConnStates::DISCONNECTED;
			}
			usleep(1000 * 1000);
		}
		switch (conn_state)
		{
		case MqttConnStates::UNKNOWN:
			LOGI(" doIt..MqttConnStates::MqttConnStates::UNKNOWN...\n");
			usleep(1000 * 1000);
			
			break;
		case MqttConnStates::ERROR_LOOP:
		{
			
		}
			break;
		case MqttConnStates::ERROR_ON_RECONNECT:
			LOGI(" doIt..MqttConnStates::ERROR_ON_RECONNECT...\n");
			usleep(1000 * 1000);
			break;
		case  MqttConnStates::ON_CONNECTED:
		{
			//if (conn_state == MqttConnStates::WAITING_CONN_ACK)
			{
				int mid = 1;// , mid1 = 2;
				string topic = "SI/#";    // subscribe to get responses from server  
				int rc = mclient->subscribe(&mid, topic.c_str());
				LOGI("Mqtt ...Sending subscribe to topic:%s mid=%d\n", topic.c_str(), mid);
				if (rc == 0)
				{
					conn_state = MqttConnStates::SUBSCRIBE_1_SENT;
				}
			}
		}
		break;
				
		case MqttConnStates::CONNECTED:
			//processComms();
			break;
		}
#endif		
		usleep(10 * 1000);                                //sleep(seconds) or usleep(microseconds)
		
	}
	LOGI("Clean Mqtt ...\n");
	if (mclient)
		delete mclient;
	mosqpp::lib_cleanup();
	initialized = false;
	return 0;
}
//===========================================================================
//static std::thread::native_handle_type mqtt_thread_native_handle;
//static std::string sss = "req.req_str-to-server";
//int main_mqtt_publish_start() {
//	LOGI("MQTT Publish threadProc... \n");
//	
//	std::thread thrd = std::thread([]() {
//		for (;;)
//		{
//			printf("publishing..%s\r\n", sss.c_str());
//			ClsMqtt::Get()->Publish("2601", sss);
//			usleep(10 * 1000 * 1000);
//		}
//	});
//	mqtt_thread_native_handle = thrd.native_handle();
//	//if (ocpp_listener_thread.joinable())
//	thrd.detach();  //the thread to run in the background (daemon)
//
//	return 0;
//}


int  ClsMqtt::doInitMqtt(int par1, char *payload)
{
	int res = 0;
	if (initialized)
	{
		LOGI("MQTT thread init=1... \n");
		return -1;
	}
	/*	
	MqttThreadRequest_T *arg = new MqttThreadRequest_T();

	arg->par1 = par1;
	arg->par2 = 2;
	arg->payload = payload;
	arg->cb = 0;
	*/
	//createRenderingThread
	{
		
		if (pthread_create(&mqtt_thread, NULL, ClsMqtt::thread_helper_function, this) != 0)
		{
			LOGI( "ERROR MQTT threadProc... \n");//LOGE("ERROR threadProc... \n");

			/*if (arg != 0)
			{
				delete arg;
			}*/
			if (payload != 0)
				delete payload;//free(payload);
			return -2;
		}
		res = pthread_detach(mqtt_thread);
		initialized = true;

		//main_mqtt_publish_start();
	}
	return res;
}
void ClsMqtt::stopMqtt()
{
	if (initialized)
	{
		terminate_mqtt_service = true;

#ifndef WIN32
		pthread_cancel(mqtt_thread);
#endif
		initialized = false;
	}
}
//------------------------------------------------------------
void ClsMqtt::OnConnect(int rc)
{
	LOGI("Connected with code %d.\n", rc);
	
	if (rc == 0) {
		conn_state = MqttConnStates::ON_CONNECTED;
	}
	else
		conn_state = MqttConnStates::UNKNOWN;
}
void ClsMqtt::OnDisconnect(int rc)
{
	if (rc == MOSQ_ERR_SUCCESS) {}
	LOGI("OnDisconnect..-rc=%d\n", rc);
	conn_state = MqttConnStates::DISCONNECTED;
}
void ClsMqtt::OnSubscribe(int mid, int qos_count, const int *granted_qos)
{
	LOGI("Subscription succeeded.mid=%d\n", mid);
	conn_state = MqttConnStates::CONNECTED;
}
