#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <stdio.h>
#include <string.h>
#include "../utils/definitions.h"
#include "client.h"

int main(int argc, char ** argv){
	int descriptor;
	struct sockaddr_in dir;

	/*---------------------------------------------------------------------*
	 * Verificar los argumentos
	 *---------------------------------------------------------------------*/
 	if (argc < 3) {
		printf("Uso: cliente <direccion>\n");
		printf("     Donde: <direccion> = <ip> <puerto>\n");
		exit(-1);
	}
	
	/*--------------------------------------------------------------------* 
	 * Establecer la dirección del servidor y conectarse
	 *--------------------------------------------------------------------*/
	bzero( (char *) &dir, sizeof( dir ) ); // Se blanquea toda la estructura
	dir.sin_family = AF_INET; // (Address Family)
	if ( inet_pton( AF_INET, argv[1], &dir.sin_addr ) <= 0 ) { // Se trasforma la ip de texto a tipo ip (binario).
	    perror("Error en la función inet_pton:");
	    exit(-1);
	}    
	dir.sin_port = htons(atoi(argv[2])); // Se convierte el numero de puerto a un dato de la red.

	if ( ( descriptor = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 ) {
		perror( "ERROR SOCKET:" );
		exit(-1);
	}
	
	/*---------------------------------------------------------------------*
	 * Realizar la función del cliente
	 *---------------------------------------------------------------------*/
	principal( stdin, descriptor, (struct sockaddr *)&dir, sizeof(dir) );
	return(0);
}

/*-------------------------------------------------------------------------*
 * principal()
 *-------------------------------------------------------------------------*/ 			    
int principal( FILE *fp, int sockfd, const struct sockaddr *dir, socklen_t sa ) {
	int n;
	char linea_env[MAXLINEA], linea_rcb[ MAXLINEA + 1 ];
	char nombre [] = "Lucas";

	SOLICITUD prueba;
	
	prueba.OP = '5';
	prueba.ID_Usuario = '1';
	prueba.ID_SUB_OP = '2';
	prueba.ID_Album = '0';
	prueba.ID_Archivo = '0';
	strcpy(prueba.nombre, nombre);

	/* TODO Se necesita crear el servidor que reciba este mensaje y lo pueda leer.
	 * TODO Tambien se necesita crear un mensaje de respuesta que pueda ser interpretado de este lado.
	*/
	sendto( sockfd, (void *)&prueba, sizeof(prueba), 0, dir, sa);
	n = recvfrom( sockfd, linea_rcb, MAXLINEA, 0, NULL, NULL );
	linea_rcb[ n ] = '\0';
	printf( "Recibido: %s\n", linea_rcb );

	return 0;
}
