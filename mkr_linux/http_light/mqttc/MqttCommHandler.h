#ifndef MqttCommHandler_H
#define MqttCommHandler_H
#include "proto/udorx_comm.pb.h"
#include "proto/udorx_common.pb.h"
#include "proto/udorx_ws_comm.pb.h"

#include <MkrLib.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
//==========================================================================================

#include <sys/types.h>
#include <unistd.h>

#ifdef WIN32

#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* Required by event.h. */
#include <sys/time.h>
#include <err.h>
#endif
#include "network/TcpRequest.h"
#include "network/Convert.h"

using namespace std;
using namespace udorx_pbf;




#endif
