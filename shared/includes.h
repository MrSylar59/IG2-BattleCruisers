#ifndef _H_INCLUDES_
#define _H_INCLUDES_

// Default includes
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Networking includes
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Signals
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

#define CHECK(sts, msg) if ((sts) == -1) {perror(msg);exit(-1);}
#define PORT_SRV 2048

#endif