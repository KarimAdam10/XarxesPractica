/**************************************************************************/
/*                                                                        */
/* P1 - UEB amb sockets TCP/IP - Part I                                   */
/* Fitxer aUEB.c que implementa la capa d'aplicació de UEB, sobre la      */
/* cap de transport TCP (fent crides a la "nova" interfície de la         */
/* capa TCP o "nova" interfície de sockets TCP), en la part servidora.    */
/*                                                                        */
/* Autors:                                                                */
/* Data:                                                                  */
/*                                                                        */
/**************************************************************************/

/* Inclusió de llibreries, p.e. #include <sys/types.h> o #include "meu.h" */
/*  (si les funcions externes es cridessin entre elles, faria falta fer   */
/*   un #include del propi fitxer capçalera)                              */

#include "UEBp1v3-aUEBs.h"
#include "UEB-tTCP.h"
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

/* Definició de constants, p.e.,                                          */

/* #define XYZ       1500                                                 */

/* Declaració de funcions INTERNES que es fan servir en aquest fitxer     */
/* (les  definicions d'aquestes funcions es troben més avall) per així    */
/* fer-les conegudes des d'aquí fins al final d'aquest fitxer, p.e.,      */

/* int FuncioInterna(arg1, arg2...);                                      */

int ConstiEnvMis(int SckCon, const char *tipus, const char *info1, int long1);
int RepiDesconstMis(int SckCon, char *tipus, char *info1, int *long1);

/* Definició de funcions EXTERNES, és a dir, d'aquelles que es cridaran   */
/* des d'altres fitxers, p.e., int UEBs_FuncioExterna(arg1, arg2...) { }  */
/* En termes de capes de l'aplicació, aquest conjunt de funcions externes */
/* formen la interfície de la capa UEB, en la part servidora.             */

/* Inicia el S UEB: crea un nou socket TCP "servidor" a una @IP local     */
/* qualsevol i al #port TCP “portTCPser”. Escriu l'identificador del      */
/* socket creat a "SckEsc".                                               */
/* Escriu un missatge que descriu el resultat de la funció a "MisRes".    */
/*                                                                        */
/* "MisRes" és un "string" de C (vector de chars imprimibles acabat en    */
/* '\0') d'una longitud màxima de 200 chars (incloent '\0').              */
/*                                                                        */
/* Retorna:                                                               */
/*  0 si tot va bé;                                                       */
/* -1 si hi ha un error en la interfície de sockets.                      */
int UEBs_IniciaServ(int *SckEsc, int portTCPser, char *MisRes)
{
	int estat=0;
	char iploc[16];
	strcpy(iploc,"0.0.0.0"); 
	if((*SckEsc=TCP_CreaSockServidor(iploc,portTCPser))==-1){ 
		sprintf(MisRes,"Error a TCP_CreaSockServidor(): %s", TCP_ObteMissError());
		estat=-1;
	}
	return estat;
}

/* Accepta una connexió d'un C UEB que arriba a través del socket TCP     */
/* "servidor" d'identificador "SckEsc". Escriu l'@IP i el #port TCP del   */
/* socket TCP "client" a "IPcli" i "portTCPcli", respectivament, i l'@IP  */
/* i el #port TCP del socket TCP "servidor" a "IPser" i "portTCPser",     */
/* respectivament.                                                        */
/* Escriu un missatge que descriu el resultat de la funció a "MisRes".    */
/*                                                                        */
/* "IPser" i "IPcli" són "strings" de C (vector de chars imprimibles      */
/* acabats en '\0') d'una longitud màxima de 16 chars (incloent '\0').    */
/* "MisRes" és un "string" de C (vector de chars imprimibles acabat en    */
/* '\0') d'una longitud màxima de 200 chars (incloent '\0').              */
/*                                                                        */
/* Retorna:                                                               */
/*  l'identificador del socket TCP connectat si tot va bé;                */
/* -1 si hi ha un error a la interfície de sockets.                       */
int UEBs_AcceptaConnexio(int SckEsc, char *IPser, int *portTCPser, char *IPcli, int *portTCPcli, char *MisRes)
{
	int scon;
	if((scon=TCP_AcceptaConnexio(SckEsc,IPcli,portTCPcli))==-1)
	{
	    sprintf(MisRes,"Error a TCP_AcceptaConnexio(): %s", TCP_ObteMissError());
	}
	if(TCP_TrobaAdrSockLoc(SckEsc,IPser,portTCPser)==-1)
	{
		sprintf(MisRes,"Error a TCP_TrobaAdrSockLoc(): %s", TCP_ObteMissError());
	    scon=-1;
	}
	return scon;
}

/* Serveix una petició UEB d'un C a través de la connexió TCP             */
/* d'identificador "SckCon". A "TipusPeticio" hi escriu el tipus de       */
/* petició (p.e., OBT) i a "NomFitx" el nom del fitxer de la petició.     */
/* Escriu un missatge que descriu el resultat de la funció a "MisRes".    */
/*                                                                        */
/* "TipusPeticio" és un "string" de C (vector de chars imprimibles acabat */
/* en '\0') d'una longitud de 4 chars (incloent '\0').                    */
/* "NomFitx" és un "string" de C (vector de chars imprimibles acabat en   */
/* '\0') d'una longitud <= 10000 chars (incloent '\0').                   */
/* "MisRes" és un "string" de C (vector de chars imprimibles acabat en    */
/* '\0') d'una longitud màxima de 200 chars (incloent '\0').              */
/*                                                                        */
/* Retorna:                                                               */
/*  0 si el fitxer existeix al servidor;                                  */
/*  1 la petició és ERRònia (p.e., el fitxer no existeix);                */
/* -1 si hi ha un error a la interfície de sockets;                       */
/* -2 si protocol és incorrecte (longitud camps, tipus de peticio, etc.); */
/* -3 si l'altra part tanca la connexió;                                  */
/* -4 si hi ha problemes amb el fitxer de la petició (p.e., nomfitxer no  */
/*  comença per /, fitxer no es pot llegir, fitxer massa gran, etc.).     */
int UEBs_ServeixPeticio(int SckCon, char *TipusPeticio, char *NomFitx, char *MisRes)
{
	int long1;
	int estat=RepiDesconstMis(SckCon,TipusPeticio,NomFitx,&long1);
	if(estat==0){
		if(NomFitx[0]!='/'){
			estat=-4;
			MisRes="Error en el fitxer de la peticio";
			estat=ConstiEnvMis(SckCon,"ERR","Error1",6);
		}
		else{
			/*strcpy(NomFitx,"primera.html");*/
			int file=open(NomFitx+1,O_RDONLY);
			if(file==-1){
				estat=1;
				MisRes="El fitxer no existeix";
				estat=ConstiEnvMis(SckCon,"ERR","Error1",6);
			}
			else{
				char buffer[10000];
				int bytes_llegits=read(file,buffer,sizeof(buffer));
				if(bytes_llegits==-1 || bytes_llegits>9999){
					estat=-4;
					MisRes="Error en el fitxer de la peticio";
					estat=ConstiEnvMis(SckCon,"ERR","Error1",6);
				}
				else{
					estat=ConstiEnvMis(SckCon,"COR",buffer,bytes_llegits);
				}
				close(file);
			}
		}
	}
	if(estat==-1){
		sprintf(MisRes,"Error a TCP_Rep(): %s", TCP_ObteMissError());
	}
	if(estat==-2){
		MisRes="Error en el protocol";
		estat=ConstiEnvMis(SckCon,"ERR","Error1",6);
	}
	return estat;
}

/* Tanca la connexió TCP d'identificador "SckCon".                        */
/* Escriu un missatge que descriu el resultat de la funció a "MisRes".    */
/*                                                                        */
/* "MisRes" és un "string" de C (vector de chars imprimibles acabat en    */
/* '\0') d'una longitud màxima de 200 chars (incloent '\0').              */
/*                                                                        */
/* Retorna:                                                               */
/*   0 si tot va bé;                                                      */
/*  -1 si hi ha un error a la interfície de sockets.                      */
int UEBs_TancaConnexio(int SckCon, char *MisRes)
{
	int estat=0;
	if(TCP_TancaSock(SckCon)==-1){
		sprintf(MisRes,"Error a TCP_TancaSock(): %s", TCP_ObteMissError());
		estat=-1;
	}
	return estat;
}

/* Si ho creieu convenient, feu altres funcions EXTERNES                  */

/* Descripció de la funció, dels arguments, valors de retorn, etc.        */
/* int UEBs_FuncioExterna(arg1, arg2...)
{
	
} */

/* Definició de funcions INTERNES, és a dir, d'aquelles que es faran      */
/* servir només en aquest mateix fitxer. Les seves declaracions es        */
/* troben a l'inici d'aquest fitxer.                                      */

/* Descripció de la funció, dels arguments, valors de retorn, etc.        */
/* int FuncioInterna(arg1, arg2...)
{
	
} */

/* "Construeix" un missatge de PUEB a partir dels seus camps tipus,       */
/* long1 i info1, escrits, respectivament a "tipus", "long1" i "info1"    */
/* (que té una longitud de "long1" bytes), i l'envia a través del         */
/* socket TCP “connectat” d’identificador “SckCon”.                       */
/*                                                                        */
/* "tipus" és un "string" de C (vector de chars imprimibles acabat en     */
/* '\0') d'una longitud de 4 chars (incloent '\0').                       */
/* "info1" és un vector de chars (bytes) qualsevol (recordeu que en C,    */
/* un char és un enter de 8 bits) d'una longitud <= 9999 bytes.           */
/*                                                                        */
/* Retorna:                                                               */
/*  0 si tot va bé;                                                       */
/* -1 si hi ha un error a la interfície de sockets;                       */
/* -2 si protocol és incorrecte (longitud camps, tipus de peticio).       */
int ConstiEnvMis(int SckCon, const char *tipus, const char *info1, int long1)
{
	int estat=0;
	if( (long1>9999) || (strcmp(tipus,"COR")!=0 && strcmp(tipus,"ERR")!=0) ){
		estat=-2;
	}
	else{
		char missatge[10006];
		char longInfo[5];
		sprintf(longInfo,"%.4d",long1);
		memcpy(missatge,tipus,3);
		memcpy(missatge+3,longInfo,4);
		memcpy(missatge+7,info1,long1);
		int longMis=7+long1;
		if(TCP_Envia(SckCon,missatge,longMis)==-1) 
		{ 
			estat=-1;
		}		
	}
	return estat;
}

/* Rep a través del socket TCP “connectat” d’identificador “SckCon” un    */
/* missatge de PUEB i el "desconstrueix", és a dir, obté els seus camps   */
/* tipus, long1 i info1, que escriu, respectivament a "tipus", "long1"    */
/* i "info1" (que té una longitud de "long1" bytes).                      */
/*                                                                        */
/* "tipus" és un "string" de C (vector de chars imprimibles acabat en     */
/* '\0') d'una longitud de 4 chars (incloent '\0').                       */
/* "info1" és un vector de chars (bytes) qualsevol (recordeu que en C,    */
/* un char és un enter de 8 bits) d'una longitud <= 9999 bytes.           */
/*                                                                        */
/* Retorna:                                                               */
/*  0 si tot va bé,                                                       */
/* -1 si hi ha un error a la interfície de sockets;                       */
/* -2 si protocol és incorrecte (longitud camps, tipus de peticio);       */
/* -3 si l'altra part tanca la connexió.                                  */
int RepiDesconstMis(int SckCon, char *tipus, char *info1, int *long1)
{
	int estat=0;	
	char missatge[10007];
	int bytes_llegits=TCP_Rep(SckCon,missatge,sizeof(missatge));
	if(bytes_llegits==-1) 
	{ 
		estat=-1;
	}	
	else if(bytes_llegits==0){
		estat=-3;
	}
	else{
		char longMis[5];
		memcpy(tipus,missatge,3);
		memcpy(longMis,missatge+3,4);
		memcpy(info1,missatge+7,bytes_llegits-7);
		tipus[3]='\0';
		longMis[4]='\0';
		*long1=atoi(longMis);
		if(sizeof(info1)>9999 || (*long1>0 && *long1<9999) || strcmp(tipus,"OBT")!=0){
			estat=-2;
		}
	}
	return estat;
}
