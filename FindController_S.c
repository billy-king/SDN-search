#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#define MAXLINE 2048
void service_reply(int , int);
void sig_handle(int);
typedef struct{
	int pid;
	char IP[20];
	uint16_t PORT;
}client_info;


client_info client[20];
int client_num = 0;


int main(int argc , char *argv[]){
	int listenfd , connectfd , clilen , pid;
	int PORT;
	struct sockaddr_in cli_addr , serv_addr;
	if((listenfd = socket(AF_INET , SOCK_STREAM , 0)) < 0)
		printf("server: can't open stream socket");
	if (argc != 2){
		fprintf(stderr,"Usage : server <port>");
 		exit(1);
 	}
 	PORT = (u_short)atoi(argv[1]);
	bzero((char *) &serv_addr, sizeof(serv_addr));//clean serv_addr
	//address family Transport layer protocol AF_INET:TCP/UDP ...
	serv_addr.sin_family = AF_INET ;
	//IP address INADDR_ANY:localhost IP 
	serv_addr.sin_addr.s_addr = htonl( INADDR_ANY );
	//port number 0系統隨機
	serv_addr.sin_port = htons(PORT);
	//設定Port可以重複bind
	int optval = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (void*)&optval, sizeof(optval));
	
	if (bind(listenfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		printf("server: can't bind local address");
	/*****listen sockfd, max requester********/
	listen(listenfd , 30);
	signal(SIGCHLD,sig_handle);
	while(1){
		clilen = sizeof(cli_addr);
		connectfd = accept(listenfd , (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
		if (connectfd < 0) 
			printf("server: accept error");
		if((pid = fork()) < 0)
			printf("server: fork error");
		else if(pid == 0){
			close(listenfd);
			/**ip value and show endian
			uint8_t *p;
			p = (uint8_t *)&cli_addr.sin_addr.s_addr; 
			printf("%0x\n", cli_addr.sin_addr.s_addr);
			printf("%p:%0x , %p:%0x ,%p:%0x ,%p:%0x\n",p ,*p , p+1 ,*(p+1) , p+2,*(p+2) ,p+3, *(p+3));
			**/
			int port = (int) ntohs(cli_addr.sin_port);
			printf("connect from: %s port: %d\n" , inet_ntoa(cli_addr.sin_addr) , port);
			service_reply(connectfd , port);
			exit(0);
		}
		client[client_num].pid = pid;
		sprintf(client[client_num].IP , "%s" , inet_ntoa(cli_addr.sin_addr));
		client[client_num].PORT = (int) ntohs(cli_addr.sin_port);
		client_num += 1;
		close(connectfd);
	}
}

void sig_handle(int signal){
	int status , pid , i;
	/*pid = wait(&status);
	printf("child %d terminated\n", pid);*/
	while((pid = waitpid(-1, &status, WNOHANG)) > 0){
		if(pid > 0){
			for(i = 0 ; i < client_num ; i++){
				if(client[i].pid == pid)
					printf("child %d terminated from %s/%d\n", pid , client[i].IP, client[i].PORT); 
			}
		}
	}
}
void service_reply(int connectfd , int port){
	int n;
	char send_buff[MAXLINE] , recv_buff[MAXLINE];
	char *instr_ptr;
	char instr[5][MAXLINE];
	int stdo = dup(STDOUT_FILENO);
	char controllerIP[80] = "tcp:";
	char FloodlightIP[] = "192.168.190.149";
	char RyuIP[] = "192.168.190.154";
	char ODLIP[] = "192.168.190.151";
	char ONOSIP[] = "192.168.190.152";
	char controllerPort[] = ":6633";
	while(1){
		//init buff to zero
		bzero(&send_buff , sizeof(send_buff));
		bzero(&recv_buff , sizeof(recv_buff));
		bzero(&instr , sizeof(instr));
		//catch clent instruction
		while(1){
			n = read(connectfd ,recv_buff ,MAXLINE);
			if(n == 0)
				return;
			else if(n < 0 && errno == EINTR)
				continue;
			else{
				if(strcmp(recv_buff , "\n\r") == 0)
						continue;
				else
					break;
			}
		}
		printf("recv_buff = %s\n" , recv_buff);

		//split receive instruction with space
		instr_ptr = strtok(recv_buff , " ");
		int token_num = 0;
		int len;
		FILE *fp;
		while(instr_ptr != NULL){
			strcpy(instr[token_num] , instr_ptr);
			len = strlen(instr[token_num]);
			if(len > 0 && instr[token_num][len - 1] == '\n')
				instr[token_num][len - 1] = '\0';
			//printf("%s\n" , instr[token_num]);
			token_num += 1;
			instr_ptr = strtok(NULL , " ");
		}

		if(strcmp(instr[0] , "F") == 0){
			if(fork() == 0){
				dup2(connectfd , STDOUT_FILENO);
				//printf("File list\n");
				strcat(controllerIP , FloodlightIP);
				strcat(controllerIP , controllerPort);
				printf("Set-Controller IP = %s\n" , controllerIP);
				execlp("ovs-vsctl" , "ovs-vsctl" ,"set-controller" , "br0", controllerIP , NULL);
				dup2(stdo , STDOUT_FILENO);
			}
		}
		if(strcmp(instr[0] , "R") == 0){
			//dup2(connectfd , STDOUT_FILENO);
			if(fork() == 0){
				dup2(connectfd , STDOUT_FILENO);
				//printf("File list\n");
				strcat(controllerIP , RyuIP);
				strcat(controllerIP , controllerPort);
				printf("Set-Controller IP = %s\n" , controllerIP);
				execlp("ovs-vsctl" ,"ovs-vsctl" , "set-controller" , "br0", controllerIP , NULL);
				dup2(stdo , STDOUT_FILENO);
			}
		}
		if(strcmp(instr[0] , "O") == 0){
			if(fork() == 0){
				dup2(connectfd , STDOUT_FILENO);
				//printf("File list\n");
				strcat(controllerIP , ODLIP);
				strcat(controllerIP , controllerPort);
				printf("Set-Controller IP = %s\n" , controllerIP);
				execlp("ovs-vsctl" , "ovs-vsctl" ,"set-controller" , "br0", controllerIP , NULL);
				dup2(stdo , STDOUT_FILENO);
			}
		}
		if(strcmp(instr[0] , "N") == 0){
			if(fork() == 0){
				dup2(connectfd , STDOUT_FILENO);
				//printf("File list\n");
				strcat(controllerIP , ONOSIP);
				strcat(controllerIP , controllerPort);
				printf("Set-Controller IP = %s\n" , controllerIP);
				execlp("ovs-vsctl" , "ovs-vsctl" ,"set-controller" , "br0", controllerIP , NULL);
				dup2(stdo , STDOUT_FILENO);
			}
		}
	}
		
}
