/*	SERVER SIDE PROGRAM IMPLEMENTATION	*/
#include <winsock2.h>//api for socket functions
#include <ws2tcpip.h>//defined socklen_t
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>//write and read function
#include <string.h>
#include <ctype.h>
#include <pthread.h>//helps to create threads
#include <signal.h>
#include <time.h>

#define bzero(b,len) (memset((b), '\0', (len)), (void) 0) //macro to clear the buffer

#define MAX_CLIENTS 100 //threshold limit for accepting clients
#define BUFFER_SZ 2048 //buffer message size
#define LENGTH 2048 //lenght of message buffer

#define DEFAULT_PORT "19819" //port number

#define cipherKey 'S' //for encryption transfering data
#define nofile "File Not Found!"

static unsigned int cli_count = 0;
static int uid = 10;


int  listenfd , connfd ;

int option = 1;//for setting socket	
struct sockaddr_in serv_addr;
struct sockaddr_in cli_addr;
pthread_t tid;
WSADATA wsa_data;
socklen_t clilen = sizeof(cli_addr);
int flag = 0, release=0;;

/* Client structure */
typedef struct{
	struct sockaddr_in address;
	int sockfd;
	int uid;
	char name[32];
} client_t;

client_t *clients[MAX_CLIENTS];

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;


//******Group Chat*********

//function to catch 'ctrl+c' signal
void catch_ctrl_c_and_exit(int sig) {
    flag = 1;
}


//function to print default error message
void DieWithError(char *errorMessage)
{
	fprintf(stderr,"\n\n\t%s\n\n",errorMessage);
}

// function over writing the std with '>' character
void str_overwrite_stdout() {
    printf("\r%s", "\t:> ");
    fflush(stdout);
}

//function to replace new line character with null character
void str_trim_lf (char* arr, int length) {
  int i;
  for (i = 0; i < length; i++) {
    if (arr[i] == '\n') {
      arr[i] = '\0';
      break;
    }
  }
}

//function to print ip address of rejected connection
void print_client_addr(struct sockaddr_in addr){
    printf("%d.%d.%d.%d",addr.sin_addr.s_addr & 0xff,(addr.sin_addr.s_addr & 0xff00) >> 8,(addr.sin_addr.s_addr & 0xff0000) >> 16,(addr.sin_addr.s_addr & 0xff000000) >> 24);
}

//Add clients to queue
void queue_add(client_t *cl){
	pthread_mutex_lock(&clients_mutex);
	
	int i;
	for(i=0; i < MAX_CLIENTS; ++i){
		if(!clients[i]){
			clients[i] = cl;
			break;
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}

//Remove clients to queue 
void queue_remove(int uid){
	pthread_mutex_lock(&clients_mutex);
	
	int i;
	for(i=0; i < MAX_CLIENTS; ++i){
		if(clients[i]){
			if(clients[i]->uid == uid){
				clients[i] = NULL;
				break;
			}
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}

//Send message to all clients except sender 
void send_message(char *s, int uid){
	pthread_mutex_lock(&clients_mutex);
	
	int i;
	for(i=0; i<MAX_CLIENTS; ++i){
		if(clients[i]){
			if(clients[i]->uid != uid){
				if(send(clients[i]->sockfd, s, strlen(s),0) < 0){
					DieWithError("ERROR: write to descriptor failed");
					break;
				}
			}
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}


//Function to Handle all communication with the client
void *handle_client(void *arg){
	char buff_out[BUFFER_SZ];
	char rname[32];
	int leave_flag = 0;

	cli_count++;
	client_t *cli = (client_t *)arg;
	bzero(rname,32);
	if(recv(cli->sockfd, rname, 32, 0) <= 0 || strlen(rname) <  2 || strlen(rname) >= 32-1){
		str_overwrite_stdout();
		printf("Didn't enter the name.\n");
		leave_flag = 1;
	} else{
		strcpy(cli->name, rname);
		bzero(buff_out,strlen(buff_out));
		sprintf(buff_out, "%s has joined\n", cli->name);
		str_overwrite_stdout();
		printf("%s", buff_out);
		send_message(buff_out, cli->uid);
	}

	bzero(buff_out, BUFFER_SZ);

	while(1){
		if (leave_flag) {
			break;
		}

		int receive = recv(cli->sockfd, buff_out, BUFFER_SZ, 0);
		if(receive>0){
			if(strlen(buff_out)>0)
			{
				
				send_message(buff_out, cli->uid);
				str_trim_lf(buff_out, strlen(buff_out));
				str_overwrite_stdout();
				printf("%s \n", buff_out);
			}
		}else{
			sprintf(buff_out, "%s has left\n", cli->name);
			str_overwrite_stdout();
			printf("%s", buff_out);
			send_message(buff_out, cli->uid);
			leave_flag = 1;
		}
		bzero(buff_out, BUFFER_SZ);
	}
	
  	//Delete client from queue 
	close(cli->sockfd);
  	queue_remove(cli->uid);
  	free(cli);
 	cli_count--;
  	pthread_detach(pthread_self());//removing the theread associated with the current exited client
	return NULL;
}

//function to start group chat server
void GroupChatServer(int listenfd)
{
	int n;

	printf("\n\n\t^^^^^^^^^^^^^ WELCOME TO THE MYCHAT APPLICATION ^^^^^^^^^^^^^ \n\n");
	while(1){
		
		connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &clilen);

		/* Check if max clients is reached (cli_count+1)==MAX_CLIENTS*/
		if((cli_count +1) == MAX_CLIENTS){
			printf("\n\tMax clients reached. Rejected: ");
			print_client_addr(cli_addr);
			printf(" :: Port :> %d\n", cli_addr.sin_port);
			closesocket(connfd);
			continue;
		}

		/* Client settings */
		client_t *cli = (client_t *)malloc(sizeof(client_t));
		cli->address = cli_addr;
		cli->sockfd = connfd;
		cli->uid = uid++;

		/* Add client to the queue and fork thread */
		queue_add(cli);

		pthread_create(&tid, NULL, &handle_client, (void*)cli);
		

		/* Reduce CPU usage */
		sleep(1);
	}	
}
////******Group Chat*********

//****Personal chat************
void PersonalChatServer(int psockfd,char *user_id) 
{
	int MAX=256,i=0;//buffersize
    char buff[MAX],friend_Uname[MAX],temp; 
    int n;
   printf("\n\n\t^^^^^^^^^^^^^ WELCOME TO THE MYCHAT APPLICATION ^^^^^^^^^^^^^ \n\n");
    scanf("%c",&temp);
    recv(psockfd, friend_Uname, 40,0);//receiving the user id of sender
    
    send(psockfd, user_id, 40,0);//sending our user id
    while(1)
    {
        bzero(buff, sizeof(buff));
        // read the message from friend and copy it in buffer 
        recv(psockfd, buff, sizeof(buff),0); 
        // print buffer which contains the client contents 
        str_overwrite_stdout();
        printf("From %s : %s\n", friend_Uname, buff);
        str_overwrite_stdout();
        printf("You : ");
        bzero(buff, MAX); 
        // copy server message in the buffer
        gets(buff);
        // and send that buffer to friend 
        send(psockfd, buff, sizeof(buff),0); 
  
        // if msg contains "Exit" then you get exited and chat ends. 
        if (strncmp(buff,"exit",3) == 0) { 
           printf("\n\n\tYou are exiting...\n\n\tThank you for using our Application.\n\n\t***HAVE a NICE day!!! :XD***\n\n");
            break; 
        }
    } 
} 
///***personal chat**********

//****************FTf's***********
// function to encrypt 
char Cipher(char ch) 
{ 
    return ch ^ cipherKey;//XOR encryption
} 
  
// function sending file 
int sendFile(FILE* fp, char* buf, int s) 
{ 
    int i, len; 
    if (fp == NULL) { 
        strcpy(buf, nofile); 
        len = strlen(nofile); 
        buf[len] = EOF; 
        for (i = 0; i <= len; i++) 
            buf[i] = Cipher(buf[i]); 
        return 1; 
    } 
  
    char ch, ch2; 
    for (i = 0; i < s; i++) { 
        ch = fgetc(fp); 
        ch2 = Cipher(ch); 
        buf[i] = ch2; 
        if (ch == EOF) 
            return 1; 
    } 
    return 0; 
}
//function to handle file data transfering
void FileTransferHandlerServer(int connfd)
{
		int nBytes;
		char net_buf[32];
		short ch;
		FILE *fp; 
		while (1)
		{ 
        	printf("\nWaiting for file name...\n\n"); 
  
        	// receive file name 
       		bzero(net_buf,32);
        	nBytes=recv(connfd, net_buf, 32, 0);
        	if(strcmp(net_buf,"exit")==0){
        		printf("\n\n User exited\n\n");
        		break;
        	}

        	printf("The file which client wants is '%s'.\nDo you have this file? Enter '1' for yes or '0' for no.\nEnter Choice : \n",net_buf);
        	scanf("%d",&ch);
        	if(ch==1)
        	{
        		
        		printf("Enter the file path at which '%s' file is located in your system directory.\nPath : \n",net_buf);
        		bzero(net_buf,sizeof(net_buf));
        		scanf("%s", net_buf);

        		fp = fopen(net_buf, "r");  
        		if (fp == NULL){
            		printf("\nFile open failed!\n");
            		send(connfd, "File Not Found!!", 32,0); 
        		}
        		else
        		{
        			send(connfd, "File Found!!", 32,0);
            		printf("\nFile Successfully opened!\n");
            		printf("\nSending File.....\n");
        			while (1)
        			{ 
  
            			// process 
            			if (sendFile(fp, net_buf, 32)){ 
                			send(connfd, net_buf, 32,0);
                			break; 
            			} 
  
            			// send 
            			send(connfd, net_buf, 32,0);
            			bzero(net_buf,32);
        			}
        			if (fp != NULL){
	            		fclose(fp); 
	        		}
    			}
			}else{send(connfd, "File Not Found!!", 32,0);}
        }
}

//****************FTf's***********



//***********************************************************************************************
void startServer(char *userID, char key)
{
	int port;
	printf("\n\n<<.>> Enter the port number of server to start the server (make sure port is valid and available!)  \n\n\t::> ");
	scanf("%d", &port);

  	//wsastartup setting
  	if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) 
 	{ 
 		fprintf(stderr,"WSAStartup failed"); 
 		exit(1); 
 	}
 	
 	//initializing values to zero
 	ZeroMemory(&serv_addr, sizeof(serv_addr));//bzero also can be used
 	ZeroMemory(&cli_addr, sizeof(cli_addr));

  	//Socket settings 
  	listenfd = socket(AF_INET, SOCK_STREAM, 0);//initializing socket
  	serv_addr.sin_family = AF_INET;
  	serv_addr.sin_addr.s_addr = INADDR_ANY;//accepts connections from any address
  	serv_addr.sin_port = htons(port);//converts host byte order to tcp/ip byte order

    if(setsockopt(listenfd, SOL_SOCKET,SO_REUSEADDR ,(char*)&option,sizeof(option)) < 0){
		DieWithError("ERROR: setsockopt failed");
		WSACleanup();
    	return;
	}

	//Bind with client address
  	if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    	DieWithError("ERROR: Socket binding failed");
    	WSACleanup();
    return;
  	}

  	//Listen to client socket connections
  	if (listen(listenfd, 10) <0) {
    	DieWithError("ERROR: Socket listening failed");
    	WSACleanup();
    	return;
	} 

	if(key=='T')//file transfering mode
	{
		printf("\n\n\tWaiting for client users to connect......\n\n");
		connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &clilen);
		if(connfd < 0){
			printf("\n\n\tConnection failed!!\n");
			return;
		}
		FileTransferHandlerServer(connfd);
		
	}
	else if(key=='G')//Group chat mode
	{
		GroupChatServer(listenfd);
	}else{//personal char mode
		connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &clilen);
		PersonalChatServer(connfd,userID);
	}
	closesocket(listenfd);
	closesocket(connfd);
	WSACleanup();
	return;
}
