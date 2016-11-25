#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

int main()
{

	int sock, connected, bytes_recieved, sin_size, true = 1, i;	/* parameter */
	char send_data [10000], recv_data[10000],  choose, chD;
	struct sockaddr_in server_addr, client_addr;
	struct dirent *ptr;
	FILE *F;
	DIR *dir;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {            /* create */
		perror("Socket");
		exit(1);
		}

	if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&true,sizeof(int)) == -1) {
		perror("Setsockopt");			/* reuse address after close socket */
		exit(1);
		}
        
	server_addr.sin_family = AF_INET;				/* set address */
	server_addr.sin_port = htons(1625);     
	server_addr.sin_addr.s_addr = INADDR_ANY;	/* any address on this computer */ 
	bzero(&(server_addr.sin_zero),8); 

        if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1){
            perror("Unable to bind");					/* bind */
            exit(1);
        }

        if (listen(sock, 5) == -1){					/* listen */
            perror("Listen");
            exit(1);
        }
		
	printf("\nWaiting for client on port 1625...");
        fflush(stdout);

        while(1){

		sin_size = sizeof(struct sockaddr_in);
		connected = accept(sock, (struct sockaddr *)&client_addr,&sin_size);	/* connect */
		printf("\nGot a connection from (%s , %d).\n",
			inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
		fflush(stdout);

		while(1){
			bytes_recieved=recv(connected,recv_data,10000,0);	/* receive */
			recv_data[bytes_recieved]='\0';	/* set the end of received massage */ 

			choose=recv_data[0];		

			switch(choose) {

				case 'C': case 'c':	/* create */

					strcpy(send_data,"Enter a file name : ");
					send(connected,send_data,strlen(send_data),0);
					bytes_recieved=recv(connected,recv_data,10000,0);
					recv_data[bytes_recieved]='\0';

					F=fopen(recv_data,"w");	/* create file */
					fclose(F);
					strcpy(send_data,"Complete !\n");

					send(connected,send_data,strlen(send_data),0);
					break;

				case 'E': case 'e':	/* edit (append a string) */

					strcpy(send_data,"Choose a file : ");
					send(connected,send_data,strlen(send_data),0);
					bytes_recieved=recv(connected,recv_data,10000,0);
					recv_data[bytes_recieved]='\0';
					F=fopen(recv_data,"r");	/* check */

					if (F!=NULL){
					fclose(F);	/* end check */

					F=fopen(recv_data,"a");	/* add a string */
					strcpy(send_data,"Enter a string :\n");
					send(connected,send_data,strlen(send_data),0);
					bytes_recieved=recv(connected,recv_data,10000,0);
					recv_data[bytes_recieved]='\0';
					fwrite(recv_data,sizeof(char),bytes_recieved,F);/* add */
					fclose(F);

					strcpy(send_data,"Complete !\n");
					send(connected,send_data,strlen(send_data),0);
					}
					else{
					strcpy(send_data,"Error !\n");
					send(connected,send_data,strlen(send_data),0);
					}
					break;

				case 'R': case 'r':	/* remove */

					strcpy(send_data,"Choose a file : ");
					send(connected,send_data,strlen(send_data),0);
					bytes_recieved=recv(connected,recv_data,10000,0);
					recv_data[bytes_recieved]='\0';

					remove(recv_data);	/* remove */

					strcpy(send_data,"Complete !\n");
					send(connected,send_data,strlen(send_data),0);
					break;

				case 'L': case 'l':	/* list */

					dir=opendir(".");	/* current folder */

					/* Append file name one by one */
					while ((ptr = readdir(dir))!=NULL){
					strcat(send_data,ptr->d_name);
					strcat(send_data,"\n");
					}

					send(connected,send_data,strlen(send_data),0);
					closedir(dir);

					strcpy(send_data,"Complete !\n");
					send(connected,send_data,strlen(send_data),0);
					break;

				case 'D': case 'd':

					strcpy(send_data,"Choose a file : ");
					send(connected,send_data,strlen(send_data),0);

					/* get file name */
					bytes_recieved=recv(connected,recv_data,10000,0);
					recv_data[bytes_recieved]='\0';
					F = fopen(recv_data,"r");

					if (F==NULL){	/* check */
						strcpy(send_data,"Error !\n");
						send(connected,send_data,strlen(send_data),0);
						break;
						}


					/* send file */
					/* get data by while loop */ 
					while (fgets(send_data,10000,F)!=NULL){
						/* send */
						send(connected,send_data,strlen(send_data),0);
						/* wait for client to save data */
						/* receive when client done */
						bytes_recieved=recv(connected,recv_data,10000,0);
						recv_data[bytes_recieved]='\0';
						}

					fclose(F);
					strcpy(send_data,"Complete !\n");
					send(connected,send_data,strlen(send_data),0);
					break;

				default :	/* other */

					strcpy(send_data,"Error !\n");
					send(connected,send_data,strlen(send_data),0);
				}
			}
		}       
	close(sock);
	return 0;
}
