#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <stdio.h>
#include <string.h>
#include "sesion.h"
#include "usuario.h"
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
	int longitud_respuesta;
	socklen_t longitud;
	char msg[MAXLINEA];
	void* respuesta;
	int i;

	for (;;) {
		longitud = longcli;
		recibido = recvfrom(descriptor, msg, MAXLINEA, 0, dir_cli_p, &longitud);
		
		// Llamado a la funcion
		respuesta = resolver(recibido, msg, &longitud_respuesta);

		sendto(descriptor, (void *) respuesta, longitud_respuesta, 0, dir_cli_p, longitud);
	}
}

void* resolver(int longitud, char * mensaje, int * longitud_respuesta) {
	switch (*mensaje) {
	case M_INICIAR_SESION:
		return iniciar_sesion(mensaje, longitud_respuesta);
		break;
	case M_REGISTRO:
		return registrar(mensaje, longitud_respuesta);
		break;
	case M_SOLICITUD:
		return solicitud(mensaje, longitud_respuesta);
		break;
	case M_CERRAR_SESION:
		return cerrar_sesion(mensaje, longitud_respuesta);
		break;
	}
	return NULL;
}

void * iniciar_sesion(char * mensaje, int * longitud_respuesta) {
	INICIAR_SESION *iniciar_sesion = (INICIAR_SESION *) mensaje;
	USUARIO * usuario;
	ERROR * mensaje_error = NULL;
	CONFIRMAR * mensaje_confirmacion = NULL;
	int numero_sesion = 0;

	usuario = buscar_usuario(iniciar_sesion->usuario);

	if(usuario != NULL){
		printf("Usuario encontrado: %s y %s\n", usuario->usuario, usuario->clave);

		if(strcmp(iniciar_sesion->clave, usuario->clave) == 0){
			
			numero_sesion = buscar_sesion_por_usuario(usuario->usuario);
			if(numero_sesion < 0){
				numero_sesion = asignar_numero_sesion();
				agregar_sesion(usuario->usuario, numero_sesion);	
			}

			mensaje_confirmacion = (CONFIRMAR *)malloc(sizeof(CONFIRMAR));

			mensaje_confirmacion->OP = M_CONFIRMAR;
			mensaje_confirmacion->ID_Usuario = numero_sesion + '0';
			mensaje_confirmacion->ID_SUB_OP = '0';
			strcpy(mensaje_confirmacion->mensaje, "Sesion iniciada correctamente!");

			*longitud_respuesta = sizeof(CONFIRMAR);

			return (void *) mensaje_confirmacion;	
		}
		else{
			mensaje_error = (ERROR *)malloc(sizeof(ERROR));

			mensaje_error->OP = M_ERROR;
			mensaje_error->ID_SUB_OP_Fallo = '0';
			strcpy(mensaje_error->mensaje, "Error: Contraseña incorrecta!");

			*longitud_respuesta = sizeof(ERROR);

			return (void *) mensaje_error;
		}
	}
	else{
		printf("Usuario no encontrado\n");
		mensaje_error = (ERROR *)malloc(sizeof(ERROR));

		mensaje_error->OP = M_ERROR;
		mensaje_error->ID_SUB_OP_Fallo = '0';
		strcpy(mensaje_error->mensaje, "Error: El usuario especificado no existe!");

		*longitud_respuesta = sizeof(ERROR);

		return (void *) mensaje_error;
	}
	return NULL;
}

void * registrar(char * mensaje, int * longitud_respuesta) {
	REGISTRO *registro = (REGISTRO *) mensaje;
	ERROR * mensaje_error = NULL;
	CONFIRMAR * mensaje_confirmacion = NULL;
	USUARIO * usuario;
	BOOLEAN resultado;

	usuario = buscar_usuario(registro->usuario);

	if(usuario == NULL){
		resultado = agregar_usuario(registro->usuario, registro->clave, registro->nombre_completo, registro->apellido);

		mensaje_confirmacion = (CONFIRMAR *)malloc(sizeof(CONFIRMAR));

		mensaje_confirmacion->OP = M_CONFIRMAR;
		mensaje_confirmacion->ID_Usuario = '0';
		mensaje_confirmacion->ID_SUB_OP = '0';
		strcpy(mensaje_confirmacion->mensaje, "Usuario registrado correctamente!");

		*longitud_respuesta = sizeof(CONFIRMAR);

		return (void *) mensaje_confirmacion;	
	}
	else{
		printf("Usuario ya existente\n");
		mensaje_error = (ERROR *)malloc(sizeof(ERROR));

		mensaje_error->OP = M_ERROR;
		mensaje_error->ID_SUB_OP_Fallo = '0';
		strcpy(mensaje_error->mensaje, "Error: El usuario especificado ya existe!");

		*longitud_respuesta = sizeof(ERROR);

		return (void *) mensaje_error;
	}
}

void * solicitud(char * mensaje, int * longitud_respuesta){
	SOLICITUD * solicitud = (SOLICITUD *)mensaje;
	ERROR error;

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
		error.ID_SUB_OP_Fallo = solicitud->ID_SUB_OP;
		
		break;
	}
}

void * cerrar_sesion(char * mensaje, int * longitud_respuesta) {
	CERRAR_SESION *cerrar_sesion = (CERRAR_SESION *) mensaje;
	ERROR * mensaje_error = NULL;
	CONFIRMAR * mensaje_confirmacion = NULL;
	BOOLEAN resultado;

	resultado = cerrar_sesion_usuario(cerrar_sesion->ID_Usuario - '0');

	if(resultado){
		mensaje_confirmacion = (CONFIRMAR *)malloc(sizeof(CONFIRMAR));

		mensaje_confirmacion->OP = M_CONFIRMAR;
		mensaje_confirmacion->ID_Usuario = '0';
		mensaje_confirmacion->ID_SUB_OP = '0';
		strcpy(mensaje_confirmacion->mensaje, "Sesion cerrada correctamente!");

		*longitud_respuesta = sizeof(CONFIRMAR);

		return (void *) mensaje_confirmacion;
	}
	else{
		printf("Error al cerrar sesion\n");
		mensaje_error = (ERROR *)malloc(sizeof(ERROR));

		mensaje_error->OP = M_ERROR;
		mensaje_error->ID_SUB_OP_Fallo = '0';
		strcpy(mensaje_error->mensaje, "Error: No se pudo cerrar la sesion");

		*longitud_respuesta = sizeof(ERROR);

		return (void *) mensaje_error;
	}
}
