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
#include <semaphore.h>
#include <ctype.h>
#include <fcntl.h>

/**
 * \def CHECK
 * Macro fonction qui vérifie qu'un appel système s'est correctement déroulé
*/
#define CHECK(sts, msg) if ((sts) == -1) {perror(msg);exit(-1);}

/**
 * \def ADDR_SRV
 * L'adresse sur laquelle les clients tentent de se connecter au serveur de match making
*/
#define ADDR_SRV "127.0.0.1"

/**
 * \def PORT_SRV
 * Le port sur lequel le serveur de matchmaking tourne
*/
#define PORT_SRV 2048

/**
 * \def MAX_BUFF
 * La taille maximale allouée à un buffer de communication
*/
#define MAX_BUFF 1024
char buffer[MAX_BUFF];

#endif