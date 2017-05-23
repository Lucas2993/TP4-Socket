/*
 * client.c
 *
 *  Created on: 19 may. 2017
 *      Author: emanuel
 */

#include "client.h"
/**
 * Este programa es la v1 del cliente donde intenta hacer un login, pasandole el
 * usuario x linea de comandos, como tercer argumento, en este caso el servidor retornara
 * una CONFIRMACION o un ERROR segun corresponda.
 * Para que el usuario sea confimado ingrese usuario "Ema"
 */
int main(int argc, char *argv[]) {

	//	descriptor de socket
	int descriptor;

	struct sockaddr_in dir;

	/*---------------------------------------------------------------------*
	 * Verificar los argumentos
	 *---------------------------------------------------------------------*/
 	if ( argc < 4 ) {
		printf("Uso: cliente <direccion> <usuario>\n");
		printf("     Donde: <direccion> = <ip> <puerto>\n");
		exit( -1 );
	}

	/*--------------------------------------------------------------------*
	 * Establecer la dirección del servidor y conectarse
	 *--------------------------------------------------------------------*/
	bzero( (char *) &dir, sizeof( dir ) );
	// Familia de direcciones AF_INET: protocolos de Internet
	dir.sin_family = AF_INET;
	// Establece direccion IP
	if ( inet_pton( AF_INET, argv[1], &dir.sin_addr ) <= 0 ) {
	    perror("Error en la función inet_pton:");
	    exit( -1 );
	}
	// Estableciendo PUERTO
	dir.sin_port = htons(atoi(argv[2]));

	// Se crea el socket
	// AF_INET: protocolos de Internet
	// SOCK_DGRAM: Servicio sin conexión (UDP si AF_INET)
	if ( ( descriptor = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 ) {
		perror( "ERROR SOCKET:" );
		exit( -1 );
	}
	// lo que envia


	INICIAR_SESION *iniciar_sesion;
	iniciar_sesion = (INICIAR_SESION *) malloc(sizeof(INICIAR_SESION));
	iniciar_sesion->OP = M_INICIAR_SESION;

	strcpy(iniciar_sesion->usuario , argv[3]);
	printf("INICIANDO SESION...  %s\n", iniciar_sesion->usuario);

	sendto( descriptor, (char*)iniciar_sesion , sizeof(iniciar_sesion), 0, &dir, sizeof(dir));

	char msg_rcb[ MAXLINEA];
	int n = recvfrom( descriptor, msg_rcb, MAXLINEA, 0, NULL, NULL );
	msg_rcb[ n ] = '\0';
	printf("%d\n",msg_rcb[0]);
	if(msg_rcb[0] == M_CONFIRMAR){
		CONFIRMAR *confirmar = (CONFIRMAR *) msg_rcb;
		printf("confirmación : %s\n",confirmar->mensaje);
	}else{
		ERROR *error = (ERROR *) msg_rcb;
		printf("error : %s\n",error->mensaje);
	}



	printf("FIN\n");

	/*---------------------------------------------------------------------*
	 * Realizar la función del cliente
	 *---------------------------------------------------------------------*/
//	principal( stdin, descriptor, (struct sockaddr *)&dir, sizeof(dir) );

	return 0 ;
}

/*-------------------------------------------------------------------------*
 * principal()
 *-------------------------------------------------------------------------*/
int principal( FILE *fp, int sockfd, const struct sockaddr *dir, socklen_t sa ) {
//	int n;
//	char linea_env[ MAXLINEA ], linea_rcb[ MAXLINEA + 1 ];
//
//	while( fgets( linea_env, MAXLINEA, fp ) != NULL ) {
//		sendto( sockfd, linea_env, strlen( linea_env ), 0, dir, sa);
////		linea_env[strlen(linea_env)]='\0';
//		n = recvfrom( sockfd, linea_rcb, MAXLINEA, 0, NULL, NULL );
////		linea_rcb[ n ] = '\0';
//		printf( "Recibidos: %s\n", linea_rcb );
//	}
	return 0;
}

