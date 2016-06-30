/*
 C ECHO client example using sockets
 */
#include "client.h" //printf


int main(int argc , char *argv[])
{
    int sock, messnum, pid;
    struct sockaddr_in server;
    char newmessage[1025];
    char name[100];
    char full[1025]; //max number of char for message
    char message[1025];
    
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1){ printf("Could not create socket"); }
    puts("Socket created");
    
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(5000); //make sure port is same as server
    
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
        perror("connect failed. Error");
        return 1;
    }
    
    puts("Connected\n");
    
    //Get username for new client
    printf("Please enter your name : ");
    scanf("%s" , name);
    
    char update[200];
    strcpy(update, name);
    
    strcat(name, ": ");
    
    //update all other clients of a new client entering the chat room
    strcat(update, " has entered the chat room");
       if(send(sock, update, strlen(update), 0) < 0){
        puts("Send name failed");
        return 1;
    }
    
    printf("You are now in the chat room, please type a message and press enter\n");
    
    //continuously scan for new messages being sent and recieved
    if((pid=fork()) > 0){
        while(scanf("%s" , message)){ //client types message and presses send
            memset(full, '\0', sizeof(full)); //make sure char is empty
            strcpy(full, name); //concatenate name so other users know who is sending message
            strcat(full, message);
            if((messnum = (int)send(sock, full, strlen(full), 0)) < 0){ //checks message was sent
                puts("Send message failed"); //send error message
                memset(message, '\0', sizeof(message)); //free message
                return 1;
            }
            memset(message, '\0', sizeof(message)); //free message char
        }
    }
    else{
        while(1){ //get new messages from other clients
            if((messnum = (int)read(sock, newmessage, 1024)) < 0){ //check message was recieved
                puts("new message fail"); //send error message
                break;
            }
            else{
                puts(newmessage); //print new message
                memset(newmessage, '\0', sizeof(newmessage)); //clear message
            }
        }
    }
    //close socket
    close(sock);
    return 0;
}