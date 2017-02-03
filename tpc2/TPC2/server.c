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
#define MAX_USERS 1024

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
	free(req->clientAddr);
	free(req->request);
	free(req);
}


void* threadRoutine(void* arg){
	char* command = strtok(arg->request," ");
	char* result;
	switch(command){
		//number
		case "functionA":
		char* end;
		int arg = strtol(strtok(NULL, " "), end, 10);
		if(strtok(NULL, " ") == NULL || end == 0){
			printf("Invalid operation");
			break;
		}
		functionA(arg, result); //what do with result?
		break;
		//number number
		case "functionB":
		char* end1;
		char* end2;
		int arg1 = strtol(strtok(NULL, " "), end1, 10);
		int arg2 = strtol(strtok(NULL, " "), end2, 10);
		if(strtok(NULL, " ") == NULL || end1 == 0 || end2 == 0){
			printf("Invalid operation");
			break;
		}
		functionb(arg1, arg2, result); //what do with result?
		break;
		//string
		case "functionC"
		break;
		default:
			//print "Invalid operation"
			break;
	}
	return (void*) result;
}

int server(int port){
	//Criar uma socket upd num socket aleatorio entre 1025 e 50000
	int sock = UDP_Open(random_number(1025,50000));
	if(sock == -1) {
		perror("Erro a criar socket: ");
		return 1;
	}
	struct sockaddr_in endereco;
	if(UDP_FillSockAddr(&endereco, "localhost", port) == -1) {
		printf("Erro a preencher o endereço de chegada\n");
		continue;
	}
	struct requestInformation* req;

	pthread_t openThreads[MAX_USERS];
	int start = 0;
	int end = 0;

	char* result;

	//open server
	//Main cycle of the server.
	while(1) {
		//Prepara as estruturas em memória para receber um novo pedido
		req = newRequestInformation();
		UDP_Read(sock, endereco, req, BUFSIZE);
		Pthread_create(openThreads[(end++)%MAX_USERS], NULL, threadRoutine, (void*)req);
		for(int i = start; i < end; i++){
			if(pthread_kill(openThreads[i], 0) != 0){
				pthread_join(openThreads[i], result);
				UDP_Write(sock, endereco, result, BUFSIZE);
				start++;//this doesnt actually work
			}

		}


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

	return server(port);
}
