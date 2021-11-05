#ifndef _MAIL_ADDRESS_INCLUDED_
#define _MAIL_ADDRESS_INCLUDED_

#ifdef WIN32
#include <winsock2.h>
#include <io.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif

#include <stdint.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

class CMailAddress
{
	char m_szUser[MAX_USER_LENGTH+5];
	char m_szDomain[MAX_DOMAIN_LENGTH+5];

	char m_szAddress[MAX_ADDRESS_LENGTH+2];
	char m_szMBoxPath[300];

public:
	CMailAddress(char szAddress[]="")
	{
		strcpy(m_szAddress,szAddress);
	}
	void SetMBoxPath(char *path)
	{
		strcpy(m_szMBoxPath, path);
		printf("MBox=%s\n",m_szMBoxPath);
	}

	bool SetAddress(char szAddress[])
	{

		char *domain=m_szDomain;
		
		if(!AddressValid(szAddress)) return false;

		strcpy(m_szAddress,szAddress);
		

		domain=(char*)strchr((char*)m_szAddress,'@');
		domain+=1;

		memset(m_szUser,0,sizeof(m_szUser));
		strncpy(m_szUser,m_szAddress,strlen(m_szAddress)-strlen(domain)-1);

		return true;
	}

	char* GetMBoxPath()
	{
		//printf("--%s\n",m_szMBoxPath);
		return m_szMBoxPath;
	}
	char* GetAddress(){return m_szAddress;}
	char* GetDomain(){return m_szAddress;}
	char* GetUser(){return m_szUser;}

	static bool AddressValid(char *szAddress)
	{
		return (strlen(szAddress)>2 && strchr(szAddress,'@'));
	}
};

#endif