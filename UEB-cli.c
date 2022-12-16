/**************************************************************************/
/*                                                                        */
/* P1 - UEB amb sockets TCP/IP - Part I                                   */
/* Fitxer cli.c que implementa el client de UEB sobre la capa de          */
/* transport TCP (fent crides a la "nova" interfície de la capa de        */
/* transport o "nova" interfície de sockets).                             */
/*                                                                        */
/* Autors:                                                                */
/* Data:                                                                  */
/*                                                                        */
/**************************************************************************/

#include "UEBp1v3-aUEBs.h"
#include "UEBp2-aDNSc.h"
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

/* Declaració de funcions INTERNESjjjuuyh que es fan servir en aquest fitxer     */
/* (les  definicions d'aquestes funcions es troben més avall) per així    */
/* fer-les conegudes des d'aquí fins al final d'aquest fitxer, p.e.,      */

/* int FuncioInterna(arg1, arg2...);                                      */

int main(int argc,char *argv[])
{
	int scon;
	int bytes_llegits, long1, estat;
	char NomFitx[10000], Fitx[10000], error[200];
	char iprem[16],iploc[16];
	int portrem, portloc;
	char nomDNS[100], uri[100], esquema[100];
	
	printf("Entra DNS, port i nom del fitxer per la teva peticio: \n");
	
	scanf("%s",uri ); //Usuai entra la uri
	
	//Deconstruim la uri en les diferents parts, si el port no hi es utilitzem el predeterminat
	if(desferURIm1(uri,esquema,nomDNS,&portloc,NomFitx)==3){
		portloc=3000;
	}
	
	if(DNSc_ResolDNSaIP(nomDNS, iploc, error) == -1){
		printf("Error a DNSc_ResolDNSaIP(): %s\n", error);
		exit(-1);
	}
		
	
	if((scon=UEBc_DemanaConnexio(iploc,portloc,iprem,&portrem,error))==-1){ //Creem i demanem conexió al socket servidor
		printf("%s \n",error);
		close(scon);
		exit(-1);
	}
		
	estat=UEBc_ObteFitxer(scon,NomFitx,Fitx,&long1,error); //Obtenim el fitxer demanat
	if(estat==0){
		write(1,Fitx,long1);
		int file=open(NomFitx+1, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if(write(file,Fitx,long1)==-1){
			printf("No s'ha pogut guardar el fitxer \n");
		}
	}
	else if(estat==1){
		write(2,Fitx,long1);
		printf("\n");
	}
	else if(estat!=-3){
		printf("%s \n",error);
	}
	
	sleep(20);
					
	if(UEBc_TancaConnexio(scon,error)==-1){ //Tanquem conexió
		printf("%s \n",error);
		exit(-1);
	}

	return(0);
}

/* Definició de funcions INTERNES, és a dir, d'aquelles que es faran      */
/* servir només en aquest mateix fitxer. Les seves declaracions es troben */
/* a l'inici d'aquest fitxer.                                             */


/* Desfà l'URI "uri" en les seves parts: l'esquema (protocol) "esq", el   */
/* nom DNS (o l'@IP), el "nom_host", el número de port "port" i el nom    */
/* del fitxer "nom_fitxer".                                               */
/*                                                                        */
/* L'URI ha de tenir la forma "esq://nom_host:port/nom_fitxer" o bé       */
/* sense el número de port "esq://nom_host/nom_fitxer", i llavors port    */
/* s'emplena amb el valor 0 (la resta de casos no es contemplen).         */
/*                                                                        */
/* "uri", "esq", "nom_host" i "nom_fitxer" són "strings" de C (vector de  */
/* chars imprimibles acabat en '\0') d'una longitud suficient.            */
/*                                                                        */
/* Retorna:                                                               */
/*  el nombre de parts de l'URI que s'han assignat (4 si l'URI tenia      */
/*  número de port o 3 si no en tenia.                                    */
int desferURIm1(const char *uri, char *esq, char *nom_host, int *port, char *nom_fitx)
{
 int nassignats;
 char port_str[100];
 
 strcpy(esq,"");
 strcpy(nom_host,"");
 *port = 0;
 strcpy(nom_fitx,"");

 nassignats = sscanf(uri,"%[^:]://%[^:]:%[^/]%s",esq,nom_host,port_str,nom_fitx);
 
 /*
 printf("nassignats %d\n",nassignats);
 printf("esq %s\n", esq);
 printf("nom_host %s\n", nom_host);
 printf("port_str %s\n", port_str);
 printf("nom_fitx %s\n", nom_fitx);
 */
 
 /* URIs amb #port, p.e., esq://host:port/fitx, 4 valors assignats */
 if(nassignats==4)
 {
  *port = atoi(port_str);
  return nassignats;
 }  
  
 /* URIs sense #port, p.e., esq://host/fitx, 2 valors assignats,  */
 /* i llavors es fa port = 0.                                     */
 if(nassignats==2)
 {
  *port = 0;
   nassignats = sscanf(uri,"%[^:]://%[^/]%s",esq,nom_host,nom_fitx);
   return nassignats;
 }

 return nassignats;
}
