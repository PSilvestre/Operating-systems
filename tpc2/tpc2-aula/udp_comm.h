#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#ifndef _UDP_COMM_
#define _UDP_COMM_

int UDP_Open(int port);

int UDP_FillSockAddr(struct sockaddr_in *addr, 
		char* hostName, int port);
		
int UDP_Write(int sd, struct sockaddr_in *addr, 
	char* buffer, int n);

int UDP_Read(int sd, struct sockaddr_in *addr, 
	char* buffer, int n);


#endif