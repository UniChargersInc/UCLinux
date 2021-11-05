#ifndef MKRTCPLISTENER_H
#define MKRTCPLISTENER_H

#define USE_LIEVENT	0

#include "proto/udorx_comm.pb.h"
#include "proto/udorx_common.pb.h"

#include <MkrLib.h>
#include <pthread.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


//==========================================================================================

#include <sys/types.h>

#ifdef WIN32
#define socklen_t int

#include <winsock2.h>
#include <io.h>
#define my_closesocket closesocket
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* Required by event.h. */
#include <sys/time.h>

#include <err.h>
#define my_closesocket close
#endif

/* On some systems (OpenBSD/NetBSD/FreeBSD) you could include
* <sys/queue.h>, but for portability we'll include the local copy. */
//#include "compat/sys/queue.h"

#if USE_LIEVENT
//https://github.com/jasonish/libevent-examples/blob/master/chat-server/chat-server.c
#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#endif
#include "network/TcpRequest.h"
#include "network/Convert.h"


//////int file_type  1 - vehicle image, 2 - inspection image, 3 - mobile worker image
//enum DownloadFileTypeEnum
//{
//	MEDIA_FILE_NONE = 0,
//	MEDIA_FILE_VEHICLE_IMAGE = 1,
//	MEDIA_FILE_INSPECTION_IMAGE = 2,
//	MEDIA_FILE_MOBILEWORKER_IMAGE = 3,
//
//	BIG_MEDIA_FILE_DOWNLOAD = 100,
//
//	BIG_MAP_TILE_FILE_DOWNLOAD = 101,
//	BIG_ROUTE_TILE_FILE_DOWNLOAD = 102,
//	BIG_GEOCODER_FILE_DOWNLOAD = 103,
//
//	BIG_MEDIA_FILE_DOWNLOAD_FW_CPU = 110,
//	BIG_MEDIA_FILE_DOWNLOAD_FW_FRAPHICS = 111,
//};


using namespace std;
using namespace udorx_pbf;

void start_tcp_listener();
void stop_tcp_listener();

void process_tcp_listener_request(/*struct bufferevent *bev*/int sockfd,unsigned char *buf, int len);

#endif