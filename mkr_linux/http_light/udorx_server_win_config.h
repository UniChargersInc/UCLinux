#ifndef UDORX_CONFIG_WIN_SERVER_H
#define UDORX_CONFIG_WIN_SERVER_H

#define UDORX_PROD_SERVER  0
#define LOCALHOST_WINDOWS_SERVER  1
#define RUN_ON_WIN_SERVER 0              // 0 - run locally 

#define MKR_USE_SSL	0


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
// C:\Windows\System32\drivers\etc\hosts     -> add entry  192.168.0.20       mkrwinhost
//#define HTTP_LIGHT_SERVER_IPADDR  "127.0.0.1"
#define HTTP_LIGHT_SERVER_IPADDR "192.168.0.20"

#define TCP_LISTENER_IPADDR HTTP_LIGHT_SERVER_IPADDR

#if MKR_USE_SSL
#define HTTPS_LIGHT_SERVER_PORT 4433
#else
#define HTTP_LIGHT_SERVER_PORT 80
#endif
  
#if MKR_USE_SSL
#define UDORX_URL_AFTER_LOGIN	"https://192.168.0.20:4433/mkr.html"
#define LOGOUT_URL	  "https://192.168.0.20:4433"  
#else
#define UDORX_URL_AFTER_LOGIN	"http://192.168.0.20:80/mkr.html"
#define LOGOUT_URL	  "http://192.168.0.20:80"  

//#define UDORX_URL_AFTER_LOGIN	"http://localhost:80/mkr.html"
//#define LOGOUT_URL	  "http://localhost:80"  

//#define UDORX_URL_AFTER_LOGIN	"http://mkrwinhost:80/mkr.html"
//#define LOGOUT_URL	  "http://mkrwinhost:80"  

#define UDORX_ADMIN_URL_AFTER_LOGIN	      "http://192.168.0.20:80/adm.html"
#define UDORX_FIRMWARE_DATA_DOWNLOAD_ROOT "C:\\My\\UC\\UCLinux\\download_firmware"


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


#define HTTPS_LIGHT_SERVER_PORT 443

#define Mkr_RootDir  "C:\\My\\UC\\UCLinux\\www"

//#define Udorx_RootDir  "C:\\My\\UC\\UCLinux\\www\\udorx"
#define DB_ROOT_DIR	"c:\\My\\UC\\UCLinux\\win_sqlite_db\\acharger\\"
#define MEDIA_DIRECTORY	"C:\\My\\UC\\UCLinux\\www/media/"
#define MEDIA_DVIR	"C:\\My\\UC\\UCLinux\\www/media/dvir/"
#define MEDIA_VEH	"C:\\My\\UC\\UCLinux\\www/media/v_img/"
#define WEB_RES_IMG_DIR "C:\\My\\UC\\UCLinux\\www/media/resImg/" 
#define WEB_TEMPLETES	"C:\\My\\UC\\UCLinux\\www\\mkr_files\\Templates\\"

//#define WEB_QRCODE_IMG_DIR "C:\\My\\UC\\UCLinux\\www/media/qrcodes/" 
#define WEB_QRCODE_IMG_DIR  "C:\\SmartBIN\\QRs/"

#define WEB_LABEL_IMG_DIR "C:\\My\\UC\\UCLinux\\www\\media\\label_logos\\"

//#define MBGL_MAP_DATA	"/home/mkr/www/mbgl/map_data/"
#define UDORX_MAP_DATA_DOWNLOAD_ROOT	"D:\\d_mb_tilemaker\\map_data_download\\"

//#define REPORT_GEN_FILES	"C:\\My\\UC\\UCLinux\\logs\\"
#define REPORT_GEN_FILES	"C:\\SmartBIN\\PDF\\"
#define mkr_mbedtls_x509_crt_cert_file  "C:\\My\\UC\\UCLinux\\SSL_Certificate\\localhost.crt"
#define mkr_mbedtls_pk_cert_keyfile  "C:\\My\\UC\\UCLinux\\SSL_Certificate\\localhost.key"

#define MKR_SSL_PORT	"443"
#define MKR_SSL_HOST_ADDR NULL


#define MKR_USE_INTERNAL_MAP_TILES_SERVICE	1

#if MKR_USE_INTERNAL_MAP_TILES_SERVICE

#define map_tiles_dir			"C:\\My\\map_data\\tiles\\"
#define map_tiles_07_dir		"C:\\My\\map_data\\tiles\\"
#define map_data_offline        "C:\\My\\map_data\\offline\\"
#define map_data_offline_fonts        "C:\\My\\map_data\\offline\\fonts\\"

#define map_sat_tiles_dir		"D:\\d_mb_tilemaker\\mb_sat_tiles\\"


#endif

#endif //#ifndef UDORX_CONFIG_SERVER_H
