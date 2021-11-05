#include "MkrLib.h"
#include "udorx_server_config.h"
#include "models/models.h"

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#include <stdlib.h>
#define mbedtls_time       time
#define mbedtls_time_t     time_t 
#define mbedtls_fprintf    fprintf
#define mkr_mbedtls_printf     printf
#endif
#define mkr_mbedtls_printf    

#include "mbedtls/base64.h"
#include "mbedtls/error.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/certs.h"
#include "mbedtls/x509.h"

#include <stdlib.h>
#include <string.h>

#if !defined(_MSC_VER) || defined(EFIX64) || defined(EFI32)
#include <unistd.h>
#else
#include <io.h>
#endif

#if defined(_WIN32) || defined(_WIN32_WCE)
#include <winsock2.h>
#include <windows.h>

#if defined(_MSC_VER)
#if defined(_WIN32_WCE)
#pragma comment( lib, "ws2.lib" )
#else
#pragma comment( lib, "ws2_32.lib" )
#endif
#endif /* _MSC_VER */
#endif

#define USE_HTML	1
#define MSG_TERM    "\r\n.\r\n"
#define MAIL_AGENT  "mikerel mail"
#if USE_HTML
//	'Content-Type' = > "text/html; charset=ISO-8859-1"
#define Content_Type  "text/html; charset=ISO-8859-1"
#define attch_type "MIME-Version: 1.0\r\nContent-Type: multipart/mixed;"
#define boundary "KkK170891tpbkKk__FV_KKKkkkjjwq===="
#endif


#define MODE_SSL_TLS            0
#define MODE_STARTTLS           1
//alt4.gmail-smtp-in.l.google.com
//"smtp.gmail.com"
//#define DFL_SERVER_NAME         "alt4.gmail-smtp-in.l.google.com"
#define DFL_SERVER_PORT         "25"
//#define DFL_USER_NAME           "skulykov"
//#define DFL_USER_PWD            "ghkz1993"
//#define DFL_MAIL_FROM           "info@mikerel.com"
//#define DFL_MAIL_TO             "skulykov@gmail.com"
//#define DFL_DEBUG_LEVEL         0
#define DFL_CA_FILE             mkr_mbedtls_x509_crt_cert_file
//#define DFL_CRT_FILE            mkr_mbedtls_x509_crt_cert_file
//#define DFL_KEY_FILE            mkr_mbedtls_pk_cert_keyfile
//#define DFL_FORCE_CIPHER        0
//#define DFL_MODE                MODE_STARTTLS
//#define DFL_AUTHENTICATION      0




//static void my_debug( void *ctx, int level,
//                      const char *file, int line,
//                      const char *str )
//{
//    ((void) level);
//
//    mbedtls_fprintf( (FILE *) ctx, "%s:%04d: %s", file, line, str );
//    fflush(  (FILE *) ctx  );
//}

static int do_handshake( mbedtls_ssl_context *ssl )
{
    int ret;
    uint32_t flags;
    unsigned char buf[1024];
    memset(buf, 0, 1024);

    /*
     * 4. Handshake
     */
    mkr_mbedtls_printf( "  . Performing the SSL/TLS handshake..." );
  

    while( ( ret = mbedtls_ssl_handshake( ssl ) ) != 0 )
    {
        if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE )
        {
#if defined(MBEDTLS_ERROR_C)
            mbedtls_strerror( ret, (char *) buf, 1024 );
#endif
            mkr_mbedtls_printf( " failed\n  ! mbedtls_ssl_handshake returned %d: %s\n\n", ret, buf );
            return( -1 );
        }
    }

    mkr_mbedtls_printf( " ok\n    [ Ciphersuite is %s ]\n",
            mbedtls_ssl_get_ciphersuite( ssl ) );

    /*
     * 5. Verify the server certificate
     */
    mkr_mbedtls_printf( "  . Verifying peer X.509 certificate..." );

    /* In real life, we probably want to bail out when ret != 0 */
    if( ( flags = mbedtls_ssl_get_verify_result( ssl ) ) != 0 )
    {
        char vrfy_buf[512];

        mkr_mbedtls_printf( " failed\n" );

        mbedtls_x509_crt_verify_info( vrfy_buf, sizeof( vrfy_buf ), "  ! ", flags );

        mkr_mbedtls_printf( "%s\n", vrfy_buf );
    }
    else
        mkr_mbedtls_printf( " ok\n" );

    mkr_mbedtls_printf( "  . Peer certificate information    ...\n" );
    mbedtls_x509_crt_info( (char *) buf, sizeof( buf ) - 1, "      ",
                   mbedtls_ssl_get_peer_cert( ssl ) );
    mkr_mbedtls_printf( "%s\n", buf );

    return( 0 );
}

static int write_ssl_data( mbedtls_ssl_context *ssl, unsigned char *buf, size_t length )
{
    int ret, bytes_written=0,len=length;
	
	while (len > 0)
	{
		ret = mbedtls_ssl_write(ssl, buf + bytes_written, len);

		if (ret <= 0)
		{
			if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
			{
				mkr_mbedtls_printf(" failed\n  ! mbedtls_ssl_write returned %d\n\n", ret);
				return -1;
			}
		}
		bytes_written += ret;
		len =length - bytes_written;
		//mkr_mbedtls_printf("write_ssl_data len=%d bytes_written=%d\r\n", len, bytes_written);
	}
	//mkr_mbedtls_printf("wrote ssl data len=%d bytes_written=%d\r\n", len, bytes_written);
    return( 0 );
}

static int write_ssl_and_get_response( mbedtls_ssl_context *ssl, unsigned char *buf, size_t len )
{
    int ret;
    unsigned char data[128];
    char code[4];
    size_t i, idx = 0;

    mkr_mbedtls_printf("\n%s", buf);
    while( len && ( ret = mbedtls_ssl_write( ssl, buf, len ) ) <= 0 )
    {
        if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE )
        {
            mkr_mbedtls_printf( " failed\n  ! mbedtls_ssl_write returned %d\n\n", ret );
            return -1;
        }
    }

    do
    {
        len = sizeof( data ) - 1;
        memset( data, 0, sizeof( data ) );
        ret = mbedtls_ssl_read( ssl, data, len );

        if( ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE )
            continue;

        if( ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY )
            return -1;

        if( ret <= 0 )
        {
            mkr_mbedtls_printf( "failed\n  ! mbedtls_ssl_read returned %d\n\n", ret );
            return -1;
        }

        mkr_mbedtls_printf("\n%s", data);
        len = ret;
        for( i = 0; i < len; i++ )
        {
            if( data[i] != '\n' )
            {
                if( idx < 4 )
                    code[ idx++ ] = data[i];
                continue;
            }

            if( idx == 4 && code[0] >= '0' && code[0] <= '9' && code[3] == ' ' )
            {
                code[3] = '\0';
                return atoi( code );
            }

            idx = 0;
        }
    }
    while( 1 );
}

static int write_and_get_response( mbedtls_net_context *sock_fd, unsigned char *buf, size_t len )
{
    int ret;
    unsigned char data[128];
    char code[4];
    size_t i, idx = 0;

    mkr_mbedtls_printf("\n%s", buf);
    if( len && ( ret = mbedtls_net_send( sock_fd, buf, len ) ) <= 0 )
    {
        mkr_mbedtls_printf( " failed\n  ! mbedtls_ssl_write returned %d\n\n", ret );
            return -1;
    }

    do
    {
        len = sizeof( data ) - 1;
        memset( data, 0, sizeof( data ) );
        ret = mbedtls_net_recv( sock_fd, data, len );

        if( ret <= 0 )
        {
            mkr_mbedtls_printf( "failed\n  ! read returned %d\n\n", ret );
            return -1;
        }

        data[len] = '\0';
        mkr_mbedtls_printf("\n%s", data);
        len = ret;
        for( i = 0; i < len; i++ )
        {
            if( data[i] != '\n' )
            {
                if( idx < 4 )
                    code[ idx++ ] = data[i];
                continue;
            }

            if( idx == 4 && code[0] >= '0' && code[0] <= '9' && code[3] == ' ' )
            {
                code[3] = '\0';
                return atoi( code );
            }

            idx = 0;
        } 
    }
    while( 1 );
}
#define BUFSIZE     2048
#define BUFSIZE2    100
int write_email_content(mbedtls_ssl_context *ssl, EmailInfo *ei)
{
	int ret = 0;

	char s_buf[BUFSIZE] = { 0 };
	char s_buf2[BUFSIZE2] = { 0 };
	time_t t_now = time(NULL);

	/* First prepare the envelope */
	strftime(s_buf2, BUFSIZE2, "%a, %d %b %Y  %H:%M:%S +0000", gmtime(&t_now));
#if USE_HTML
	snprintf(s_buf, BUFSIZE, "Date: %s\r\nFrom: Mikerel Systems <%s>\r\nTo: %s\r\nSubject: %s\r\nX-Mailer: %s\r\nReply-To: %s\r\nContent-Type: %s\r\n\r\n",
		s_buf2, ei->from, ei->to, ei->subject, MAIL_AGENT, ei->replyto, Content_Type);
#else
	snprintf(s_buf, BUFSIZE, "Date: %s\r\nFrom: %s\r\nTo: %s\r\nSubject: %s\r\nX-Mailer: %s\r\nReply-To: %s\r\n\r\n",
		s_buf2, from, to, subject, MAIL_AGENT, replyto);
#endif
	mkr_mbedtls_printf("sending the envelope .. ");
	ret = write_ssl_data(ssl, (unsigned char*)s_buf, strlen(s_buf));
	if (ret < 0)
		return ret;
	mkr_mbedtls_printf("ok\r\n Now send the message .. ");
	/* Now send the message */
	ret = write_ssl_data(ssl, (unsigned char*)ei->msg, strlen(ei->msg));
	if (ret < 0)
		return ret;
	mkr_mbedtls_printf("ok\r\n Now send the terminator .. ");
	/* Now send the terminator*/
	ret = write_ssl_and_get_response(ssl, (unsigned char*)MSG_TERM, strlen(MSG_TERM));
	if (ret < 200 || ret > 299)
	{
		mkr_mbedtls_printf(" failed\n  ! server responded with %d\n\n", ret);
		return -1;
	}
	return ret;
}
int write_email_content_attachment(mbedtls_ssl_context *ssl, EmailInfo *email)
{
	int ret = 0;

	char s_buf[BUFSIZE] = { 0 };
	char s_buf2[BUFSIZE2] = { 0 };
	time_t t_now = time(NULL);
	
	mkr_mbedtls_printf("sending msg with attachment: %s\r\n", email->attachment_file_name);
	/* First prepare the envelope */
	strftime(s_buf2, BUFSIZE2, "%a, %d %b %Y  %H:%M:%S +0000", gmtime(&t_now));
#if USE_HTML
	snprintf(s_buf, BUFSIZE, "Date: %s\r\nFrom: Mikerel Systems <%s>\r\nTo: %s\r\nSubject: %s\r\nX-Mailer: %s\r\nReply-To: %s\r\n%s boundary= %s\r\n\r\n",
		s_buf2, email->from, email->to, email->subject, MAIL_AGENT, email->replyto, attch_type, boundary);
#else
	snprintf(s_buf, BUFSIZE, "Date: %s\r\nFrom: %s\r\nTo: %s\r\nSubject: %s\r\nX-Mailer: %s\r\nReply-To: %s\r\n\r\n",
		s_buf2, from, to, subject, MAIL_AGENT, replyto);
#endif
	/* Send the envelope */
	mkr_mbedtls_printf("sending the envelope .. ");
	ret = write_ssl_data(ssl, (unsigned char*)s_buf, strlen(s_buf));
	if (ret < 0)
		return ret;
	mkr_mbedtls_printf("ok\r\n Now send the message .. ");

	//1. Send first multi part ( message)
	memset(s_buf, 0, BUFSIZE);
	snprintf(s_buf, BUFSIZE, "--%s\r\nContent-Type: %s\r\n\r\n", boundary, Content_Type);
	ret = write_ssl_data(ssl, (unsigned char*)s_buf, strlen(s_buf));
	if (ret < 0)
			return ret;
	
	/* Now send the message */
	ret = write_ssl_data(ssl, (unsigned char*)email->msg, strlen(email->msg));
	if (ret < 0)
		return ret;

	ret = write_ssl_data(ssl, (unsigned char*)"\r\n\r\n", 4);
	if (ret < 0)
		return ret;
	//------------end of the first multi part-----------------
	//2. Send second multi part ( attachment)
	mkr_mbedtls_printf("2. Send second multi part ( attachment) \r\n");
	memset(s_buf, 0, BUFSIZE);
	snprintf(s_buf, BUFSIZE, "--%s\r\nContent-Type: application/octet-stream\r\nContent-Transfer-Encoding: base64\r\nContent-Disposition: attachment;filename= %s\r\n\r\n", boundary, email->attachment_file_name);
	ret = write_ssl_data(ssl, (unsigned char*)s_buf, strlen(s_buf));
	if (ret < 0)
		return ret;

	//here goes the Base64 encoded attachment CRLF
	memset(s_buf2, 0, BUFSIZE2);
	//snprintf(s_buf2, BUFSIZE2, "C:\\inetpub\\www_mkr\\mkr_files\\Templates\\%s", filename);
	snprintf(s_buf2, BUFSIZE2, "%s%s", REPORT_GEN_FILES, email->attachment_file_name);
	int length = 0;
	mkr_mbedtls_printf("2. Reading bin file ( attachment) %s \r\n", email->attachment_file_name);
	unsigned char* msga = read_bin_file(s_buf2, length);
	if (msga != 0 && length>0)
	{
		int bytes_written = 0;
		unsigned char *dst_content = new unsigned char[length * 2];
		if (!dst_content)
			return -1;
		int rv=mbedtls_base64_encode(dst_content, length * 2,(size_t*) &bytes_written, msga, length);

		delete[] msga;
		if (rv==0&& bytes_written>0)
		{
			int len = bytes_written;
			mkr_mbedtls_printf("Sending the attachment %s length=%d base64L=%d\r\n", email->attachment_file_name, length, len);
			
			ret = write_ssl_data(ssl, dst_content, len);
			if (ret < 0)
			{
				delete dst_content;
				return ret;
			}
			delete dst_content;
		}
	}
	else
	{
		m_printf("2. Could not read ( attachment) %s \r\n", email->attachment_file_name);
	}

	//send boundary when finish
	memset(s_buf2, 0, BUFSIZE2);
	snprintf(s_buf2, BUFSIZE2, "\r\n--%s--\r\n", boundary);
	ret = write_ssl_data(ssl, (unsigned char*)s_buf2, strlen(s_buf2));
	if (ret < 0)
		return -1;

	/* Now send the terminator*/
	ret = write_ssl_and_get_response(ssl, (unsigned char*)MSG_TERM, strlen(MSG_TERM));
	if (ret < 200 || ret > 299)
	{
		mkr_mbedtls_printf(" failed\n  ! server responded with %d\n\n", ret);
		return -1;
	}
	
	return 0;
}

int send_email_ssl(EmailInfo *ei)
{
    int ret = 0, len;
    mbedtls_net_context server_fd;
    unsigned char buf[1024];

    //char hostname[32];
	char *hostname="mikerel.com";
    const char *pers = "ssl_mail_client";

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt cacert;
    //mbedtls_x509_crt clicert;
    //mbedtls_pk_context pkey;
    int i;
    size_t n;
    char *p, *q;
    const int *list;

    /*
     * Make sure memory references are valid in case we exit early.
     */
    mbedtls_net_init( &server_fd );
    mbedtls_ssl_init( &ssl );
    mbedtls_ssl_config_init( &conf );
    memset( &buf, 0, sizeof( buf ) );
    mbedtls_x509_crt_init( &cacert );
   // mbedtls_x509_crt_init( &clicert );
   // mbedtls_pk_init( &pkey );
    mbedtls_ctr_drbg_init( &ctr_drbg );
  
    /*
     * 0. Initialize the RNG and the session data
     */
    mkr_mbedtls_printf( "\n  . Seeding the random number generator..." );
  

    mbedtls_entropy_init( &entropy );
    if( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
                               (const unsigned char *) pers,
                               strlen( pers ) ) ) != 0 )
    {
        mkr_mbedtls_printf( " failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret );
        goto exit;
    }

    mkr_mbedtls_printf( " ok\n" );

    /*
     * 1.1. Load the trusted CA
     */
	mkr_mbedtls_printf("  . Loading the CA root certificate ...");
	

#if defined(MBEDTLS_FS_IO)
	
		ret = mbedtls_x509_crt_parse_file(&cacert, mkr_mbedtls_x509_crt_cert_file);
	
#endif
	if (ret < 0)
	{
		mkr_mbedtls_printf(" failed\n  !  mbedtls_x509_crt_parse returned %d\n\n", ret);
		goto exit;
	}

	mkr_mbedtls_printf(" ok (%d skipped)\n", ret);

    /*
     * 2. Start the connection
     */
    mkr_mbedtls_printf( "  . Connecting to tcp/%s/%s...", /*opt.server_name*/ei->smtpserver, /*opt.server_port*/DFL_SERVER_PORT);
 

    if( ( ret = mbedtls_net_connect( &server_fd, ei->smtpserver,
		DFL_SERVER_PORT, MBEDTLS_NET_PROTO_TCP ) ) != 0 )
    {
        mkr_mbedtls_printf( " failed\n  ! mbedtls_net_connect returned %d\n\n", ret );
        goto exit;
    }

    mkr_mbedtls_printf( " ok\n" );

    /*
     * 3. Setup stuff
     */
    mkr_mbedtls_printf( "  . Setting up the SSL/TLS structure..." );
  

    if( ( ret = mbedtls_ssl_config_defaults( &conf,
                    MBEDTLS_SSL_IS_CLIENT,
                    MBEDTLS_SSL_TRANSPORT_STREAM,
                    MBEDTLS_SSL_PRESET_DEFAULT ) ) != 0 )
    {
        mkr_mbedtls_printf( " failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret );
        goto exit;
    }

    /* OPTIONAL is not optimal for security,
     * but makes interop easier in this simplified example */
    mbedtls_ssl_conf_authmode( &conf, MBEDTLS_SSL_VERIFY_OPTIONAL );

    mbedtls_ssl_conf_rng( &conf, mbedtls_ctr_drbg_random, &ctr_drbg );
   // mbedtls_ssl_conf_dbg( &conf, my_debug, stdout );

   /* if( opt.force_ciphersuite[0] != DFL_FORCE_CIPHER )
        mbedtls_ssl_conf_ciphersuites( &conf, opt.force_ciphersuite );*/

    mbedtls_ssl_conf_ca_chain( &conf, &cacert, NULL );
    /*if( ( ret = mbedtls_ssl_conf_own_cert( &conf, &clicert, &pkey ) ) != 0 )
    {
        mkr_mbedtls_printf( " failed\n  ! mbedtls_ssl_conf_own_cert returned %d\n\n", ret );
        goto exit;
    }*/

    if( ( ret = mbedtls_ssl_setup( &ssl, &conf ) ) != 0 )
    {
        mkr_mbedtls_printf( " failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret );
        goto exit;
    }

    if( ( ret = mbedtls_ssl_set_hostname( &ssl, /*opt.server_name*/ei->smtpserver ) ) != 0 )
    {
        mkr_mbedtls_printf( " failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret );
        goto exit;
    }

    mbedtls_ssl_set_bio( &ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL );

    mkr_mbedtls_printf( " ok\n" );

    {
        mkr_mbedtls_printf( "  > Get header from server:" );
      

        ret = write_and_get_response( &server_fd, buf, 0 );
        if( ret < 200 || ret > 299 )
        {
            mkr_mbedtls_printf( " failed\n  ! server responded with %d\n\n", ret );
            goto exit;
        }

        mkr_mbedtls_printf(" ok\n" );

        mkr_mbedtls_printf( "  > Write EHLO to server:" );
      

       // gethostname( hostname, 32 );
        len = sprintf( (char *) buf, "EHLO %s\r\n", hostname );
        ret = write_and_get_response( &server_fd, buf, len );
        if( ret < 200 || ret > 299 )
        {
            mkr_mbedtls_printf( " failed\n  ! server responded with %d\n\n", ret );
            goto exit;
        }

        mkr_mbedtls_printf(" ok\n" );

        mkr_mbedtls_printf( "  > Write STARTTLS to server:" );
     

       // gethostname( hostname, 32 );
        len = sprintf( (char *) buf, "STARTTLS\r\n" );
        ret = write_and_get_response( &server_fd, buf, len );
        if( ret < 200 || ret > 299 )
        {
            mkr_mbedtls_printf( " failed\n  ! server responded with %d\n\n", ret );
            goto exit;
        }

        mkr_mbedtls_printf(" ok\n" );

        if( do_handshake( &ssl ) != 0 )
            goto exit;
    }

    mkr_mbedtls_printf( "  > Write MAIL FROM to server:" );
  

    len = sprintf( (char *) buf, "MAIL FROM:<%s>\r\n", /*opt.mail_from*/ei->from );
    ret = write_ssl_and_get_response( &ssl, buf, len );
    if( ret < 200 || ret > 299 )
    {
        mkr_mbedtls_printf( " failed\n  ! server responded with %d\n\n", ret );
        goto exit;
    }

    mkr_mbedtls_printf(" ok\n" );

    mkr_mbedtls_printf( "  > Write RCPT TO to server:" );
   

    len = sprintf( (char *) buf, "RCPT TO:<%s>\r\n", /*opt.mail_to*/ei->to );
    ret = write_ssl_and_get_response( &ssl, buf, len );
    if( ret < 200 || ret > 299 )
    {
        mkr_mbedtls_printf( " failed\n  ! server responded with %d\n\n", ret );
        goto exit;
    }

    mkr_mbedtls_printf(" ok\n" );

    mkr_mbedtls_printf( "  > Write DATA to server:" );
  

    len = sprintf( (char *) buf, "DATA\r\n" );
    ret = write_ssl_and_get_response( &ssl, buf, len );
    if( ret < 300 || ret > 399 )
    {
        mkr_mbedtls_printf( " failed\n  ! server responded with %d\n\n", ret );
        goto exit;
    }

    mkr_mbedtls_printf(" ok\n" );

	if (ei->attachment_file_name != 0)
	{
		if (write_email_content_attachment(&ssl, ei) < 0)
			goto exit;
	}
	else
	{
		if (write_email_content(&ssl, ei) < 0)
			goto exit;
	}
    mbedtls_ssl_close_notify( &ssl );

exit:

    mbedtls_net_free( &server_fd );
//    mbedtls_x509_crt_free( &clicert );
    mbedtls_x509_crt_free( &cacert );
//    mbedtls_pk_free( &pkey );
    mbedtls_ssl_free( &ssl );
    mbedtls_ssl_config_free( &conf );
    mbedtls_ctr_drbg_free( &ctr_drbg );
    mbedtls_entropy_free( &entropy );

#if defined(_WIN32)
    mkr_mbedtls_printf( "  + Press Enter to exit this program.\n" );
    fflush( stdout ); getchar();
#endif

    return( ret );
}



