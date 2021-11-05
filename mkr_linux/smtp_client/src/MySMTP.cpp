// MySMTP.cpp : Defines the entry point for the console application.
//

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



int main_test_sign(/*int argc, char *argv[]*/);
int main_send_register_email(char *mailTo);
int main(int argc, char* argv[])
{    
	
	//make_connection();
#if TEST_PDKIM
	main_test_sign();
	main_test_verify();
#endif
	//printf("press q to exit\r\n");
	int key = 0;
	//while (1)
	{
		main_send_register_email("skulykov@gmail.com");
/*
		if (_kbhit()) {
			key = _getch();
			if (key == 'q')
			{
			
				break;
			}
			if (key == 'g')
			{
				main_send_email("skulykov@gmail.com");
			}
			else if (key == 't')
			{
				main_send_email("Sergey.KULIKOV@external.thalesgroup.com"); 
			}
			else if (key == 'y')
			{
				main_send_email("Sergey.KULYKOV@yahoo.com");
			}
			else {
				//gChar = key;
			}

		}
		*/
	}
	return 0;
}
