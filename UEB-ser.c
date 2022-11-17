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

/* #define XYZ       1500                                                 */

/* Declaració de funcions INTERNES que es fan servir en aquest fitxer     */
/* (les  definicions d'aquestes funcions es troben més avall) per així    */
/* fer-les conegudes des d'aquí fins al final d'aquest fitxer, p.e.,      */

/* int FuncioInterna(arg1, arg2...);                                      */

int main(int argc,char *argv[])
{
	int sesc, scon;
	int estat;
	char buffer[10000],NomFitx[10000],TipusPeticio[4];
	char iploc[16],iprem[16];
	int portloc, portrem;
	char error[200];
 	
	/*int file=fopen(NomFitx,O_RDONLS);*/
	portloc=8000;
	if(UEBs_IniciaServ(&sesc,portloc,error)==-1){ //Creem i guardem el socket servidor
		printf("%s \n",error);
		exit(-1);
	}   
	
	bool bucle=true;
	while(bucle){
		if((scon=UEBs_AcceptaConnexio(sesc,iploc,&portloc,iprem,&portrem,error))==-1) //Acceptem conexió del client
		{
			printf("%s \n",error);
			close(sesc);
			exit(-1);
		}
		
		printf("@socket client:\n");  
		printf("%s %i \n", iprem, portrem); //mostrem @IP i port del client per pantalla
		printf("@socket servidor:\n");  
		printf("%s %i \n", iploc, portloc); //mostrem @IP i port del client per pantalla
		
		while(estat!=-3){
			int estat=UEBs_ServeixPeticio(scon,TipusPeticio,NomFitx,error); //Envia el fitxer demanat al client
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
			if(UEBs_TancaConnexio(scon,error)==-1){
				printf("%s \n",error);
				exit(-1);
			}
		}
	}

	if(UEBs_TancaConnexio(sesc,error)==-1){
		printf("%s \n",error);
		exit(-1);
	}
	
	return(0);
}

/* Definició de funcions INTERNES, és a dir, d'aquelles que es faran      */
/* servir només en aquest mateix fitxer. Les seves declaracions es troben */
/* a l'inici d'aquest fitxer.                                             */

/* Descripció de la funció, dels arguments, valors de retorn, etc.        */
/*int FuncioInterna(arg1, arg2...)
{
	
} */
