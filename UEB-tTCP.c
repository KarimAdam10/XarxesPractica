/**************************************************************************/
/*                                                                        */
/* P1 - UEB amb sockets TCP/IP - Part I                                   */
/* Fitxer tTCP.c que "implementa" la capa de transport TCP, o més         */
/* ben dit, que encapsula les funcions de la interfície de sockets        */
/* TCP, en unes altres funcions més simples i entenedores: la "nova"      */
/* interfície de sockets TCP.                                             */
/*                                                                        */
/* Autors:                                                                */
/* Data:                                                                  */
/*                                                                        */
/**************************************************************************/

/* Inclusió de llibreries, p.e. #include <sys/types.h> o #include "meu.h" */
/*  (si les funcions externes es cridessin entre elles, faria falta fer   */
/*   un #include del propi fitxer capçalera)                              */

#include "UEB-tTCP.h"
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>

/* Definició de constants, p.e.,                                          */

/* #define XYZ       1500                                                 */

/* Declaració de funcions INTERNES que es fan servir en aquest fitxer     */
/* (les  definicions d'aquestes funcions es troben més avall) per així    */
/* fer-les conegudes des d'aquí fins al final d'aquest fitxer, p.e.,      */

/* int FuncioInterna(arg1, arg2...);                                      */

/* Definició de funcions EXTERNES, és a dir, d'aquelles que es cridaran   */
/* des d'altres fitxers, p.e., int TCP_FuncioExterna(arg1, arg2...) { }   */
/* En termes de capes de l'aplicació, aquest conjunt de funcions externes */
/* són la "nova" interfície de la capa de transport TCP (la "nova"        */
/* interfície de sockets TCP).                                            */

/* Crea un socket TCP “client” a l’@IP “IPloc” i #port TCP “portTCPloc”   */
/* (si “IPloc” és “0.0.0.0” i/o “portTCPloc” és 0 es fa/farà una          */
/* assignació implícita de l’@IP i/o del #port TCP, respectivament).      */
/*                                                                        */
/* "IPloc" és un "string" de C (vector de chars imprimibles acabat en     */
/* '\0') d'una longitud màxima de 16 chars (incloent '\0').               */
/*                                                                        */
/* Retorna -1 si hi ha error; l’identificador del socket creat si tot     */
/* va bé.                                                                 */
int TCP_CreaSockClient(const char *IPloc, int portTCPloc)
{
	int i;
	struct sockaddr_in adrrem;
	int scon=(socket(AF_INET,SOCK_STREAM,0));
	if(scon!=-1){
		adrrem.sin_family=AF_INET;
		adrrem.sin_port=htons(portTCPloc);
		adrrem.sin_addr.s_addr= inet_addr(IPloc);
		for(i=0;i<8;i++){adrrem.sin_zero[i]='\0';}
		
		int nbind=(bind(scon,(struct sockaddr*)&adrrem,sizeof(adrrem)));
		if(nbind==-1) scon=-1;
	}
	return scon;
}

/* Crea un socket TCP “servidor” (o en estat d’escolta – listen –) a      */
/* l’@IP “IPloc” i #port TCP “portTCPloc” (si “IPloc” és “0.0.0.0” i/o    */
/* “portTCPloc” és 0 es fa una assignació implícita de l’@IP i/o del      */
/* #port TCP, respectivament).                                            */
/*                                                                        */
/* "IPloc" és un "string" de C (vector de chars imprimibles acabat en     */
/* '\0') d'una longitud màxima de 16 chars (incloent '\0').               */
/*                                                                        */
/* Retorna -1 si hi ha error; l’identificador del socket creat si tot     */
/* va bé.                                                                 */
int TCP_CreaSockServidor(const char *IPloc, int portTCPloc)
{
	int i;
	struct sockaddr_in adrloc;
	int sesc=socket(AF_INET,SOCK_STREAM,0);
	if(sesc!=-1){
		adrloc.sin_family=AF_INET;
		adrloc.sin_port=htons(portTCPloc);
		adrloc.sin_addr.s_addr=inet_addr(IPloc);  
		for(i=0;i<8;i++){adrloc.sin_zero[i]='\0';}
		
		if((bind(sesc,(struct sockaddr*)&adrloc,sizeof(adrloc)))==-1) sesc=-1;
	}
	if(sesc!=-1){
		if((listen(sesc,3))==-1) sesc=-1;
	}
	return sesc;
}

/* El socket TCP “client” d’identificador “Sck” es connecta al socket     */
/* TCP “servidor” d’@IP “IPrem” i #port TCP “portTCPrem” (si tot va bé    */
/* es diu que el socket “Sck” passa a l’estat “connectat” o establert     */
/* – established –). Recordeu que això vol dir que s’estableix una        */
/* connexió TCP (les dues entitats TCP s’intercanvien missatges           */
/* d’establiment de la connexió).                                         */
/*                                                                        */
/* "IPrem" és un "string" de C (vector de chars imprimibles acabat en     */
/* '\0') d'una longitud màxima de 16 chars (incloent '\0').               */
/*                                                                        */
/* Retorna -1 si hi ha error; 0 si tot va bé.                             */
int TCP_DemanaConnexio(int Sck, const char *IPrem, int portTCPrem)
{
	struct sockaddr_in adrrem;
	adrrem.sin_family=AF_INET;
	adrrem.sin_port=htons(portTCPrem);
	adrrem.sin_addr.s_addr= inet_addr(IPrem);
	int nconnect=(connect(Sck,(struct sockaddr*)&adrrem,sizeof(adrrem)));
	return nconnect;
}

/* El socket TCP “servidor” d’identificador “Sck” accepta fer una         */
/* connexió amb un socket TCP “client” remot, i crea un “nou” socket,     */
/* que és el que es farà servir per enviar i rebre dades a través de la   */
/* connexió (es diu que aquest nou socket es troba en l’estat “connectat” */
/* o establert – established –; el nou socket té la mateixa adreça que    */
/* “Sck”).                                                                */
/*                                                                        */
/* Omple “IPrem” i “portTCPrem*” amb respectivament, l’@IP i el #port     */
/* TCP del socket remot amb qui s’ha establert la connexió.               */
/*                                                                        */
/* "IPrem" és un "string" de C (vector de chars imprimibles acabat en     */
/* '\0') d'una longitud màxima de 16 chars (incloent '\0').               */
/*                                                                        */
/* Retorna -1 si hi ha error; l’identificador del socket connectat creat  */
/* si tot va bé.                                                          */
int TCP_AcceptaConnexio(int Sck, char *IPrem, int *portTCPrem)
{	
	struct sockaddr_in adrrem;
	socklen_t long_adrrem=sizeof(adrrem);
	int scon=accept(Sck,(struct sockaddr*)&adrrem, &long_adrrem);
	if(scon!=-1){
		*portTCPrem=ntohs(adrrem.sin_port);
		strcpy(IPrem,inet_ntoa(adrrem.sin_addr));
	}
	return scon;
}

/* Envia a través del socket TCP “connectat” d’identificador “Sck” la     */
/* seqüència de bytes escrita a “SeqBytes” (de longitud “LongSeqBytes”    */
/* bytes) cap al socket TCP remot amb qui està connectat.                 */
/*                                                                        */
/* "SeqBytes" és un vector de chars qualsevol (recordeu que en C, un      */
/* char és un enter de 8 bits) d'una longitud >= LongSeqBytes bytes.      */
/*                                                                        */
/* Retorna -1 si hi ha error; el nombre de bytes enviats si tot va bé.    */
int TCP_Envia(int Sck, const char *SeqBytes, int LongSeqBytes)
{
	int bytes_escrits=write(Sck,SeqBytes,LongSeqBytes);
	return bytes_escrits;
}

/* Rep a través del socket TCP “connectat” d’identificador “Sck” una      */
/* seqüència de bytes que prové del socket remot amb qui està connectat,  */
/* i l’escriu a “SeqBytes” (que té una longitud de “LongSeqBytes” bytes), */
/* o bé detecta que la connexió amb el socket remot ha estat tancada.     */
/*                                                                        */
/* "SeqBytes" és un vector de chars qualsevol (recordeu que en C, un      */
/* char és un enter de 8 bits) d'una longitud >= LongSeqBytes bytes.      */
/*                                                                        */
/* Retorna -1 si hi ha error; 0 si la connexió està tancada; el nombre    */
/* de bytes rebuts si tot va bé.                                          */
int TCP_Rep(int Sck, char *SeqBytes, int LongSeqBytes)
{
	LongSeqBytes=read(Sck,SeqBytes,LongSeqBytes);
	if(LongSeqBytes==-1 && errno==104){
		LongSeqBytes=0;
	}
	return LongSeqBytes;
}

/* S’allibera (s’esborra) el socket TCP d’identificador “Sck”; si “Sck”   */
/* està connectat es tanca la connexió TCP que té establerta.             */
/*                                                                        */
/* Retorna -1 si hi ha error; 0 si tot va bé.                             */
int TCP_TancaSock(int Sck)
{
	int nclose=close(Sck);
	return nclose;
}

/* Donat el socket TCP d’identificador “Sck”, troba l’adreça d’aquest     */
/* socket, omplint “IPloc” i “portTCPloc” amb respectivament, la seva     */
/* @IP i #port TCP.                                                       */
/*                                                                        */
/* "IPloc" és un "string" de C (vector de chars imprimibles acabat en     */
/* '\0') d'una longitud màxima de 16 chars (incloent '\0').               */
/*                                                                        */
/* Retorna -1 si hi ha error; 0 si tot va bé.                             */
int TCP_TrobaAdrSockLoc(int Sck, char *IPloc, int *portTCPloc)
{
	int retorn=0;
	struct sockaddr_in local;
	socklen_t long_local=sizeof(local);
	if((retorn=getsockname(Sck,(struct sockaddr*)&local, &long_local))!=-1){
		*portTCPloc=ntohs(local.sin_port);
		strcpy(IPloc,inet_ntoa(local.sin_addr));
	}
	return retorn;
}

/* Donat el socket TCP “connectat” d’identificador “Sck”, troba l’adreça  */
/* del socket remot amb qui està connectat, omplint “IPrem” i             */
/* “portTCPrem*” amb respectivament, la seva @IP i #port TCP.             */
/*                                                                        */
/* "IPrem" és un "string" de C (vector de chars imprimibles acabat en     */
/* '\0') d'una longitud màxima de 16 chars (incloent '\0').               */
/*                                                                        */
/* Retorna -1 si hi ha error; 0 si tot va bé.                             */
int TCP_TrobaAdrSockRem(int Sck, char *IPrem, int *portTCPrem)
{
	int retorn=0;
	struct sockaddr_in rem;
	socklen_t long_rem=sizeof(rem);
	if((retorn=getpeername(Sck,(struct sockaddr*)&rem, &long_rem))!=-1){
		*portTCPrem=ntohs(rem.sin_port);
		strcpy(IPrem,inet_ntoa(rem.sin_addr));
	}
	return retorn;
}

/* Obté un missatge de text que descriu l'error produït en la darrera     */
/* crida de sockets TCP.                                                  */
/*                                                                        */
/* Retorna aquest missatge de text en un "string" de C (vector de chars   */
/* imprimibles acabat en '\0')                                            */
char* TCP_ObteMissError(void)
{
 return strerror(errno);
} 

/* Examina simultàniament durant "Temps" (en [ms]) els sockets (poden ser */
/* TCP, UDP i teclat -stdin-) amb identificadors en la llista “LlistaSck” */
/* (de longitud “LongLlistaSck” sockets) per saber si hi ha arribat       */
/* alguna cosa per ser llegida. Si Temps és -1, s'espera indefinidament   */
/* fins que arribi alguna cosa.                                           */
/*                                                                        */
/* "LlistaSck" és un vector d'int d'una longitud d'almenys LongLlistaSck. */
/*                                                                        */
/* Retorna:                                                               */
/*  l'identificador del socket a través del qual ha arribat alguna cosa;  */
/*  -1 si hi ha error;                                                    */
/*  -2 si passa "Temps" sense que arribi res.                             */
int TCP_HaArribatAlgunaCosaEnTemps(const int *LlistaSck, int LongLlistaSck, int Temps)
{
	int estat,descmax=0,i=-1;
	fd_set conjunt; 
	FD_ZERO(&conjunt);
	int mida=sizeof(LlistaSck)-1;
	while(i<mida){
		i++;
		if(LlistaSck[i]!=-1) FD_SET(LlistaSck[i],&conjunt);
		if(LlistaSck[i]>descmax) descmax=LlistaSck[i];
	}
	if(Temps==-1){
		estat=select(descmax+1,&conjunt, NULL, NULL, NULL);
	}
	else {
		struct timeval tv;
		tv.tv_sec = Temps/1000;
		tv.tv_usec = Temps*1000;
		estat=select(descmax+1,&conjunt, NULL, NULL, &tv);
	}
	if(estat==0){
		estat=-2;
	}
	else if(estat>0){
		i=-1;
		while(i<mida && estat!=LlistaSck[i]){
			i++;
			if(FD_ISSET(LlistaSck[i],&conjunt)) estat = LlistaSck[i];
		}
	}
	return estat;
}


/* Si ho creieu convenient, feu altres funcions EXTERNES                  */

/* Descripció de la funció, dels arguments, valors de retorn, etc.        */
/* int TCP_FuncioExterna(arg1, arg2...)
{
	
} */

/* Definició de funcions INTERNES, és a dir, d'aquelles que es faran      */
/* servir només en aquest mateix fitxer. Les seves declaracions es troben */
/* a l'inici d'aquest fitxer.                                             */

/* Descripció de la funció, dels arguments, valors de retorn, etc.        */
/* int FuncioInterna(arg1, arg2...)
{
	
} */