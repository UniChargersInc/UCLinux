//http://stackoverflow.com/questions/1093410/pulling-mx-record-from-dns-server
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MkrLib.h"



#if MKR_LINUX
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/nameser.h> 
#include <resolv.h>

#include <my_lib/my_lib.h>         // my implementation of strdup  !!!

char* get_mx_record(char *domain)
{
	u_char nsbuf[4096];
//	char dispbuf[4096];
	ns_msg msg;
	ns_rr rr;
	int i, j, l;

//http://www.sourcexr.com/articles/2013/10/12/dns-records-query-with-res_query	
//http://docstore.mik.ua/orelly/networking_2ndEd/dns/ch15_02.htm

//http://stackoverflow.com/questions/1688432/querying-mx-record-in-c-linux   -> example to get MX record
/*
and then Compile it as 
gcc <pname.c> /usr/lib/libresolv.a   (instead of gcc pname.c -lresolv)     !!!!!!!!!!!!! When compile with GCC4.2 with -lresolv I got "unresolved reference to __ns_initparse  ???

BUT with GCC 5.4  -lresolv works OK !!! 
*/	
	l = res_search(domain, C_IN, T_MX, nsbuf, sizeof(nsbuf));    //res_query(domain, ns_c_any, ns_t_mx, nsbuf, sizeof(nsbuf));
		if (l < 0) {
			perror(domain);
		}
		else {
#ifdef USE_PQUERY
			/* this will give lots of detailed info on the request and reply */
			res_pquery(&_res, nsbuf, l, stdout);
#else
			/* just grab the MX answer info */
			ns_initparse(nsbuf, l, &msg);
			//m_printf("%s :\n", domain);
			l = ns_msg_count(msg, ns_s_an);
			for (j = 0; j < l; j++) {
				if(ns_parserr(&msg, ns_s_an, j, &rr))
					continue;
				//ns_sprintrr(&msg, &rr, NULL, NULL, dispbuf, sizeof(dispbuf));
				//string ss = string(dispbuf);
				//m_printf("%s\n", dispbuf);
				if (ns_rr_class(rr) == ns_c_in && ns_rr_type(rr) == ns_t_mx) {
					char mxname[/*MAXDNAME*/128];
					dn_expand(ns_msg_base(msg), ns_msg_base(msg) + ns_msg_size(msg), ns_rr_rdata(rr) + NS_INT16SZ, mxname, sizeof(mxname));
					return mkr_my_strdup(mxname);
					/*string ss = string(mxname);
					if (!ss.empty())
					{
					}*/
				}

			}
#endif
		}
	

	return 0;
}
#else
#include <windows.h>
#include <windns.h> 
/*
#pragma comment(lib,"Iphlpapi.lib") 
#pragma comment(lib,"dnsapi.lib")
#pragma comment(lib,"ws2_32.lib") 
*/
//•Dnsapi.lib
PSTR  get_mx_record(char *domain)
{
	IN_ADDR ipaddr;
	PDNS_RECORD ppQueryResultsSet, p;
	PIP4_ARRAY pSrvList = NULL;
	char temp[MAX_PATH];
	int iRecord = 0;

	//http://www.codeproject.com/Articles/3720/Getting-an-MX-record-the-easy-way
	DNS_STATUS dnsStatus=DnsQuery(/*"google.com"*/domain,
		DNS_TYPE_MX,
		DNS_QUERY_STANDARD,//DNS_QUERY_BYPASS_CACHE,
		NULL,//pSrvList, // Documented as reserved, but can take a PIP4_ARRAY for the DNS server
		&ppQueryResultsSet,
		NULL);
	PSTR _HostName=NULL;
	if (dnsStatus == ERROR_SUCCESS)
	{
		// Found the SMTP Host Name, insert it into our map
		if (ppQueryResultsSet->Data.MX.pNameExchange != NULL)
		{
			int len = strlen(ppQueryResultsSet->Data.MX.pNameExchange);
			if (len > 0)
			{
				_HostName = new CHAR[len + 1];
				strcpy(_HostName,	ppQueryResultsSet->Data.MX.pNameExchange);
			}
		}
		if (_HostName)
		{

		}
		//m_SMTPHost.insert(HostMapValue(strDomain, _HostName));
	}


	if (dnsStatus)
	{

		printf("\nDNSQuery failed and returned %d, GLE = %d \r\n", dnsStatus, GetLastError());
		return NULL;
	}

	p = ppQueryResultsSet;

	while (p) // Loop through the returned addresses
	{

		iRecord++;
		printf("\nRecord #%d\r\n", iRecord);

		ipaddr.S_un.S_addr = (p->Data.A.IpAddress);
		printf("The IP address of %s is %s \n", p->pName, inet_ntoa(ipaddr));
		printf("TTL: %d(secs)\n", p->dwTtl);

		p = p->pNext;
	}

	if (pSrvList) LocalFree(pSrvList);

	DnsRecordListFree(ppQueryResultsSet, DnsFreeRecordList);

	return _HostName;
}


/*
* Routines for  Windows Socket Library
*/
int startup_sockets_lib(void)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int n_err;

	wVersionRequested = MAKEWORD(1, 1);

	n_err = WSAStartup(wVersionRequested, &wsaData);
	if (n_err != 0) {
		//m_printf("Could not find winsock.dll.Aborting...\n");
		return FALSE;
	}

	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1) {
		//m_printf("Could not find w1.1 version of winsock.dll.Aborting...\n");
		WSACleanup();
		return FALSE;
	}

	return TRUE;
}

int cleanup_sockets_lib(void)
{
	WSACleanup();
	return TRUE;
}



#endif