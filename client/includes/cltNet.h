#ifndef _H_CLTNET_
#define _H_CLTNET_

#include "../../shared/includes.h"
#include <poll.h>

/**
 * Numéro de port de l'hôte
*/
#define SERVER_PORT 6666

/**
 * Nombre de milisecondes avant qu'une opération de lecture time out
*/
#define READ_TO 500

/**
 * Structure de données représentant un paquet de l'application
*/
typedef struct netPacket {
    u_int8_t size;
    u_int8_t flag;
    u_int8_t data[20];
} packet_t;

/**
 * Structure de données contenant les informations de connexion de l'hôte
*/
typedef struct netConnectionInfo {
    u_int16_t port;
    char* ipaddr;
} connectionInfo_t;

/**
 * Fonction permettant à une application cliente d'héberger une partie en P2P
*/
void createServer();

/**
 * Fonction permettant à une application cliente de rejoindre une partie hébergée
*/
void joinServer(connectionInfo_t*);

/**
 * Fonction permettant à l'hôte de fermer son serveur
*/
void closeServer();

/**
 * Fonction permettant à un client de se déconnecter d'un hôte
*/
void disconnect();


/**
 * Fonction permettant d'envoyer des données sur le réseau
*/
void netSend(packet_t);

/**
 * Fonction permettant de lire des données depuis le réseau
*/
packet_t netRead();


/**
 * Fonction permettant à une application cliente de savoir si elle est hôte
*/
u_int8_t isHost();

/**
 * Fonction permettant à une application de savoir si elle est connectée
*/
u_int8_t isConnected();

/**
 * Fonction permettant à une application cliente de savoir si elle a des clients connectés
*/
u_int8_t hasClient();

#endif