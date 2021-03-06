/* lws_config.h  Generated from lws_config.h.in  */

#ifndef NDEBUG
	#ifndef _DEBUG
		#define _DEBUG
	#endif
#endif

/* Define to 1 to use wolfSSL/CyaSSL as a replacement for OpenSSL.
 * LWS_OPENSSL_SUPPORT needs to be set also for this to work. */
//#cmakedefine USE_WOLFSSL

/* Also define to 1 (in addition to USE_WOLFSSL) when using the
  (older) CyaSSL library */
//#cmakedefine USE_OLD_CYASSL

/* The Libwebsocket version */
//#cmakedefine LWS_LIBRARY_VERSION "${LWS_LIBRARY_VERSION}"

/* The current git commit hash that we're building from */
//#cmakedefine LWS_BUILD_HASH "${LWS_BUILD_HASH}"

/* Build with OpenSSL support */
//#cmakedefine LWS_OPENSSL_SUPPORT

/* The client should load and trust CA root certs it finds in the OS */
//#cmakedefine LWS_SSL_CLIENT_USE_OS_CA_CERTS

/* Sets the path where the client certs should be installed. */
//#cmakedefine LWS_OPENSSL_CLIENT_CERTS "${LWS_OPENSSL_CLIENT_CERTS}"

/* Turn off websocket extensions */
#define LWS_NO_EXTENSIONS

/* Enable libev io loop */
//#cmakedefine LWS_USE_LIBEV

/* Build with support for ipv6 */
//#cmakedefine LWS_USE_IPV6

/* Build with support for HTTP2 */
//#cmakedefine LWS_USE_HTTP2

/* Turn on latency measuring code */
#define LWS_LATENCY

/* Don't build the daemonizeation api */
#define LWS_NO_DAEMONIZE

/* Build without server support */
//#define LWS_NO_SERVER

/* Build without client support */
#define LWS_NO_CLIENT

/* If we should compile with MinGW support */
//#cmakedefine LWS_MINGW_SUPPORT

/* Use the BSD getifaddrs that comes with libwebsocket, for uclibc support */
//#cmakedefine LWS_BUILTIN_GETIFADDRS

/* use SHA1() not internal libwebsockets_SHA1 */
//#cmakedefine LWS_SHA1_USE_OPENSSL_NAME

//${LWS_SIZEOFPTR_CODE}
