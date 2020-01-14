/**
 * \file cltNet.h
 * \brief Déclaration pour la lib networking
 * \author Thomas De Maen
 * \version 1.0
 * \date 14 Janvier 2020
 * 
 * Il s'agit du fichier de délaration des différentes structures de données et fonctions
 * utilisées pour le bon fonctionnement de la librairie Networking.
*/
#ifndef _H_CLTNET_
#define _H_CLTNET_

#include "../../shared/includes.h"
#include <poll.h>

/**
 * \def SERVER_PORT
 * Numéro de port de l'hôte d'une partie
*/
#define SERVER_PORT 6666

/**
 * \def READ_TO
 * Nombre de milisecondes avant qu'une opération de lecture time out
*/
#define READ_TO 500

/**
 * \struct netPacket
 * \def packet_t
 * Structure de données représentant un paquet de l'application
*/
typedef struct netPacket {
    int8_t size;
    u_int8_t flag;
    u_int8_t data[20];
} packet_t;

/**
 * \struct netConnectionInfo
 * \def connectionInfo_t
 * Structure de données contenant les informations de connexion de l'hôte
*/
typedef struct netConnectionInfo {
    u_int16_t port;
    char* ipaddr;
} connectionInfo_t;

/**
 * \fn createServer (void)
 * Fonction permettant à une application cliente d'héberger une partie en P2P
*/
void createServer();

/**
 * \fn joinServer (connectionInfo_t* pInfos)
 * Fonction permettant à une application cliente de rejoindre une partie hébergée
 * \param pInfos Pointeur sur les informations de connexion au serveur
*/
void joinServer(connectionInfo_t*);

/**
 * \fn closeServer (void)
 * Fonction permettant à l'hôte de fermer son serveur
*/
void closeServer();

/**
 * \fn disconnect (void)
 * Fonction permettant à un client de se déconnecter d'un hôte
*/
void disconnect();


/**
 * \fn netSend (packet_t p)
 * Fonction permettant d'envoyer des données sur le réseau, il est nécessaire qu'une connexion
 * soit active afin d'être utilisée
 * \param p Paquet à envoyer au travers d'une connexion TCP
*/
void netSend(packet_t);

/**
 * \fn netRead (void)
 * Fonction permettant de lire des données depuis le réseau, il est nécessaire qu'une connexion
 * soit active afin d'être utilisée
 * \return Le paquet qui a été lu depuis une connexion TCP
*/
packet_t netRead();


/**
 * \fn isHost (void)
 * Fonction permettant à une application cliente de savoir si elle est hôte
 * \return 1 si l'appellant est l'hôte d'un serveur et 0 sinon
*/
u_int8_t isHost();

/**
 * \fn isConnected (void)
 * Fonction permettant à une application de savoir si elle est connectée
 * \return 1 si l'appellant est connecté (ou est l'hôte) à un serveur et 0 sinon
*/
u_int8_t isConnected();

/**
 * \fn hasClient (void)
 * Fonction permettant à une application cliente de savoir si elle a des clients connectés
 * \return 1 si l'appellant est un hôte avec au moins un client connecté et 0 sinon
*/
u_int8_t hasClient();

#endif