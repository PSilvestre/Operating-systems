#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <pthread.h>

#include "udp_comm.h"
#include "server_functions.h"

#define BUFSIZE 1024
#define MAX_USERS 64

pthread_mutex_t mtx=PTHREAD_MUTEX_INITIALIZER;
int sock;

struct requestInformation {
	struct sockaddr_in* clientAddr;
	char* request;
};

struct requestInformation* newRequestInformation() {
	struct requestInformation* req = malloc (sizeof(struct requestInformation));
	req->clientAddr = malloc(sizeof(struct sockaddr_in));
	req->request = malloc(sizeof(char) * BUFSIZE);
	return req;
}

void freeRequestInformation(struct requestInformation* req) {
	/*free(req->clientAddr);
	free(req->request);*/
	free(req);
}


void* threadRoutine(void* arg){
	struct requestInformation* req;
	req = (struct requestInformation*) arg;
	char* saveptr;
	char* command = strtok_r((*req).request," ", &saveptr);
	char* end1;
	char* end2;
	char* result = malloc(BUFSIZE*sizeof(char));

	if(strcmp(command, "FunctionA") == 0){
		char* argPointer = strtok_r(NULL, " ", &saveptr);
		int arg1 = strtol(argPointer, &end1, 10);
		if(strtok_r(NULL, " ", &saveptr) != NULL){
			result = "Invalid operation";
		}else{
			functionA(arg1, result);
		}
	}else if(strcmp(command, "FunctionB") == 0){
		pthread_mutex_lock(&mtx);
		char* argPointer1 = strtok_r(NULL, " ", &saveptr);
		char* argPointer2 = strtok_r(NULL, " ", &saveptr);
		int arg1 = strtol(argPointer1, &end1, 10);
		int arg2 = strtol(argPointer2, &end2, 10);
		if(strtok_r(NULL, " ", &saveptr) != NULL){
			result = "Invalid operation";
		}else{
			functionB(arg1, arg2, result);
		}
		pthread_mutex_unlock(&mtx);
	}else if(strcmp(command, "FunctionC") == 0){
		char* arg1 = malloc(BUFSIZE*sizeof(char));
		strcpy(arg1, ((*req).request+10));
		if(strlen(arg1) <= 1){
			result = "Invalid operation";
		}else{
			functionC(arg1, result);
		}
	}else{
		result = "Invalid operation";
	}
	UDP_Write(sock, (*req).clientAddr, result, strlen(result) + 1);
	freeRequestInformation(req);
	free(result);
	pthread_exit(NULL);
}

int server(int port){
	sock = UDP_Open(port);
	struct requestInformation* req;
	pthread_t newThread;
	//Main cycle of the server.
	while(1) {
		//Prepara as estruturas em memória para receber um novo pedido
		req = newRequestInformation();
		UDP_Read(sock, (*req).clientAddr, (*req).request, BUFSIZE);
		pthread_create(&newThread, NULL, threadRoutine, (void*)req);
		pthread_detach(newThread);
	}
	return 0;
}

int main( int argc, char *argv[]){
	int port;

	if( argc == 2){
		port = atoi(argv[1]);
	} else {
		printf("Usage: %s port\n", argv[0]);
		return 1;
	}
	fflush(stdout);
	return server(port);
}
