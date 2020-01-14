#include "../shared/includes.h"

void dialogueSrv (int, struct sockaddr_in, int);

int main(int argc, char** argv) {
	int sock;
	struct sockaddr_in svc;

	if(argc != 3 ){
		printf("Le format n'est pas bon le bon format:\n./clt ip port\n");
		return 1;
	}
	// Création de la socket d’appel et de dialogue
	printf("Création socket\n");
	CHECK(sock=socket(PF_INET, SOCK_STREAM, 0), "Can't create");
	// Préparation de l’adressage du service à contacter
	 svc.sin_family = PF_INET;
	 svc.sin_port = htons (PORT_SRV);
	 svc.sin_addr.s_addr = inet_addr(argv[1]);
	 memset(&svc.sin_zero, 0, 8);
	// Demande d’une connexion au service
	printf("Demande d'un connexion\n");
	CHECK(connect(sock, (struct sockaddr *)&svc, sizeof svc) , "Can't connect");
	// Dialogue avec le serveur
	dialogueSrv (sock, svc, atoi(argv[2]));
	shutdown(sock,SHUT_WR);
	printf("fin écriture\n");
	//close(sock);
	return 0;
}

void dialogueSrv (int sd, struct sockaddr_in srv,int mode) {
	char reponse[MAX_BUFF];
	char* MSG;
	if (mode == 1)
	{
		printf("demande list\n");
		MSG = "2 : 192.168.152.26 : 2082";
		CHECK(write(sd, MSG, strlen(MSG)+1), "Can't send");
		memset(reponse,0,sizeof(reponse));
		CHECK(read(sd, reponse, sizeof(reponse)), "Can't send");
		printf("%s\n",reponse );
		getchar();
		printf("\nRejoindre\n");
		MSG = "3 : 1";
		CHECK(write(sd, MSG, strlen(MSG)+1), "Can't send");
		memset(reponse,0,sizeof(reponse));
		CHECK(read(sd, reponse, sizeof(reponse)), "Can't send");
		printf("%s\n",reponse );
		getchar();
	}else{
		if (mode == 2)
		{
			MSG = "5 : 1 : 383";
			CHECK(write(sd, MSG, strlen(MSG)+1), "Can't send");
			CHECK(read(sd, reponse, sizeof(reponse)), "Can't send");
			printf("%s\n",reponse );
		}else{
			MSG = "1 : 192.168.152.26 : 2082";
			CHECK(write(sd, MSG, strlen(MSG)+1), "Can't send");
			CHECK(read(sd, reponse, sizeof(reponse)), "Can't send");
			printf("%s\n",reponse );
		}
	}
	printf("Message Bye\n");
	MSG ="0 : BYE";
	CHECK(write(sd, MSG, strlen(MSG)+1), "Can't send");
} 