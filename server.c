#include<stdio.h>
#include<string.h>
#include<stdlib.h>	
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>

#define MAX 250 
#define PORT 8888

struct socket_chat
{
	int sock;
	int id;
};

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
	//Get the socket descriptor
	struct socket_chat *sockfd = socket_desc;
	char buff[MAX]; 

    // infinite loop for chat 
    for (;;) { 
        bzero(buff, MAX); 
  
        // read the message from client and copy it in buffer 
        read(sockfd->sock, buff, sizeof(buff)); 
        // print buffer which contains the client contents 
        printf("From client nº%d: %s\t To client : ",sockfd->id , buff); 
        bzero(buff, MAX); 

        // copy server message in the buffer 
		int i = 0;
        while((buff[i++] = getchar()) != '\n'); 
  
        // and send that buffer to client 
        write(sockfd->sock, buff, sizeof(buff)); 
  
        // if msg contains "Exit" then server exit and chat ended. 
        if (strncmp("exit", buff, 4) == 0) { 
            printf("Server Exit...\n"); 
            break; 
        } 
    }

	pthread_exit(NULL); 
}

int main(int argc , char *argv[])
{
	int socket_desc , new_socket , c, i=0;
	struct sockaddr_in server , client;
	struct socket_chat new_sock;
	char *message;
	
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);
	
	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		puts("bind failed");
		return 1;
	}
	puts("bind done");
	
	//Listen
	listen(socket_desc , 3);
	
	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	while( (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
	{
		puts("Connection accepted");
		
		//Reply to the client
		message = "Hello Client , I have received your connection. And now I will assign a handler for you\n";
		write(new_socket , message , strlen(message));
		
		pthread_t sniffer_thread;
		new_sock.sock = new_socket;
		new_sock.id = i;
		
		if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) &new_sock) < 0)
		{
			perror("could not create thread");
			return 1;
		}
		
		//Now join the thread , so that we dont terminate before the thread
		//pthread_join( sniffer_thread , NULL);
		puts("Handler assigned");
		i++;
	}
	
	if (new_socket<0)
	{
		perror("accept failed");
		return 1;
	}
	
	pthread_exit(NULL);

	return 0;
}
