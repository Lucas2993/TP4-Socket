/*
 * client.c
 *
 *  Created on: 19 may. 2017
 *      Author: emanuel
 */

#include "client.h"

int main(int argc, char *argv[]) {

	INICIAR_SESION *iniciar_sesion;
	iniciar_sesion = (INICIAR_SESION *) malloc(sizeof(INICIAR_SESION));
	iniciar_sesion->OP = M_INICIAR_SESION;

	char* usuario;
	char* clave;

	usuario = (char*) malloc(MAX_USUARIO * (sizeof(char)));
	usuario = "EMA";
	iniciar_sesion->usuario = usuario;

	clave = (char*) malloc(MAX_CLAVE * (sizeof(char)));
	clave = "CLAVE";
	iniciar_sesion->clave= clave;

	printf("INICIANDO SESION...  %s -> %s\n", iniciar_sesion->usuario,iniciar_sesion->clave);
	int descriptor;
	struct sockaddr_in dir;

	/*---------------------------------------------------------------------*
	 * Verificar los argumentos
	 *---------------------------------------------------------------------*/
 	if ( argc < 3 ) {
		printf("Uso: cliente <direccion>\n");
		printf("     Donde: <direccion> = <ip> <puerto>\n");
		exit( -1 );
	}

	/*--------------------------------------------------------------------*
	 * Establecer la dirección del servidor y conectarse
	 *--------------------------------------------------------------------*/
	bzero( (char *) &dir, sizeof( dir ) );
	dir.sin_family = AF_INET;
	if ( inet_pton( AF_INET, argv[1], &dir.sin_addr ) <= 0 ) {
	    perror("Error en la función inet_pton:");
	    exit( -1 );
	}
	dir.sin_port = htons(atoi(argv[2]));

	if ( ( descriptor = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 ) {
		perror( "ERROR SOCKET:" );
		exit( -1 );
	}

	/*---------------------------------------------------------------------*
	 * Realizar la función del cliente
	 *---------------------------------------------------------------------*/
	principal( stdin, descriptor, (struct sockaddr *)&dir, sizeof(dir) );
	return 0 ;
}

/*-------------------------------------------------------------------------*
 * principal()
 *-------------------------------------------------------------------------*/
int principal( FILE *fp, int sockfd, const struct sockaddr *dir, socklen_t sa ) {
	int n;
	char linea_env[ MAXLINEA ], linea_rcb[ MAXLINEA + 1 ];

	while( fgets( linea_env, MAXLINEA, fp ) != NULL ) {
		sendto( sockfd, linea_env, strlen( linea_env ), 0, dir, sa);
//		linea_env[strlen(linea_env)]='\0';
		n = recvfrom( sockfd, linea_rcb, MAXLINEA, 0, NULL, NULL );
		linea_rcb[ n ] = '\0';
		printf( "Recibidos: %s\n", linea_rcb );
	}
	return 0;
}

