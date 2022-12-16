/**************************************************************************/
/*                                                                        */
/* P1 - UEB amb sockets TCP/IP - Part I                                   */
/* Fitxer ser.c que implementa el servidor de UEB sobre la capa de        */
/* transport TCP (fent crides a la "nova" interfície de la capa de        */
/* transport o "nova" interfície de sockets).                             */
/*                                                                        */
/* Autors:                                                                */
/* Data:                                                                  */
/*                                                                        */
/**************************************************************************/

#include "UEBp1v3-aUEBs.h"
#include <stdlib.h>
#include <stdio.h>  
#include <stdbool.h>  
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

/* Definició de constants, p.e.,                                          */

#define NOMBRECONNSMAX		3

/* Declaració de funcions INTERNES que es fan servir en aquest fitxer     */
/* (les  definicions d'aquestes funcions es troben més avall) per així    */
/* fer-les conegudes des d'aquí fins al final d'aquest fitxer, p.e.,      */

int AfegeixSck(int Sck, int *LlistaSck, int LongLlistaSck);
int TreuSck(int Sck, int *LlistaSck, int LongLlistaSck);

int main(int argc,char *argv[])
{
	int sesc, scon;
	int estat;
	char buffer[10000],NomFitx[10000],TipusPeticio[4];
	char iploc[16],iprem[16];
	int portloc, portrem;
	char error[200];
	FILE *fp;
	char linia[50];
	int LlistaSck[NOMBRECONNSMAX];
	int LongLlistaSck=0;
	
	fp=fopen("ser.cfg","r");
	fgets(linia,sizeof(linia),fp);
	portloc=atoi(linia+9);

	if(UEBs_IniciaServ(&sesc,portloc,error)==-1){ //Creem i guardem el socket servidor
		printf("%s \n",error);
		exit(-1);
	}   
	
	int n=-1;
	while(n<NOMBRECONNSMAX-1){
		n++;
		LlistaSck[n]=-1;
	}
	AfegeixSck(sesc,LlistaSck,LongLlistaSck);
	
	bool bucle=true;
	while(bucle){
		scon=UEBs_HaArribatAlgunaCosa(LlistaSck,LongLlistaSck,error);
		
		if(scon==sesc){
			if((scon=UEBs_AcceptaConnexio(sesc,iploc,&portloc,iprem,&portrem,error))==-1) //Acceptem conexió del client
			{
				printf("%s \n",error);
				close(sesc);
				exit(-1);
			}
			
			int ple=AfegeixSck(scon,LlistaSck,LongLlistaSck);
			
			if(ple==-1){
				if(UEBs_TancaConnexio(scon,error)==-1){
					printf("%s \n",error);
					exit(-1);
				}
			}
			
			else{
				printf("@socket client:\n");  
				printf("%s %i \n", iprem, portrem); //mostrem @IP i port del client per pantalla
				printf("@socket servidor:\n");  
				printf("%s %i \n", iploc, portloc); //mostrem @IP i port del client per pantalla
			}
		}
		
		else{
			estat=10;
			estat=UEBs_ServeixPeticio(scon,TipusPeticio,NomFitx,error); //Envia el fitxer demanat al client
			if(estat==0){
				printf("Obtenir %s \n", NomFitx);
			}
			else if(estat==-1){
			  printf("%s \n",error);
			  close(scon);
			  exit(-1);
			}
			else if(estat==1 || estat==-2 || estat==-4){
				printf("No hi ha hagut exit en obtenir %s \n", NomFitx);
			}
			else if(estat==-3){
				if(UEBs_TancaConnexio(scon,error)==-1){
					printf("%s \n",error);
					exit(-1);
				}
				TreuSck(scon,LlistaSck,LongLlistaSck);
			}
		}
		
	}
	
	if(UEBs_TancaConnexio(sesc,error)==-1){
		printf("%s \n",error);
		exit(-1);
	}
	
	return(0);
}

/* Donada la llista d'identificadors de sockets “LlistaSck” (de longitud  */
/* “LongLlistaSck” sockets), hi busca una posició "lliure" (una amb un    */
/* contingut igual a -1) i hi escriu l'identificador de socket "Sck".     */
/*                                                                        */
/* "LlistaSck" és un vector d'int d'una longitud d'almenys LongLlistaSck. */
/*                                                                        */
/* Retorna:                                                               */
/*  0 si tot va bé;                                                       */
/* -1 si hi ha error.                                                     */
int AfegeixSck(int Sck, int *LlistaSck, int LongLlistaSck)
{
	int estat=0;
	int n=-1;
	bool trobat=false;
	while(n<NOMBRECONNSMAX-1 && !trobat){
		n++;
		if(LlistaSck[n]==-1){
			LlistaSck[n]=Sck;
			LongLlistaSck++;
			trobat=true;
		}
	}
	if(!trobat){
		estat=-1;
	}
	return estat;
}

/* Donada la llista d'identificadors de sockets “LlistaSck” (de longitud  */
/* “LongLlistaSck” sockets), hi busca la posició on hi ha l'identificador */
/* de socket "Sck" i la marca com "lliure" (hi escriu un contingut igual  */
/* a -1).                                                                 */ 
/*                                                                        */
/* "LlistaSck" és un vector d'int d'una longitud d'almenys LongLlistaSck. */
/*                                                                        */
/* Retorna:                                                               */
/*  0 si tot va bé;                                                       */
/* -1 si hi ha error.                                                     */
int TreuSck(int Sck, int *LlistaSck, int LongLlistaSck)
{
	int estat=0;
	int n=-1;
	bool trobat=false;
	while(n<NOMBRECONNSMAX-1 && !trobat){
		n++;
		if(LlistaSck[n]==Sck){
			LlistaSck[n]=-1;
			LongLlistaSck--;
			trobat=true;
		}
	}
	if(!trobat){
		estat=-1;
	}
	return estat;
}
