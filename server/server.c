#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <stdio.h>
#include <string.h>
#include "sesion.h"
#include "usuario.h"
#include "../utils/definitions.h"
#include "album.h"
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

	crear_carpeta_general_albumes();


	for (;;) {
		longitud = longcli;
		// Recibe una solicitud del cliente
		recibido = recvfrom(descriptor, msg, MAXLINEA, 0, dir_cli_p, &longitud);
		// Llamado a la funcion
		respuesta = resolver(recibido, msg, &longitud_respuesta);
		// Retorna la respuesta al cliente.
		sendto(descriptor, (void *) respuesta, longitud_respuesta, 0, dir_cli_p, longitud);
	}
}

/*--------------------------------------------------------------------*
 * Se ocupa de resolver las peticiones del cliente                    *
 *--------------------------------------------------------------------*/
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
	return mensaje_error(M_ERROR , '0' ,"Error: servicio invalido!", longitud_respuesta);
}


/*--------------------------------------------------------------------*
 * Inicio de sesion 															                    *
 *--------------------------------------------------------------------*/
void * iniciar_sesion(char * mensaje, int * longitud_respuesta) {
	INICIAR_SESION *iniciar_sesion = (INICIAR_SESION *) mensaje;
	USUARIO * usuario;
	int numero_sesion = 0;

	usuario = buscar_usuario(iniciar_sesion->usuario);

	// Si el usuario no existe
	if(usuario == NULL){
		printf("Error: El usuario especificado no existe!\n");
		return mensaje_error(M_ERROR , '0' ,"Error: El usuario especificado no existe!", longitud_respuesta);
	}
		printf("Usuario encontrado: %s y %s\n", usuario->usuario, usuario->clave);

		//Si la contraseña es incorrecta
		if(strcmp(iniciar_sesion->clave, usuario->clave) != 0){
			printf("Error: Contraseña incorrecta!\n");
			return mensaje_error(M_ERROR , '0' ,"Error: Contraseña incorrecta!", longitud_respuesta);
		}
		// Si no hubo error se inicia la sesion.
		numero_sesion = buscar_sesion_por_usuario(usuario->usuario);
		if(numero_sesion < 0){
			numero_sesion = asignar_numero_sesion();
			agregar_sesion(usuario->usuario, numero_sesion);
		}
		return mensaje_confirmacion(M_CONFIRMAR ,numero_sesion+'0', '0' , "Sesion iniciada correctamente!" , longitud_respuesta);
}


/*--------------------------------------------------------------------*
 * Registro de usuarios														                    *
 *--------------------------------------------------------------------*/
void * registrar(char * mensaje, int * longitud_respuesta) {
	REGISTRO *registro = (REGISTRO *) mensaje;
	USUARIO * usuario;
	BOOLEAN usuario_esta_agregado;

	usuario = buscar_usuario(registro->usuario);

	// Si el usuario ya existe , no debe crearlo
	if(usuario != NULL){
			printf("Usuario ya existente\n");
			return mensaje_error(M_ERROR , '0' ,"Error: El usuario especificado ya existe!", longitud_respuesta);
	}
		// Intenta agregar al usuario.
		usuario_esta_agregado = agregar_usuario(registro->usuario, registro->clave, registro->nombre_completo, registro->apellido);
		// Si el archivo no pudo ser accedido.
		if( !usuario_esta_agregado ){
			return mensaje_error(M_ERROR , '0' ,"Error: El archivo usuarios.txt no pudo ser abiero", longitud_respuesta);
		}
		return mensaje_confirmacion(M_CONFIRMAR ,'0', '0' , "Usuario registrado correctamente!" , longitud_respuesta);
}


/*--------------------------------------------------------------------*
 * Se ocupa de resolver las sub-operaciones del cliente               *
 *--------------------------------------------------------------------*/
void * solicitud(char * mensaje, int * longitud_respuesta){
	SOLICITUD * solicitud = (SOLICITUD *)mensaje;

	// TODO Verificar que no esta al pedo
	if(!validar_usuario( solicitud->ID_Usuario - '0' ) ){
			return mensaje_error(M_ERROR , '0' ,"Error: Usuario invalido", longitud_respuesta);
	}

	switch(solicitud->ID_SUB_OP){

	case SubOP_Listar_albumes:
		break;
	case SubOP_Crear_album:
			return subOP_crear_album(solicitud,longitud_respuesta);
		break;
	case SubOP_Modificar_album:
		break;
	case SubOP_Eliminar_album:
			return subOP_eliminar_album( solicitud,longitud_respuesta );
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
			return mensaje_error(M_ERROR , '0' ,"Error: El codigo de sub operacion no existe", longitud_respuesta);
		break;
	}
}


/*--------------------------------------------------------------------*
 * Cerrar sesion de usuario 											                    *
 *--------------------------------------------------------------------*/
void * cerrar_sesion(char * mensaje, int * longitud_respuesta) {
	CERRAR_SESION *cerrar_sesion = (CERRAR_SESION *) mensaje;
	BOOLEAN usuario_cerro_sesion;

	usuario_cerro_sesion = cerrar_sesion_usuario(cerrar_sesion->ID_Usuario - '0');

	// Si el usuario no pudo cerrar sesion
	if(!usuario_cerro_sesion ){
			return mensaje_error(M_ERROR , '0' ,"Error: No se pudo cerrar la sesion", longitud_respuesta);
	}
	return mensaje_confirmacion(M_CONFIRMAR ,'0', '0' , "Sesion cerrada correctamente!", longitud_respuesta);
}



/*--------------------------------------------------------------------*
 * Crear un album 							 								 	                   *
 *--------------------------------------------------------------------*/
void * subOP_crear_album( SOLICITUD * solicitud , int * longitud_respuesta ){
	char * nombre_usuario;
	nombre_usuario = buscar_usuario_por_sesion(solicitud->ID_Usuario - '0');
		// TODO validacion de errorer de crear_album
		if(crear_album(solicitud->nombre, nombre_usuario)){
			// TODO validacion de errores de registrar_album
			if(registrar_album(solicitud->nombre, nombre_usuario)){
				return mensaje_confirmacion(M_CONFIRMAR ,solicitud->ID_Usuario, SubOP_Crear_album , "Album creado correctamente!", longitud_respuesta);
			}
		}

}// fin subOP_crear_album

void * subOP_eliminar_album( SOLICITUD * solicitud,int * longitud_respuesta){
		char * nombre_usuario;
		nombre_usuario = buscar_usuario_por_sesion(solicitud->ID_Usuario - '0');

		if(eliminar_album(solicitud->nombre, nombre_usuario )){
				return mensaje_confirmacion(M_CONFIRMAR ,solicitud->ID_Usuario, SubOP_Eliminar_album , "Album borrado correctamente!", longitud_respuesta);
		}
		return mensaje_error(M_ERROR , '0' ,"Error: No se pudo eliminar el album", longitud_respuesta);
}// fin subOP_eliminar_album


void * mensaje_error(char codigo_OP , char codigo_Sub_OP ,char * mensaje , int * longitud_respuesta){
		ERROR * mensaje_error = (ERROR *)malloc(sizeof(ERROR));

		mensaje_error->OP = codigo_OP;
		mensaje_error->ID_SUB_OP_Fallo = codigo_Sub_OP;
		strcpy(mensaje_error->mensaje, mensaje);

		*longitud_respuesta = sizeof(ERROR);
		return (void *) mensaje_error;
}

void * mensaje_confirmacion(char codigo_OP ,char id_usuario, char codigo_Sub_OP ,char * mensaje , int * longitud_respuesta){
				CONFIRMAR * mensaje_confirmacion = (CONFIRMAR *)malloc(sizeof(CONFIRMAR));

				mensaje_confirmacion->OP = codigo_OP;
				mensaje_confirmacion->ID_Usuario = id_usuario;
				mensaje_confirmacion->ID_SUB_OP = codigo_Sub_OP;
				strcpy(mensaje_confirmacion->mensaje, mensaje);

				*longitud_respuesta = sizeof(CONFIRMAR);
				return (void *) mensaje_confirmacion;
}
