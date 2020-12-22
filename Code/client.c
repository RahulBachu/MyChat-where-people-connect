/*	CLIENT SIDE PROGRAM IMPLEMENTATION	*/
#include <winsock2.h>
#include <ws2tcpip.h>//socklen_t
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>//write and read function
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <signal.h>

#include "server.c"

#define bzero(b,len) (memset((b), '\0', (len)), (void) 0) //macro to clear the buffer

// Global variables
int sockfd = 0;
char *cname=NULL;

//******Group Chat*********

//functoion to handle sending message of client
void send_msg_handler() {
	
  	char message[LENGTH] = {};
	char buffer[LENGTH + 60] = {};
	char temp;
	int i=0;

  	while(1)
  	{
  		str_overwrite_stdout();
  		if(i==1){
  			scanf("%c",&temp);

  		}
  		i=1;
    	scanf("%[^\n]",message);
    	if(strncmp(message, "exit",3) == 0) {
				break;
    	} 
    	else{
    		bzero(buffer, LENGTH + 32);
      		sprintf(buffer, "%s: %s\n", cname, message);
     		send(sockfd, buffer, strlen(buffer), 0);
    	}

		bzero(message, LENGTH);
    	bzero(buffer, LENGTH + 32);
  	}
  	catch_ctrl_c_and_exit(2);
}

//functoion to handle receiving message of client
void recv_msg_handler() {
	char message[LENGTH] = {};
  	while (1)
  	{	
		int receive = recv(sockfd, message, LENGTH, 0);
    	if (receive > 0)
    	{
    			printf("%s", message);
      			str_overwrite_stdout();
    	} else if (receive == 0) 
    	{
			break;
    	} else {
    		//-1
			}
		memset(message, 0, sizeof(message));
	}
}
//function for having group conversation
void GroupChatClient(int sockfd,char *name1)
{

	// Send name entered to server
	cname=(char *)malloc(32*sizeof(char));
	strcpy(cname,name1);
	send(sockfd, name1, 40, 0);

	printf("\n\n\t^^^^^^^^^^^^^ WELCOME TO THE MYCHAT APPLICATION ^^^^^^^^^^^^^ \n\n");

	//creating a thread to handle sending messages thread
	pthread_t send_msg_thread;
  	if(pthread_create(&send_msg_thread, NULL, (void *) send_msg_handler, NULL) != 0){
		printf("\n\tERROR CODE: pthread...Consult Server\n");
		return;
	}

	//creating a thread to handle recieving messages thread
	pthread_t recv_msg_thread;
  	if(pthread_create(&recv_msg_thread, NULL, (void *) recv_msg_handler, NULL) != 0){
		printf("\n\tERROR CODE: pthread...Consult Server\n");
		return;
	}

	while (1){
		if(flag){
			printf("\n\n\tYou are exiting...\n\tThank you for using our Application.\n\n\t***HAVE a NICE day!!! :XD***\n\n");
			return;
    	}
	}
}
//******Group Chat*********

///***personal chat**********
void PersonalChatClient(int psockfd,char *user_id) 
{
	int MAX=256, i=0;//buffer size
    char buff[MAX], friend_Uname[MAX], temp;

    printf("\n\n\t^^^^^^^^^^^^^ WELCOME TO THE MYCHAT APPLICATION ^^^^^^^^^^^^^ \n\n");
    send(psockfd, user_id, 40,0);//sending our user id
    recv(psockfd, friend_Uname, 40,0);//receiving the user id of sender

    while(1)
    {
        bzero(buff, sizeof(buff)); 
         
        // copy server message in the buffer 
        str_overwrite_stdout();
        printf("You : ");
        gets(buff);
        send(psockfd, buff, sizeof(buff),0);
        if((strncmp(buff,"exit",3))==0)
        { 
            printf("\n\n\tYou are exiting...\n\tThank you for using our Application.\n\n\t***HAVE a NICE day!!! :XD***\n\n");
            break; 
        }
        
        bzero(buff, sizeof(buff)); 
        recv(psockfd, buff, sizeof(buff),0); 
        str_overwrite_stdout();
        printf("From %s : %s\n", friend_Uname,buff);
    } 
}
///***personal chat**********

///////***********file transfering functions*********  
// function to receive file
static int file_count=0; //number of files received count
int recvFile(char* buf, int s,char *fname) 
{
	FILE *fp;
    int i; 
    char ch, fileName[256];
    strcpy(fileName,fname);
    strcat(fileName,".txt");
    fp=fopen(fileName,"a+");
    if(!fp)
    {
    	printf("\n\tTrouble in open file!!n\n");
    	file_count--;
    	return 0;
    }
    for(i=0;i<s;i++)
    { 
        ch = buf[i]; 
        ch = Cipher(ch); 
        if (ch == EOF){
            return 1; 
        }
        else{
            fprintf(fp,"%c", ch); 
        }
    }
    fclose(fp);
    return 0; 
}
//function to handle data tranfer
void FileTransferHandlerClient(int sockfd)
{
	//retriving current date
	struct tm *local;
	time_t t=time(NULL);
	local=localtime(&t);
	FILE *fp; 
	int nBytes,i=0;
	char net_buf[32],file_name[512],current_date[256],temp;	   
	file_count=0;
	while (1)
	{
		//saving file in dd-mm-yyyy-hh-mm-ss-RecievedFile-
		bzero(current_date,sizeof(current_date));
		bzero(net_buf,sizeof(net_buf));
        strcpy(current_date,itoa(local->tm_mday,net_buf,10));
        strcat(current_date,"-");
          
        bzero(net_buf,sizeof(net_buf));
        strcat(current_date,itoa((local->tm_mon)+1,net_buf,10));
        strcat(current_date,"-");

        bzero(net_buf,sizeof(net_buf));
        strcat(current_date,itoa((local->tm_year)+1900,net_buf,10));
        strcat(current_date,"-");

        bzero(net_buf,sizeof(net_buf));
        strcat(current_date,itoa(local->tm_hour,net_buf,10));
        strcat(current_date,"-");

        bzero(net_buf,sizeof(net_buf));
        strcat(current_date,itoa(local->tm_min,net_buf,10));
        strcat(current_date,"-");

        bzero(net_buf,sizeof(net_buf));
        strcat(current_date,itoa(local->tm_min,net_buf,10));
        strcat(current_date,"-");

        bzero(net_buf,sizeof(net_buf));
        strcat(current_date,itoa(local->tm_sec,net_buf,10));
        strcat(current_date,"-");

		bzero(file_name,sizeof(file_name));
		strcpy(file_name,current_date);
		strcat(file_name,"RecievedFile-");

		bzero(net_buf,sizeof(net_buf));
        printf("\n\n\tEnter 'exit' as file name to terminate the program.\n\t<<.>> Please enter file name to receive : ");
        if(i==0){
        	scanf("%c",&temp);
        	i=1;
        } 
        scanf("%[^\n]", net_buf);
        net_buf[strcspn(net_buf,"\n")]='\0';
        send(sockfd, net_buf, sizeof(net_buf),0);

        if(strcmp(net_buf,"exit")==0){
        		break;
        }
        	
        bzero(net_buf,sizeof(net_buf));
        recv(sockfd, net_buf, sizeof(net_buf), 0);//recieving confirmation message for file found or not found
        printf("\n\n\tConfirmation Message received : %s",net_buf);

        if(strcmp(net_buf,"File Found!!")==0)
        {
        	printf("\n\n\t--------- Data recieving ------------\n\n");
        	file_count++;
        	bzero(net_buf,sizeof(net_buf));
        	strcat(file_name,itoa(file_count,net_buf,10));
        	printf("\n\n\t !!Recieving.......!!\n\n");
        	while (1)
        	{ 
            	// receive 
            	bzero(net_buf,sizeof(net_buf)); 
            	nBytes=recv(sockfd, net_buf, sizeof(net_buf), 0); 
  
            	// process recieved buffer data in to file
            	if (recvFile(net_buf, sizeof(net_buf),file_name)) { 
                	break; 
            	} 
        	} 
        	printf("\n\n\t--------- Recieved Successfully ---------\n\n"); 
			}
		}
		printf("\n\n\tTotal Number of Files Recieved : %d\n\n",file_count);
}
//****************FTf's***********

void connect_to_Server(char *userid,char key)
{
	int port;
	printf("\n\n<<.>> Enter the port number of server to connect (make sure server is running already!)  \n\n\t::> ");
	scanf("%d", &port);

	char *ipaddr=(char *) malloc(40*sizeof(char));
	char temp;
	bzero(ipaddr, sizeof(ipaddr));
	printf("\n\n<<.>> Enter the IP Address of server to connect (make sure server is running already!)  \n\n\t::> ");
	scanf("%c",&temp);
	gets(ipaddr);
    
	//wsastartup setting
  	if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) 
 	{ 
 		fprintf(stderr,"\n\tWSA Startup failed due to system issues..!!\n\n"); 
 		return; 
 	}
	signal(SIGINT, catch_ctrl_c_and_exit);
	struct sockaddr_in server_addr;

	/* Socket settings */
  	sockfd = socket(AF_INET, SOCK_STREAM, 0);
  	server_addr.sin_family = AF_INET;
  	server_addr.sin_addr.s_addr = inet_addr(ipaddr);
  	server_addr.sin_port = htons(port);

  	// Connect to Server
 	int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  	if (err == -1) {
		printf("\n\tConnection Failed!!, Please check with Server..!\n");
        WSACleanup();
		return;
	}

	if(key=='T')//file transfering mode
	{
		FileTransferHandlerClient(sockfd);
	}
	else if(key=='G')//Group chat mode
	{
		GroupChatClient(sockfd,userid);
	}else{//personal chat mode(on this port only 2 members can have conversation - server and client)
		PersonalChatClient(sockfd,userid);
	}
	closesocket(sockfd);
    WSACleanup();
	return;
}

