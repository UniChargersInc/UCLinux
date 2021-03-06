#ifndef _MAIL_SESSION_INCLUDED_
#define _MAIL_SESSION_INCLUDED_

#include "mailaddress.h"

#ifdef WIN32
#include <winsock2.h>
#include <io.h>
#else
#define MAX_PATH  64
#define INVALID_SOCKET  -1
#define SOCKET_ERROR    -1

#define HANDLE int
#define SOCKET int
#endif
#ifndef WIN32 //_IPHONE <- your platform define here
#define stricmp strcasecmp
#define strnicmp strncasecmp
#endif

extern char g_szDirectoryPath[300];

#define DIRECTORY_ROOT g_szDirectoryPath

class CMailSession
{
public:
	char m_szFileName[MAX_PATH+1];

	void *m_pszData;
	int data_len;

	HANDLE m_hMsgFile;

	unsigned int m_nStatus;
	int m_nRcptCount;
	CMailAddress m_FromAddress, m_ToAddress[MAX_RCPT_ALLOWED+1];


	SOCKET m_socConnection;

	CMailSession(SOCKET client_soc)
	{
		m_nStatus=SMTP_STATUS_INIT;
		m_socConnection=client_soc;
		m_pszData=NULL;
		data_len=0;
		m_nRcptCount=0;
	}
	

private:
	int ProcessHELO(char *buf, int len);
	int ProcessRCPT(char *buf, int len);
	int ProcessMAIL(char *buf, int len);
	int ProcessRSET(char *buf, int len);
	int ProcessNOOP(char *buf, int len);
	int ProcessQUIT(char *buf, int len);
	int ProcessDATA(char *buf, int len);
	int ProcessNotImplemented(bool bParam=false);

public:

	int ProcessCMD(char *buf, int len);
	int SendResponse(int nResponseType);
	int ProcessDATAEnd(void);
	bool CreateNewMessage(void);
};
#endif //_MAIL_SESSION_INCLUDED_