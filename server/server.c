#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <stdio.h>
#include <string.h>
#include "../utils/definitions.h"
#include "server.h"

void main(int argc, char *argv[]) {
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

}

int id_user = 0;

/*-----------------------------------------------------------------------*
 * procesar() - realizar la tarea específica del servidor
 *-----------------------------------------------------------------------*/
void procesar(int descriptor, struct sockaddr *dir_cli_p, socklen_t longcli) {
	int recibido;
	socklen_t longitud;
	char msg[MAXLINEA];
	void* respuesta;
	int i;

	for (;;) {
		longitud = longcli;
		recibido = recvfrom(descriptor, msg, MAXLINEA, 0, dir_cli_p, &longitud);
		msg[recibido] = '\0';
		printf("mng -> %d\n",msg[0]);
		// Llamado a la funcion

		respuesta = resolver(recibido, msg);

		sendto(descriptor, (void *) respuesta, sizeof(respuesta), 0, dir_cli_p, longitud);
	}
}

//void* resolver(int longitud, char * mensaje) {
void* resolver(int longitud, char mensaje[]) {

//	char* copia_mensaje = (char *) malloc(sizeof(char) * longitud);
	char copia_msg [longitud];
	strcpy(copia_msg, mensaje);

	printf("copia mng -> %s\n",copia_msg);

	switch (copia_msg[0]) {
	case M_INICIAR_SESION:
		printf("Iniciando sesion\n");
		return iniciar_sesion(copia_msg);
		break;
	case M_REGISTRO:
		return registrar(copia_msg);
		break;
	case M_SOLICITUD:
		return solicitud(copia_msg);
		break;
	case M_CERRAR_SESION:
		return cerrar_sesion(copia_msg);
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
	return NULL;
}

void * iniciar_sesion(char mensaje[]) {
	INICIAR_SESION *iniciar_sesion = (INICIAR_SESION *) mensaje;
	printf("Leido %d -> %s\n", iniciar_sesion->OP, iniciar_sesion->usuario);
	return verificar_usuario(iniciar_sesion);
}

void * verificar_usuario(INICIAR_SESION *iniciar_sesion) {

	if (strcmp(iniciar_sesion->usuario, "Ema") == 0) {
				printf("OK\n");
				CONFIRMAR *confirmar;
				confirmar = (CONFIRMAR *) malloc(sizeof(CONFIRMAR));
				confirmar->OP = M_CONFIRMAR;
				confirmar->ID_usuario = '1';
				confirmar->ID_SUB_OP = '0';
				id_user = 1;
				char res[]="Usuario correcto";
				strcpy(confirmar->mensaje, res);
				return confirmar;

			} else {
				printf("ERROR\n");
				ERROR *error;
				error = (ERROR *) malloc(sizeof(ERROR));
				error->OP = M_ERROR;
				error->ID_SUB_OP_Fallo = '0';
				char res[] = "Usuario incorrecto";
				strcpy(error->mensaje, res);
				return error;
			}
}

void * registrar(char * mensaje) {

}

void * solicitud(char * mensaje){
	SOLICITUD * solicitud = (SOLICITUD *)mensaje;

	switch(solicitud->ID_SUB_OP){
	case SubOP_Listar_albumes:
		break;
	case SubOP_Crear_album:
		break;
	case SubOP_Modificar_album:
		break;
	case SubOP_Eliminar_album:
		break;
	case SubOP_Listar_archivos_album:
		break;
	case SubOP_Subir_archivo_album:
		break;
	case SubOP_Modificar_archivo_album:
		break;
	case SubOP_Eliminar_archivo_album:
		break;
	case SubOP_Compartir_album_usuario:
		break;
	case SubOP_Dejar_compartir_album_usuario:
		break;
	case SubOP_Listar_usuario:
		break;
	default:
		ERROR error;
		error->ID_SUB_OP_Fallo = solicitud->ID_SUB_OP;
		
		break;
	}
}

void * cerrar_sesion(char * mensaje) {

}
