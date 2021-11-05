#pragma once

#define USE_LOCAL_NETWORK 1

#ifdef WIN32
#define voip_listener_IPADDR "76.70.23.12"
#define voip_listener_IPADDR_SERVER_PORT	5060

#if !USE_LOCAL_NETWORK
#define voip_listener_IPADDR "192.168.0.20"
//#define voip_listener_IPADDR_SERVER_PORT	1884
#define voip_listener_IPADDR_SERVER_PORT	5060
#else
//#define voip_listener_IPADDR "127.0.0.1"
//#define voip_listener_IPADDR_SERVER_PORT	5060
#endif


#define voip_ws_listener_IPADDR "76.70.23.12"
#define	voip_ws_listener_PORT  	9002

#else
//LINUX host 173.208.164.58
#define voip_listener_IPADDR "173.208.164.58"
#define voip_listener_IPADDR_SERVER_PORT	5060

#define voip_ws_listener_IPADDR "173.208.164.58"
#define	voip_ws_listener_PORT  	9002

#endif

#define USE_UDP_SIP_TRANSPORT	1
#define USE_TCP_SIP_TRANSPORT   0