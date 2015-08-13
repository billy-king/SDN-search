#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <netdb.h>
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
 	bzero(&serv_addr , sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr = *((struct in_addr *)hostname->h_addr); 
	serv_addr.sin_port = htons(SERVER_PORT);

	serv_fd = socket(AF_INET , SOCK_STREAM , 0);
	if(connect(serv_fd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) == -1)
	{
	  	fprintf(stderr,"connect error:%s" , strerror(errno));
	  	exit(1);
	}
	get_service(stdin ,serv_fd);
}
void get_service(FILE *fp , int serv_fd){
	int n;
	char *instr_ptr;
	char instr[5][MAXLINE];
	char send_buff[MAXLINE] , recv_buff[MAXLINE] , send_copy[MAXLINE];
	while(1){
		printf("[C]hange dir , [S]how file , [D]ownload file [U]pload file [E]xit:\n");
		//init buff to zero
		bzero(&send_buff , sizeof(send_buff));
		bzero(&send_copy , sizeof(send_copy));
		bzero(&recv_buff , sizeof(recv_buff));
		while(fgets(send_buff , MAXLINE , fp) != NULL){
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
				printf("%s\n" , instr[token_num]);
				token_num += 1;
				instr_ptr = strtok(NULL , " ");
			}
			if(strcmp(instr[0] , "C") == 0){
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
			if(strcmp(instr[0] , "S") == 0){
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
			if(strcmp(instr[0] , "D") == 0){
				if((fp = fopen(instr[1] , "w+")) == NULL)
					printf("Open file error!! Error:%s\n", strerror(errno));
				write(serv_fd  , send_buff , sizeof(send_buff));
				bzero(&send_buff , sizeof(send_buff));
				int file_size;
				read(serv_fd , recv_buff , sizeof(recv_buff));
				file_size = atoi(recv_buff);
				printf("file_size = %d\n" , file_size);
				bzero(&recv_buff , sizeof(recv_buff));
				int recv_size;
				while(file_size > 0){
					recv_size = read(serv_fd , recv_buff , sizeof(recv_buff));
					if(file_size >=2048)
						fwrite(recv_buff , 1 ,  recv_size , fp);
					else
						fwrite(recv_buff , 1 ,  file_size , fp);
					file_size -= sizeof(recv_buff);
					bzero(&recv_buff , sizeof(recv_buff));
					//printf("file_size = %d\n" , file_size);
				}
				fclose(fp);
				read(serv_fd , recv_buff , sizeof(recv_buff));
				printf("%s" , recv_buff);
				bzero(&recv_buff , sizeof(recv_buff));
				bzero(&send_buff , sizeof(send_buff));
				break;
			}
			if(strcmp(instr[0] , "U") == 0){
				int file_size;
				int send_size;
				write(serv_fd  , send_buff , sizeof(send_buff));
				bzero(&send_buff , sizeof(send_buff));
				if((fp = fopen(instr[1] , "rb")) == NULL){
					printf("Open file error!! Error:%s\n", strerror(errno));
				}
				printf("Sending File:[%s]\n" , instr[1]);
				fseek(fp, 0, SEEK_END);
				file_size = ftell(fp);
				fseek(fp, 0, SEEK_SET);
				sprintf(send_buff, "%d", file_size);
				write(serv_fd , send_buff , sizeof(send_buff));
				bzero(&send_buff , sizeof(send_buff));
				while((send_size=fread(send_buff, sizeof(char), 2048, fp)) != 0){
					if(write(serv_fd , send_buff , sizeof(send_buff)) < 0){
						fprintf(stderr , "Write Error!!");
						break;
					}
					bzero(&send_buff , sizeof(send_buff));
	      		}
	      		fclose(fp);
	      		printf("Close File:[%s]\n" , instr[1]);
				printf("Upload Success!!\n");
				break;
			}
		}
	}


}