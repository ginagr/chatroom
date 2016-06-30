//
//  server.c
//  Server1
//
//  Created by GGR.
//  Copyright © 2016 GGR. All rights reserved.
//
#include "server.h"

int main(int argc , char *argv[])
{
    int opt = 1;
    int csock[MAX_CLIENTS]; //array of client sockets
    int msock, addrlen, nsock, act, messnum, sd, msd;
    
    struct sockaddr_in address;
    
    char message[1025];  //max number for message

    fd_set readfds; //socket desc
    
    //initialise all csock to 0
    for (int i = 0; i < MAX_CLIENTS; i++){ csock[i] = 0; }
    
    //master socket
    if((msock = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    //make sure msock can allow multiple connections
    if(setsockopt(msock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 ) {
        perror("setsockopt fail");
        exit(EXIT_FAILURE);
    }
    
    //assign address, family, port
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_family = AF_INET;
    address.sin_port = htons( PORT );
    
    //bind socket to localhost port 5000 **make sure clients and server port are same
    if (bind(msock, (struct sockaddr *)&address, sizeof(address))<0){
        perror("bind fail");
        exit(EXIT_FAILURE);
    }
    //print port
    printf("Server on port %d \n", PORT);
    
    //4 pending connections for msock
    if (listen(msock, 4) < 0){
        perror("listen fail");
        exit(EXIT_FAILURE);
    }
    
    //accept incoming connection and print out status
    addrlen = sizeof(address);
    puts("Connecting...");
    
    while(1){ //loop
        FD_ZERO(&readfds); //clear
        FD_SET(msock, &readfds); //add msock
        msd = msock;
        
        for (int i = 0; i < MAX_CLIENTS; i++){ //iterate through sock array
            sd = csock[i];
            if(sd > 0) { FD_SET( sd , &readfds); } // there is a socket in csock[i]
            if(sd > msd) {msd = sd;} //get largest file desc. num for select
        }
        
        //wait for activity on a sockets
        act = select(msd+1, &readfds, NULL, NULL, NULL); //timeout is NULL, so will go indefinitely
        
        if((act < 0) && (errno!=EINTR)){ printf("select error"); } //report activity error
        
        if(FD_ISSET(msock, &readfds)){ //incoming connection
            if((nsock = accept(msock, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0){
                perror("accept");
                exit(EXIT_FAILURE);
            }
            
            //inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , nsock , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
            
            puts("Welcome message sent successfully");
            
            //add new socket to array of sockets
            for (int i = 0; i < MAX_CLIENTS; i++){
                //if position is empty
                if( csock[i] == 0 ){
                    csock[i] = nsock;
                    printf("Adding to list of sockets as %d\n" , i);
                    break;
                }
            }
        }
        
        //else its some IO operation on some other socket
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            //get socket num
            sd = csock[i];
            
            if (FD_ISSET( sd , &readfds))
            {
                //Check if it was for closing , and also read the incoming message
                if ((messnum = (int)read( sd , message, 1024)) == 0)
                {
                    //Somebody disconnected , get his details and print
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                    
                    //Close the socket and mark as 0 in list for reuse
                    close( sd );
                    csock[i] = 0;
                }
                
                //Send out message to all clients
                else {
                    //set the string terminating NULL byte on the end of the data read
                    message[messnum] = '\0';
                    for(int j=0; j < MAX_CLIENTS; j++){
                        if(sd != csock[j]){
                            send(csock[j], message, strlen(message), 0);
                        }
                    }
                    memset(message, '\0', messnum);

              //      send(sd , buffer , strlen(buffer) , 0 );
                }
            }
        }
    }
    
    return 0;
}