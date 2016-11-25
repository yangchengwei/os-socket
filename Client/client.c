#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main()
{

	int sock, bytes_recieved;				/* parameter */
	char send_data[10000], recv_data[10000], Fname[100];
	struct hostent *host;
	struct sockaddr_in server_addr;
	FILE *F;

	host = gethostbyname("10.0.2.15");		/* get IP */

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {   /* create */
		perror("Socket");
		exit(1);
		}

        server_addr.sin_family = AF_INET;                       /* set address */
        server_addr.sin_port = htons(1625);   
        server_addr.sin_addr = *((struct in_addr *)host->h_addr);
        bzero(&(server_addr.sin_zero),8); 

        if (connect(sock, (struct sockaddr *)&server_addr,	/* connect */
			sizeof(struct sockaddr)) == -1){
		perror("Connect");
		exit(1);
		}

	printf("\nWelcome to Internet editer !\n");
	printf("There are some options you can choose below :\n");
	printf("(C)reate\n(E)dit\n(R)emove\n(L)ist\n(D)ownload\n");
	printf("Which one do you want to choose ?\n");

	while(1){			/* one command in one loop */
		printf("Please enter C , E , R , L or D : ");

		gets(send_data);		/* get command */

		if (strlen(send_data)!=1){	/* check command */
			printf("Error !\n");
			continue;
			}

		send(sock,send_data,strlen(send_data),0);	/* send command */

		/* download command have to open file */
		if (strcmp(send_data,"D") == 0 ||strcmp(send_data,"d") == 0){ 
			bytes_recieved = recv(sock,recv_data,10000,0);
			recv_data[bytes_recieved] = '\0';
			printf("%s",recv_data);	/* ask file name */
			gets(send_data);
			send(sock,send_data,strlen(send_data),0);
			F = fopen(send_data,"w");	/* create file */
			strcpy(Fname,send_data);	/* keep file name */

			/* download */
			while(1){	/* keep receiving data by while loop */
				
				bytes_recieved = recv(sock,recv_data,10000,0);	/* receive data */
				recv_data[bytes_recieved] = '\0';
				if (strcmp(recv_data,"Complete !\n") != 0&&	/* check if complete */
					strcmp(recv_data,"Error !\n") != 0){	/* check if error */

					fputs(recv_data,F);	/* write data into file */
					/* tell server that data was been writen */ 
					send(sock,recv_data,strlen(send_data),0);
					}
				else{
					printf("%s",recv_data); /* error or complete */
					break;
					}
				}
			fclose(F);		/* close file */
			/* remove it if error */
			if (strcmp(recv_data,"Error !\n") == 0) remove(Fname);
			continue;	/* finish command */
			}

		while(1){	/* keep receive message by while loop */

			bytes_recieved = recv(sock,recv_data,10000,0);
			recv_data[bytes_recieved] = '\0';

			if (recv_data[bytes_recieved-2] == '!'){	/* complete command */
				printf("%s",recv_data);			/* or error */
				break;
				}
			else if (recv_data[bytes_recieved-2] == ':'){	/* server ask message */
				printf("%s",recv_data);
				gets(send_data);			/* send massage */
				send(sock,send_data,strlen(send_data),0);
				}
			else printf("%s",recv_data);		/* server send a message to print */
			}
		}   
	return 0;
}
