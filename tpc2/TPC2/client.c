#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include "udp_comm.h"

#define BUFSIZE 1024

//This function returns a random number between min_num and max_num 
int random_number(int min_num, int max_num)
{
	int result=0,low_num=0,hi_num=0;
	if(min_num<max_num)
	{
		low_num=min_num;
		hi_num=max_num+1; // this is done to include max_num in output.
	}else{
		low_num=max_num+1;// this is done to include max_num in output.
		hi_num=min_num;
	}
	srand(time(NULL));
	result = (rand()%(hi_num-low_num))+low_num;
	return result;
}

int client(char* hostname, int port) {

	char buffer[BUFSIZE];

	//Criar uma socket upd num socket aleatorio entre 1025 e 50000
	int sock = UDP_Open(random_number(1025,50000));
	if(sock == -1) {
		perror("Erro a criar socket: ");
		return 1;
	}

	while(1) {
		//Ler commando
		fgets(buffer, BUFSIZE, stdin);
		
		//Preparar o endereço de destino
		struct sockaddr_in endereco;
		if(UDP_FillSockAddr(&endereco, hostname, port) == -1) {
			printf("Erro a preencher o endereço de destino da mensagem\n");
			continue;
		}
		
		//Enviar a mensagem
		int sent = UDP_Write(sock, &endereco, buffer, strlen(buffer) + 1);
		if(sent != strlen(buffer) + 1) {
			printf("Erro ao enviar a mensagem para o servidor (only sent %d bytes)\n", sent);
			continue;
		}
		
		//Receber a mensagem de resposta e imprimir o seu conteudo.
		int received = UDP_Read(sock, &endereco, buffer, BUFSIZE);
		if(received < 0) {
			printf("Erro ao receber mensagem do servidor\n");
			continue;
		} else {
			printf("%s\n", buffer);
		}
	}
	
	printf("Client terminated.");
}

int main( int argc, char *argv[]){

	int port;
	char* hostname;
	
	if( argc == 2){
		hostname = "localhost"; //hostname do servidor.
		port = atoi(argv[1]); //porta do servidor.
	}
	else{
		printf("Usage: %s port\n", argv[0]);
		return 1;
	}
	
	return client(hostname, port);

}

