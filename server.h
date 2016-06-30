//
//  server.h
//  Server1
//
//  Created by GGR.
//  Copyright © 2016 GGR. All rights reserved.
//

#ifndef server_h
#define server_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> 

#define PORT 5000 //arbitrary number, must be same as client though
#define MAX_CLIENTS 10 //max number of clients in chatroom

#endif /* server_h */
