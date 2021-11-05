#ifndef UC_CONFIG_LINUX_SERVER_H
#define UC_CONFIG_LINUX_SERVER_H

#define UDORX_PROD_SERVER  1
#define LOCALHOST_WINDOWS_SERVER  0
#define RUN_ON_WIN_SERVER 0              // 0 - run locally 

#define MKR_USE_SSL	1

#define DefaultPage  "index.html"

#if UDORX_PROD_SERVER
//#define TCP_LISTENER_IPADDR "76.70.23.12"   // Ruks win server
//#define TCP_LISTENER_IPADDR "173.208.164.58"
//#define TCP_LISTENER_IPADDR "192.168.0.15"

// UC
#define TCP_LISTENER_IPADDR "199.243.91.13" 

///* "localhost";*/ "192.168.0.15";//"127.0.0.1";
												 /* Port to listen on. */
#define HTTP_LIGHT_SERVER_IPADDR TCP_LISTENER_IPADDR
#define HTTP_LIGHT_SERVER_PORT 80
#define HTTPS_LIGHT_SERVER_PORT 443

#define LOGOUT_URL						"https://acharger.ca"
#define UDORX_URL_AFTER_LOGIN			"https://acharger.ca/mkr.html"

//#define UDORX_ADMIN_URL_AFTER_LOGIN		"http://acharger.ca/adm.html"

#define TCP_LISTENER_IPADDR_SERVER_PORT 2411
#else
#if LOCALHOST_WINDOWS_SERVER
//#define TCP_LISTENER_IPADDR "127.0.0.1"
#if RUN_ON_WIN_SERVER
#define TCP_LISTENER_IPADDR "192.168.123.9"
#define TCP_LISTENER_IPADDR_SERVER_PORT 2411
#define HTTP_LIGHT_SERVER_IPADDR "192.168.123.9"
#define HTTPS_LIGHT_SERVER_PORT 443
#define LOGOUT_URL	"https://mikerel.com"
#define UDORX_URL_AFTER_LOGIN	"https://mikerel.com/mkr.html"


#else

#define HTTP_LIGHT_SERVER_IPADDR "127.0.0.1"
//#define HTTP_LIGHT_SERVER_IPADDR "192.168.0.20"

#define TCP_LISTENER_IPADDR HTTP_LIGHT_SERVER_IPADDR
#define TCP_LISTENER_IPADDR_SERVER_PORT 2411

#if MKR_USE_SSL
#define HTTPS_LIGHT_SERVER_PORT 4433
#else
#define HTTPS_LIGHT_SERVER_PORT 80
#endif
        

#if MKR_USE_SSL
#define UDORX_URL_AFTER_LOGIN	"https://192.168.0.20:4433/mkr.html"
#define LOGOUT_URL	  "https://192.168.0.20:4433"  
#else
//#define UDORX_URL_AFTER_LOGIN	"http://192.168.0.20:80/mkr.html"
//#define LOGOUT_URL	  "http://192.168.0.20:80"  

#define UDORX_URL_AFTER_LOGIN	"http://localhost:80/mkr.html"
#define LOGOUT_URL	  "http://localhost:80"  
#endif

#endif

#else

#define TCP_LISTENER_IPADDR "192.168.0.15"
#define TCP_LISTENER_IPADDR_SERVER_PORT 2411
#define HTTP_LIGHT_SERVER_IPADDR "192.168.0.15"
#define HTTP_LIGHT_SERVER_PORT 5555
#define HTTPS_LIGHT_SERVER_PORT 4433
#define UDORX_URL_AFTER_LOGIN	"https://192.168.0.15:4433/mkr.html"
#define LOGOUT_URL	  "https://192.168.0.15:4433"           
#endif
#endif

#define MKR_MQTT_IP_ADDR TCP_LISTENER_IPADDR 
#define MKR_MQTT_PORT  1883


#if LOCALHOST_WINDOWS_SERVER
#define Mkr_RootDir  "C:\\my\\linux\\www"

//#define Udorx_RootDir  "C:\\my\\linux\\www\\udorx"
#define DB_ROOT_DIR	"c:\\my\\linux\\win_sqlite_db\\"

#define MEDIA_DVIR	"C:\\my\\linux\\www/media/dvir/"
#define MEDIA_VEH	"C:\\my\\linux\\www/media/v_img/"
#define WEB_RES_IMG_DIR "C:\\my\\linux\\www/media/resImg/" 
#define WEB_TEMPLETES	"C:\\my\\linux\\www\\mkr_files\\Templates\\"

//#define MBGL_MAP_DATA	"/home/mkr/www/mbgl/map_data/"
#define UDORX_MAP_DATA_DOWNLOAD_ROOT	"D:\\d_mb_tilemaker\\map_data_download\\"

#define REPORT_GEN_FILES	"C:\\my\\linux\\logs\\"
#define mkr_mbedtls_x509_crt_cert_file  "C:\\My\\Linux\\SSL_Certificate\\localhost.crt"
#define mkr_mbedtls_pk_cert_keyfile  "C:\\My\\Linux\\SSL_Certificate\\localhost.key"

#define MKR_SSL_PORT	"443"
#define MKR_SSL_HOST_ADDR NULL
#else
#define Mkr_RootDir  "/home/mkr/www"
#define Mkr_No_SSL_Ref  "index_http.html"

#define DB_ROOT_DIR	"/home/mkr/service/"
#define MEDIA_DVIR	"/home/mkr/www/media/dvir/"
#define MEDIA_VEH	"/home/mkr/www/media/v_img/"
#define WEB_RES_IMG_DIR	"/home/mkr/www/media/resImg/"
#define WEB_QRCODE_IMG_DIR	"/home/mkr/www/media/qrcodes/"

#define WEB_LABEL_IMG_DIR   "/home/mkr/www/media/label_logos/"

#define WEB_TEMPLETES	"/home/mkr/www/mkr_files/Templates/"

#define MEDIA_DIRECTORY "/home/mkr/www/media"

#define UDORX_MAP_DATA_DOWNLOAD_ROOT	"/home/mkr/service/map_data_download/"

#define UDORX_FIRMWARE_DATA_DOWNLOAD_ROOT	"/home/mkr/service/firmware_download"


#define REPORT_GEN_FILES	"/home/mkr/service/gen_files/"

#define mkr_mbedtls_x509_crt_cert_file  "/home/mkr/service/SSL_Certificate/mikerel.crt"
#define mkr_mbedtls_pk_cert_keyfile  "/home/mkr/service/SSL_Certificate/mikerel.key"

//#define MKR_SSL_PORT	"443"
//#define MKR_SSL_HOST_ADDR HTTP_LIGHT_SERVER_IPADDR

#endif

#define MKR_USE_INTERNAL_MAP_TILES_SERVICE	1

#if MKR_USE_INTERNAL_MAP_TILES_SERVICE
#if LOCALHOST_WINDOWS_SERVER
#define map_tiles_dir			"D:\\d_mb_tilemaker\\Toronto\\tiles_tt\\"
#define map_tiles_07_dir		"D:\\d_mb_tilemaker\\mb_load_tiles\\"
#define map_data_offline        "C:\\MY\\m_maps\\offline\\"

#define map_sat_tiles_dir		"D:\\d_mb_tilemaker\\mb_sat_tiles\\"

#else
#define map_tiles_dir			"/home/mkr/service/map_data/tiles_db/"
#define map_tiles_07_dir		"/home/mkr/service/map_data/tiles_db/"
#define map_data_offline        "/home/mkr/service/map_data/offline/"
#define map_data_offline_fonts  "/home/mkr/service/map_data/offline/fonts/"

#define map_sat_tiles_dir		"/home/mkr/service/map_data/mb_sat_tiles/"

#define UDORX_FIRMWARE_DATA_DOWNLOAD_ROOT "/home/mkr/service/firmware_download"

#endif
#endif



#endif //#ifndef UDORX_CONFIG_SERVER_H
