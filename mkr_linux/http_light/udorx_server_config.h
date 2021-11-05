#ifndef UDORX_CONFIG_SERVER_H
#define UDORX_CONFIG_SERVER_H

#define DefaultPage  "index.html"

#define TCP_LISTENER_IPADDR_SERVER_PORT 2411
#define MKR_MQTT_IP_ADDR TCP_LISTENER_IPADDR 
#define MKR_MQTT_PORT  1883

#define USE_LIBEVENT	1

#define USE_UNITED_CHARGERS_CONFIG	1


// platform specific definitions
#ifdef MKR_LINUX
#if USE_UNITED_CHARGERS_CONFIG
//#include "uc_dev_server_linux_config.h"
#include "uc_server_linux_config.h"
#else
#include "udorx_server_linux_config.h"
#endif
#else
#include "udorx_server_win_config.h"
#endif



#endif //#ifndef UDORX_CONFIG_SERVER_H
