#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <stdio.h>
#include <string.h>
#include "../utils/definitions.h"
#include "server.h"

void main ( int argc, char *argv[] ) {
	int descriptor;
	struct sockaddr_in dir_srv, dir_cli;

	/*---------------------------------------------------------------------*
	 * Verificar los argumentos recibidos								   *
	 *---------------------------------------------------------------------*/
	if ( argc < 2 ) {
		printf( "Uso: servidor <puerto>\n" );
		exit(-1);
	}

	/*--------------------------------------------------------------------*
	 * Inicializar el servidor                               			  *
	 *--------------------------------------------------------------------*/
	if ( ( descriptor = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 ) {
		perror("ERROR SOCKET: ");
		exit(-1);
	}

	bzero( (char *) &dir_srv, sizeof( dir_srv ) );
	dir_srv.sin_family = AF_INET;
	dir_srv.sin_addr.s_addr = htonl( INADDR_ANY );
	dir_srv.sin_port = htons( atoi( argv[1] ) );

	if ( bind( descriptor, (struct sockaddr *) &dir_srv, sizeof( dir_srv ) ) < 0 ) {
		perror("ERROR BIND: ");
		exit (-1);
	}
	printf( "Servidor inicializado.\n\n" );

	procesar( descriptor, (struct sockaddr *) &dir_cli, sizeof( dir_cli ) );

}

/*-----------------------------------------------------------------------*
 * procesar() - realizar la tarea específica del servidor
 *-----------------------------------------------------------------------*/
void procesar( int descriptor, struct sockaddr *dir_cli_p, socklen_t longcli ) {
	int recibido;
	socklen_t longitud;
	char msg[ MAXLINEA ];
	void respuesta;
	int i;

	for(;;) {
		longitud = longcli;
		recibido = recvfrom( descriptor, msg, MAXLINEA, 0, dir_cli_p, &longitud );
		msg[recibido] = '\0';
		// Llamado a la funcion
		respuesta = resolver(recibido, msg);

		sendto( descriptor, respuesta, sizeof(respuesta), 0, dir_cli_p, longitud );
	}
}

void * resolver(int longitud, char * mensaje){
	char * copia_mensaje = (char *)malloc(sizeof(char)*longitud);
	SOLICITUD * prueba;

	strcpy(copia_mensaje, mensaje);

	switch(*copia_mensaje-'0'){
	case M_INICIAR_SESION:
		return iniciar_sesion(copia_mensaje);
		break;
	case M_REGISTRO:
		return registrar(copia_mensaje);
		break;
	case M_SOLICITUD:
		return solicitud(copia_mensaje);
		break;
	case M_CERRAR_SESION:
		return cerrar_sesion(copia_mensaje);
		break;
	}

//	if(*mensaje == '5'){
//		prueba = (SOLICITUD *) copia_mensaje;
//		printf("OP = %c\n", prueba->OP);
//		printf("ID_Usuario = %c\n", prueba->ID_Usuario);
//		printf("ID_SUB_OP = %c\n", prueba->ID_SUB_OP);
//		printf("ID_Album = %c\n", prueba->ID_Album);
//		printf("ID_Archivo = %c\n", prueba->ID_Archivo);
//		printf("Nombre = %s\n", prueba->nombre);
//	}
//	else{
//		printf("No funco...\n");
//	}

}

void * iniciar_sesion(char * mensaje){

}

void * registrar(char * mensaje){
	
}

void * solicitud(char * mensaje){
	
}

void * cerrar_sesion(char * mensaje){
	
}