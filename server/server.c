/*
 * server.c
 *
 *  Created on: 19 may. 2017
 *      Author: emanuel
 */

#include "server.h"

int main(int argc, char *argv[]) {
	int descriptor;
	struct sockaddr_in dir_srv, dir_cli;

	/*---------------------------------------------------------------------*
	 * Verificar los argumentos recibidos								   *
	 *---------------------------------------------------------------------*/
	if (argc < 2) {
		printf("Uso: servidor <puerto>\n");
		exit(-1);
	}

	/*--------------------------------------------------------------------*
	 * Inicializar el servidor                               			  *
	 *--------------------------------------------------------------------*/
	if ((descriptor = socket( AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("ERROR SOCKET: ");
		exit(-1);
	}

	bzero((char *) &dir_srv, sizeof(dir_srv));
	dir_srv.sin_family = AF_INET;
	dir_srv.sin_addr.s_addr = htonl( INADDR_ANY);
	dir_srv.sin_port = htons(atoi(argv[1]));

	if (bind(descriptor, (struct sockaddr *) &dir_srv, sizeof(dir_srv)) < 0) {
		perror("ERROR BIND: ");
		exit(-1);
	}
	printf("Servidor inicializado.\n\n");

	procesar(descriptor, (struct sockaddr *) &dir_cli, sizeof(dir_cli));

	return 0;
}

/*-----------------------------------------------------------------------*
 * procesar() - realizar la tarea específica del servidor
 *-----------------------------------------------------------------------*/
void procesar(int descriptor, struct sockaddr *dir_cli_p, socklen_t longcli) {
	int recibido;
	socklen_t longitud;
	char msg[MAXLINEA];
//	char *msg;
	printf("Procesando\n");

	for (;;) {
		longitud = longcli;
		recibido = recvfrom(descriptor, msg, MAXLINEA, 0, dir_cli_p, &longitud);
		INICIAR_SESION *iniciar_sesion = (INICIAR_SESION *) msg;
		printf("Leido %d -> %s\n", iniciar_sesion->OP, iniciar_sesion->usuario);

		// verificación
		if (strcmp(iniciar_sesion->usuario, "Ema") == 0) {
			printf("OK\n");
			CONFIRMAR *confirmar;
			confirmar = (CONFIRMAR *) malloc(sizeof(CONFIRMAR));
			confirmar->OP = M_CONFIRMAR;
			confirmar->ID_usuario = '1';
			confirmar->ID_SUB_OP = '0';
			strcpy(confirmar->mensaje, "CORRECTO");
			sendto(descriptor, (char*)confirmar, sizeof(confirmar), 0, dir_cli_p, longitud);

		} else {
			printf("ERROR\n");
			ERROR *error;
			error = (ERROR *) malloc(sizeof(ERROR));
			error->OP = M_ERROR;
			error->ID_SUB_OP_Fallo = '0';
			strcpy(error->mensaje, "USUARIO_INCORRECTO");
			sendto(descriptor, (char*)error, sizeof(error), 0, dir_cli_p, longitud);
		}

//	 printf("fin\n");

	}
}
