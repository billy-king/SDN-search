#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <netdb.h>
#include <string.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <errno.h>


#define MAXLINE 2048
void get_service(FILE *fp ,int);
int main(int argc , char *argv[]){
	int serv_fd , SERVER_PORT;
	struct sockaddr_in serv_addr;
	struct hostent *hostname;
	if (argc != 3){
		fprintf(stderr,"Usage : client <server ip> <port>");
 		exit(1);
 	}
	if((hostname = gethostbyname(argv[1])) == NULL){	
 		fprintf(stderr,"Usage : client <server ip> <port>");
 		exit(1);	
 	}
 	SERVER_PORT = (u_short)atoi(argv[2]);
 	
	while(1){
		printf("connect....\n");
		bzero(&serv_addr , sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr = *((struct in_addr *)hostname->h_addr); 
		serv_addr.sin_port = htons(SERVER_PORT);

		serv_fd = socket(AF_INET , SOCK_STREAM , 0);
		if(connect(serv_fd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) == -1){
			sleep(3);
			continue;
		}
		else
			break;
	}
	get_service(stdin ,serv_fd);
}
void get_service(FILE *fp , int serv_fd){
	int n;
	char *instr_ptr;
	char instr[5][MAXLINE];
	char send_buff[MAXLINE] , recv_buff[MAXLINE] , send_copy[MAXLINE];
	while(1){
		printf("Find Controller...\n");
		//init buff to zero
		bzero(&send_buff , sizeof(send_buff));
		bzero(&send_copy , sizeof(send_copy));
		bzero(&recv_buff , sizeof(recv_buff));
		//while(1){
			strcpy(send_buff , "X");
			strcpy(send_copy , send_buff);

			//split receive instruction with space
			instr_ptr = strtok(send_copy , " ");
			int token_num = 0;
			int len;
			while(instr_ptr != NULL){
				strcpy(instr[token_num] , instr_ptr);
				len = strlen(instr[token_num]);
				if(len > 0 && instr[token_num][len - 1] == '\n')
					instr[token_num][len - 1] = '\0';
				//printf("%s\n" , instr[token_num]);
				token_num += 1;
				instr_ptr = strtok(NULL , " ");
			}
			if(strcmp(instr[0] , "X") == 0){
				write(serv_fd  , send_buff , sizeof(send_buff));
				bzero(&send_buff , sizeof(send_buff));
				if((n = read(serv_fd , recv_buff , MAXLINE)) == 0){
					printf("str_cli: server terminated prematurely"); 
					exit(0);
				}
				fputs(recv_buff , stdout);
				execlp("ovs-vsctl" , "ovs-vsctl" ,"set-controller" , "br0", recv_buff , NULL);
				bzero(&recv_buff , sizeof(recv_buff));
				break;
			}
			if(strcmp(instr[0] , "F") == 0){
				write(serv_fd  , send_buff , sizeof(send_buff));
				bzero(&send_buff , sizeof(send_buff));
				if((n = read(serv_fd , recv_buff , MAXLINE)) == 0){
					printf("str_cli: server terminated prematurely"); 
					exit(0);
				}
				fputs(recv_buff , stdout);
				bzero(&recv_buff , sizeof(recv_buff));
				break;
			}
			if(strcmp(instr[0] , "R") == 0){
				write(serv_fd  , send_buff , sizeof(send_buff));
				bzero(&send_buff , sizeof(send_buff));
				if((n = read(serv_fd , recv_buff , MAXLINE)) == 0){
					printf("str_cli: server terminated prematurely"); 
					exit(0);
				}
				fputs(recv_buff , stdout);
				bzero(&recv_buff , sizeof(recv_buff));
				break;
			}
			if(strcmp(instr[0] , "O") == 0){
				write(serv_fd  , send_buff , sizeof(send_buff));
				bzero(&send_buff , sizeof(send_buff));
				if((n = read(serv_fd , recv_buff , MAXLINE)) == 0){
					printf("str_cli: server terminated prematurely"); 
					exit(0);
				}
				fputs(recv_buff , stdout);
				bzero(&recv_buff , sizeof(recv_buff));
				break;
			}
			if(strcmp(instr[0] , "N") == 0){
				write(serv_fd  , send_buff , sizeof(send_buff));
				bzero(&send_buff , sizeof(send_buff));
				if((n = read(serv_fd , recv_buff , MAXLINE)) == 0){
					printf("str_cli: server terminated prematurely"); 
					exit(0);
				}
				fputs(recv_buff , stdout);
				bzero(&recv_buff , sizeof(recv_buff));
				break;
			}
			if(strcmp(instr[0] , "E") == 0){
				close(serv_fd);
				exit(1);
			}
		//}
	}


}
