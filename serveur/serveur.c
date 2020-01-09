#include "../shared/includes.h"

sem_t* mutex;

void dialogueClt (int, struct sockaddr_in);

void acquitterLesFils(int signal_number)
{
	int status;
    printf("\nSIGCHLD reçu, fin du fils %d\n",signal_number);
    wait(&status);
}

void ajouterRoom(char* ip,int port){
	FILE* pf;
	pf = fopen("memoire.dat", "a");

	fprintf(pf, "%s:%d\n",ip,port);

	fclose(pf);
}

void supprimerRoom(char* ip,int port){
	FILE* pf;
	char* ligne;
	/*pf = fopen("memoire.dat", "");
	while(fscanf(pf,ligne) != EOF)
		if (!strcmp(ligne,ip+":"+port))
		{
			
		}
	fclose(pf);*/

}

char* LireRoom(){
	FILE* pf;
	char* ligne="";
	pf = fopen("memoire.dat", "r");
	if (pf != NULL)
	{
		while(!feof(pf)){
			fscanf(pf,"%s",ligne);
			printf("ligne = %s\n",ligne );
		}
	}
	else{
		printf("pf null\n");
	}
	fclose(pf);
	return ligne;
}


int main() {
	int se, sd;
	struct sockaddr_in svc, clt;
	socklen_t cltLen;

	struct sigaction newact;

	// Création de la socket de réception d’écoute des appels
	CHECK(se=socket(PF_INET, SOCK_STREAM, 0), "Can't create");
	// Préparation de l’adressage du service (d’appel)
	svc.sin_family = PF_INET;
	svc.sin_port = htons (PORT_SRV);
	svc.sin_addr.s_addr = INADDR_ANY;
	memset(&svc.sin_zero, 0, 8);
	// Association de l’adressage préparé avec la socket d’écoute
	printf("Création\n");
	CHECK(bind(se, (struct sockaddr *) &svc, sizeof svc) , "Can't bind");
	// Mise en écoute de la socket
 	printf("Ecoute\n");
 	CHECK(listen(se, 5) , "Can't calibrate");
	// Boucle permanente de service

	newact.sa_handler = acquitterLesFils;  
	CHECK(sigemptyset(&newact.sa_mask),"problème sigemptyset"); 
	newact.sa_flags = SA_RESTART;           	 
	CHECK(sigaction(SIGCHLD, &newact, NULL),"problème sigaction");

	mutex = sem_open("/mutex", O_CREAT|O_EXCL, 0644, 1);
	sem_unlink("/mutex");

	while (1) {
		// Attente d’un appel
		cltLen = sizeof(clt);
		CHECK(sd=accept(se, (struct sockaddr *)&clt, &cltLen) , "Can't connect");
		printf("Client trouvé\n");
		// Dialogue avec le client

		int pid = fork();
		if (pid == 0)
		{
			dialogueClt (sd , clt);
			shutdown(sd,SHUT_RD);
			printf("fin lecture\n");
			exit(1);
		}
		//close(sd);
	}
	close(se);
	return 0;
}  

void dialogueClt (int sd, struct sockaddr_in clt) {
	char requete[MAX_BUFF];
	char adresse[MAX_BUFF];
	char port[MAX_BUFF];
	do {
		read(sd, buffer, sizeof(buffer));
		printf("Message reçu :\n");
		sscanf (buffer, "%s : %s : %s",requete,adresse,port); 
		switch (atoi(requete)) {
			case 0 : 
				printf("Au revoir\n");
			break;
			case 1 : 
				printf("Creation Room\n");
				printf("%s\n",buffer);
				printf("%s\n",adresse);
				printf("%s\n",port);
				sem_wait(mutex);
				ajouterRoom(adresse,(int)*port);
				sem_post(mutex);
				int fullSize = strlen(adresse)+strlen(port)+2;
				char* MSG = (char *) malloc( fullSize );
    			printf("MSG\n");
    			strcpy(MSG,adresse);
    			strcat(MSG,":");
    			strcat(MSG,port);
				printf("%s\n",MSG );
				CHECK(write(sd, MSG, strlen(MSG)+1), "Can't send");
				free(MSG);
			break;
			case 2 :
				printf("Demande List\n");
				printf("?\n");
				sem_wait(mutex);
				char* liste = LireRoom();
				printf("nani\n");
				sem_post(mutex);
				CHECK(write(sd,liste , sizeof(liste)+1), "Can't send");
			break;
			case 3:
				printf("Join Room\n");
			break;
			case 4 :
				printf("Lancement partie\n");
				sem_wait(mutex);
				supprimerRoom(adresse,(int)*port);
				sem_post(mutex);
			break;
			default :
			printf("NOK : message recu %s\n", buffer); 
			break;
		}
		getchar();
	} while ( atoi(requete) != 0);
}