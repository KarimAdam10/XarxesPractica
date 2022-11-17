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
                 

	while(strcmp(iploc,".")!=0 || estat==-3){ //Per sortir el client ha d'introduir un punt d'@IP
		
		printf("Entra ip, port i nom del fitxer per la teva peticio: \n");
		
		scanf("%s",iploc ); //Usuai entra l'@IP del servidor
		
		if(strcmp(iploc,".")!=0){
			scanf("%d", &portloc ); //Usuai entra el port del servidor
			if(portloc==0){
				portloc=3000;
			}
			
			scanf("%s", NomFitx ); //Usuai entra el port del servidor //Entra el nom del fitxer que vol demanar
			
			if((scon=UEBc_DemanaConnexio(iploc,portloc,iprem,&portrem,error))==-1){ //Creem i demanem conexió al socket servidor
				printf("%s \n",error);
				close(scon);
				exit(-1);
			}
				
			estat=UEBc_ObteFitxer(scon,NomFitx,Fitx,&long1,error); //Obtenim el fitxer demanat
			if(estat==0){
				write(1,Fitx,long1);
				int file=open(NomFitx, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
				if(write(file,Fitx,long1)==-1){
					printf("No s'ha pogut guardar el fitxer \n");
				}
			}
			else if(estat==1){
				printf("%s \n",error);
			}
			else if(estat!=-3){
				printf("%s \n",error);
			}
		}
	}
	
	/*sleep(20);*/
					
	if(UEBc_TancaConnexio(scon,error)==-1){ //Tanquem conexió
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
