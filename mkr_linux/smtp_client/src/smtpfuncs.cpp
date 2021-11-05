#include <stdio.h>
#include <string.h>
#include <time.h>
#include "smtpfuncs.h"
#include "MkrLib.h"
#include "udorx_server_config.h"
#include "../pdkim/src/pdkim.h"

int dkim_sign_and_send(int n_sock, char *s_buf);
//#define USE_ATTACHMENTS	1
//int send_mail(const char *smtpserver, const char *from, const char *to,
//	const char *subject, const char *replyto, const char *msg)
int send_mail(EmailInfo *email)
{
	int n_socket;
	int n_retval = 0;
	
#ifdef WIN32
	startup_sockets_lib();
#endif

	/* First connect the socket to the SMTP server */
	if ((n_socket = connect_to_server(email->smtpserver)) == ERROR)
	{
		n_retval = E_NO_SOCKET_CONN;
		m_printf("Could not connect to server..goto Exit\r\n");
		goto __Exit;
	}
	/* All connected. Now send the relevant commands to initiate a mail transfer */
	m_printf("All connected. Now send the relevant commands to initiate a mail transfer\r\n");
	
	if (n_retval == 0 && send_command(n_socket, "HELO ","mail.mikerel.com","\r\n", MAIL_OK) == ERROR)
			n_retval = E_PROTOCOL_ERROR;
	if (n_retval == 0 && send_command(n_socket, "MAIL From:<", email->from, ">\r\n", MAIL_OK) == ERROR)
		n_retval = E_PROTOCOL_ERROR;
	if (n_retval == 0 && send_command(n_socket, "RCPT To:<", email->to, ">\r\n", MAIL_OK) == ERROR)
		n_retval = E_PROTOCOL_ERROR;

	/* Now send the actual message */
	if (n_retval == 0 && send_command(n_socket, "", "DATA", "\r\n", MAIL_GO_AHEAD) == ERROR)
		n_retval = E_PROTOCOL_ERROR;
//#if USE_ATTACHMENTS
	if (email->attachment_file_name)
	{
		//fname = "hpdf_table.pdf";// "rpt_17705_2016102622429322.xlsx";
		if (n_retval == 0 && send_mail_message_with_attachment(n_socket, email) == ERROR)
			n_retval = E_PROTOCOL_ERROR;
	}
//#else
	else
	{
		if (n_retval == 0 && send_mail_message(n_socket, email) == ERROR)
			n_retval = E_PROTOCOL_ERROR;
	}
//#endif
	/* Now tell the mail server that we're done */
	if (n_retval == 0 && send_command(n_socket, "", "QUIT", "\r\n", MAIL_GOODBYE) == ERROR)
		n_retval = E_PROTOCOL_ERROR;

	/* Now close up the socket and clean up */
	m_printf("Now close up the socket and clean up\r\n");
	try {
		if (my_closesocket(n_socket) == ERROR) {                         //closesocket
			m_printf("Could not close socket.\n");
			n_retval = ERROR;
		}
	}
	catch (...)
	{
		m_printf("EXC when close up the socket and clean up\r\n");
	}

__Exit:
	m_printf("Now cleanup_sockets_lib()..p\r\n");
#ifdef WIN32
	cleanup_sockets_lib();
#endif

	return n_retval;
}

int connect_to_server(const char *server)
{
	struct hostent *host;
	struct in_addr  inp;
	struct protoent *proto;
	struct sockaddr_in sa;
	int n_sock;
#define SMTP_PORT      25
#define BUFSIZE     4096
	char s_buf[BUFSIZE] = { 0 };
	int n_ret;

	/* First resolve the hostname */
	host = gethostbyname(server);
	if (host == NULL) {
		m_printf("Could not resolve hostname %s. Aborting...\r\n", server);
		return ERROR;
	}

	memcpy(&inp, host->h_addr_list[0], host->h_length);

	/* Next get the entry for TCP protocol */
	if ((proto = getprotobyname("tcp")) == NULL) {
		m_printf("Could not get the protocol for TCP. Aborting...\r\n");
		return ERROR;
	}

	/* Now create the socket structure */
	if ((n_sock = socket(PF_INET, SOCK_STREAM, proto->p_proto)) == INVALID_SOCKET) {
		m_printf("Could not create a TCP socket. Aborting...\r\n");
		return ERROR;
	}

	/* Now connect the socket */
	memset(&sa, 0, sizeof(sa));
	sa.sin_addr = inp;
	sa.sin_family = host->h_addrtype;
	sa.sin_port = htons(SMTP_PORT);
	if (connect(n_sock, (struct sockaddr *)&sa, sizeof(sa)) == SOCKET_ERROR) {
		m_printf("Connection refused by host %s. \r\n", server);
		return ERROR;
	}

	/* Now read the welcome message */
	m_printf("Now read the welcome message. \r\n");
	n_ret = recv(n_sock, s_buf, BUFSIZE, 0);
	m_printf("Now read the welcome message. len=%d msg:%s \r\n", n_ret,s_buf);
	return n_sock;
}

int send_command(int n_sock, const char *prefix, const char *cmd,
	const char *suffix, int ret_code)
{
#define BUFSIZE     4096
	char s_buf[BUFSIZE] = { 0 };
	char s_buf2[50] = { 0 };

	strncpy(s_buf, prefix, BUFSIZE);
	strncat(s_buf, cmd, BUFSIZE);
	strncat(s_buf, suffix, BUFSIZE);

	m_printf("Sending command.. %s\r\n",s_buf);
	if (send(n_sock, s_buf, strlen(s_buf), 0) == SOCKET_ERROR) {
		m_printf( "Could not send command string %s to server.", s_buf);
		return ERROR;
	}

	/* Now read the response. */
	m_printf("Now read the response... \r\n");
	memset(s_buf, 0, BUFSIZE);
	int len=recv(n_sock, s_buf, BUFSIZE, 0);
	if (len > 0)
	{
		m_printf("GOT the response = %s \r\n", s_buf);
		/* Now check if the ret_code is in the buf */
		sprintf(s_buf2, "%d", ret_code);

		if (strstr(s_buf, s_buf2) != NULL)
			return TRUE;
	}
	//else
		return ERROR;
}

#define USE_HTML	1
//<The message data (body text, subject, e-mail header, attachments etc) is sent>
int send_mail_message(int n_sock, EmailInfo *email)
{
#define BUFSIZE     4096
#define BUFSIZE2    100
#define MSG_TERM    "\r\n.\r\n"
#define MAIL_AGENT  "mikerel mail"
#if USE_HTML
//	'Content-Type' = > "text/html; charset=ISO-8859-1"
#define Content_Type  "text/html; charset=ISO-8859-1"
#endif
	char s_buf[BUFSIZE] = { 0 };
	char s_buf2[BUFSIZE2] = { 0 };
	time_t t_now = time(NULL);
	int n_ret;

	/* First prepare the envelope */
	strftime(s_buf2, BUFSIZE2, "%a, %d %b %Y  %H:%M:%S +0000", gmtime(&t_now));
#if USE_HTML
	snprintf(s_buf, BUFSIZE, "Date: %s\r\nFrom: Mikerel Systems <%s>\r\nTo: %s\r\nSubject: %s\r\nX-Mailer: %s\r\nReply-To: %s\r\nContent-Type: %s\r\n\r\n",
		s_buf2, email->from, email->to, email->subject, MAIL_AGENT, email->replyto, Content_Type);
#else
	snprintf(s_buf, BUFSIZE, "Date: %s\r\nFrom: %s\r\nTo: %s\r\nSubject: %s\r\nX-Mailer: %s\r\nReply-To: %s\r\n\r\n",
		s_buf2, from, to, subject, MAIL_AGENT, replyto);
#endif
	/* Send the envelope */
	/*if (send(n_sock, s_buf, strlen(s_buf), 0) == SOCKET_ERROR) {
		m_printf("Could not send message header: %s", s_buf);
		return ERROR;
	}*/
	int rv = dkim_sign_and_send(n_sock, s_buf);
	if (rv < 0)
	{
		m_printf("Could not send dkim envelope: %s\r\n", s_buf);
		return ERROR;
	}
	/* Now send the message */
	if (send(n_sock, email->msg, strlen(email->msg), 0) == SOCKET_ERROR) {
		m_printf("Could not send the message %s\n", email->msg);
		return ERROR;
	}

	/* Now send the terminator*/
	if (send(n_sock, MSG_TERM, strlen(MSG_TERM), 0) == SOCKET_ERROR) {
		m_printf("Could not send the message terminator.\r\n");
		return ERROR;
	}

	m_printf("send_mail_message sent ->Read and discard the returned message ID\r\n");
	/* Read and discard the returned message ID */
	memset(s_buf, 0, BUFSIZE);
	n_ret = recv(n_sock, s_buf, BUFSIZE, 0);
	m_printf("send_mail_message sent ->Response: len=%d  res:%s\r\n",n_ret, s_buf);
	return TRUE;
}

#define BUFSIZE     4096
#define BUFSIZE2    100
#define MSG_TERM    "\r\n.\r\n"
#define MAIL_AGENT  "mikerel mail"
#if USE_HTML
//	'Content-Type' = > "text/html; charset=ISO-8859-1"
#define Content_Type  "text/html; charset=ISO-8859-1"
#define attch_type "MIME-Version: 1.0\r\nContent-Type: multipart/mixed;"
#define boundary "KkK170891tpbkKk__FV_KKKkkkjjwq===="
#endif
static char s_buf[BUFSIZE] ;
static char s_buf2[BUFSIZE2] ;

static void reset_buffers()
{
	memset(s_buf2, 0, BUFSIZE2);
	memset(s_buf, 0, BUFSIZE);
}
int send_mail_message_with_attachment(int n_sock, EmailInfo *email)
{
	reset_buffers();
	time_t t_now = time(NULL);
	int n_ret;
	m_printf("sending msg with attachment: %s\r\n", email->attachment_file_name);
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
	/*if (send(n_sock, s_buf, strlen(s_buf), 0) == SOCKET_ERROR) {
	m_printf("Could not send message header: %s", s_buf);
	return ERROR;
	}*/
	int rv = dkim_sign_and_send(n_sock, s_buf);
	if (rv < 0)
	{
		m_printf("Could not send dkim envelope: %s\r\n", s_buf);
		return ERROR;
	}
	m_printf("dkim envelope: Sent\r\n");
	//1. Send first multi part ( message)
	memset(s_buf, 0, BUFSIZE);
	snprintf(s_buf, BUFSIZE, "--%s\r\nContent-Type: %s\r\n\r\n", boundary, Content_Type);
	if (send(n_sock, s_buf, strlen(s_buf), 0) == SOCKET_ERROR) {
		m_printf("Could not send the message header %s\n", email->msg);
		return ERROR;
	}
	/* Now send the message */
	m_printf("Now send the message ..\r\n");
	if (send(n_sock, email->msg, strlen(email->msg), 0) == SOCKET_ERROR) {
		m_printf("Could not send the message %s\n", email->msg);
		return ERROR;
	}
	if (send(n_sock, "\r\n\r\n", 4, 0) == SOCKET_ERROR) {
		m_printf("Could not send the end of the message %s\n", email->msg);
		return ERROR;
	}
	//------------end of the first multi part-----------------
	//2. Send second multi part ( attachment)
	m_printf("2. Send second multi part ( attachment) \r\n");
	memset(s_buf, 0, BUFSIZE);
	snprintf(s_buf, BUFSIZE, "--%s\r\nContent-Type: application/octet-stream\r\nContent-Transfer-Encoding: base64\r\nContent-Disposition: attachment;filename= %s\r\n\r\n", boundary, email->attachment_file_name);
	if (send(n_sock, s_buf, strlen(s_buf), 0) == SOCKET_ERROR) {
		m_printf("Could not send the message header %s\n", email->msg);
		return ERROR;
	}

	//here goes the Base64 encoded attachment CRLF
	memset(s_buf2, 0, BUFSIZE2);
	//snprintf(s_buf2, BUFSIZE2, "C:\\inetpub\\www_mkr\\mkr_files\\Templates\\%s", filename);
	snprintf(s_buf2, BUFSIZE2, "%s%s", REPORT_GEN_FILES, email->attachment_file_name);
	int length = 0;
	m_printf("2. Reading bin file ( attachment) %s \r\n", email->attachment_file_name);
	unsigned char* msga = read_bin_file(s_buf2,length);
	if (msga != 0&&length>0)
	{
		char *content=pdkim_encode_base64((char*)msga, length);
		delete[] msga;
		if (content != 0)
		{
			int len = strlen(content);
			m_printf("Sending the attachment %s length=%d base64L=%d\r\n", email->attachment_file_name,length,len);
			if (send(n_sock, content, len, 0) == SOCKET_ERROR) {
				m_printf("Could not send the attachment %s\n", email->attachment_file_name);
				free(content);
				return ERROR;
			}
			free(content);
		}
	}
	else
	{
		m_printf("2. Could not read ( attachment) %s \r\n", email->attachment_file_name);
	}



	//send boundary when finish
	memset(s_buf2, 0, BUFSIZE2);
	snprintf(s_buf2, BUFSIZE2, "\r\n--%s--\r\n", boundary);
	if (send(n_sock, s_buf2, strlen(s_buf2), 0) == SOCKET_ERROR) {
		m_printf("Could not send the FINISH %s\n", email->msg);
		return ERROR;
	}

	/* Now send the terminator*/
	if (send(n_sock, MSG_TERM, strlen(MSG_TERM), 0) == SOCKET_ERROR) {
		m_printf("Could not send the message terminator.\n");
		return ERROR;
	}

	m_printf("send_mail_message sent ->Read and discard the returned message ID\r\n");
	/* Read and discard the returned message ID */
	memset(s_buf, 0, BUFSIZE);
	n_ret = recv(n_sock, s_buf, BUFSIZE, 0);
	m_printf("send_mail_message sent ->Response: len=%d  res:%s\r\n", n_ret, s_buf);
	return TRUE;
}

