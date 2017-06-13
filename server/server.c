#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "sesion.h"
#include "usuario.h"
#include "../utils/mensajes_utils.h"
#include "../utils/definitions.h"
#include "album.h"
#include "archivo.h"
#include "compartir.h"
#include "server_ftp.h"
#include "server.h"

void main(int argc, char *argv[]) {
	int descriptor;
	struct sockaddr_in dir_srv, dir_cli;
	int puerto_transferencia;
	int pid;

	/*---------------------------------------------------------------------*
	 * Verificar los argumentos recibidos								   *
	 *---------------------------------------------------------------------*/
	if (argc < 3) {
		printf("Uso: servidor <puerto> <puerto_transferencia>\n");
		exit(-1);
	}

	puerto_transferencia = atoi(argv[2]);

	/*--------------------------------------------------------------------*
	 * Inicializar el servidor                               			  *
	 *--------------------------------------------------------------------*/
	if ((descriptor = socket( AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("Servidor UDP: Error creando el socket: %d\n", errno);
		exit(-1);
	}

	bzero((char *) &dir_srv, sizeof(dir_srv));
	dir_srv.sin_family = AF_INET;
	dir_srv.sin_addr.s_addr = htonl( INADDR_ANY);
	dir_srv.sin_port = htons(atoi(argv[1]));

	if (bind(descriptor, (struct sockaddr *) &dir_srv, sizeof(dir_srv)) < 0) {
		printf("Servidor UDP: Error al crear el enlace: %d\n", errno);
		exit(-1);
	}
	printf("Servidor UDP: Servidor iniciado.\n");

	pid = fork();

	if(pid == 0)
		iniciar_servidor_ftp(puerto_transferencia);
	else
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
		printf("Servidor UDP: Error: El usuario %s no existe!\n", iniciar_sesion->usuario);
		return mensaje_error(M_ERROR , '0' ,"Error: El usuario especificado no existe!", longitud_respuesta);
	}
	// printf("Usuario encontrado: %s y %s\n", usuario->usuario, usuario->clave);

	//Si la contraseña es incorrecta
	if(strcmp(iniciar_sesion->clave, usuario->clave) != 0){
		printf("Servidor UDP: Error: El usuario %s ha ingresado una contraseña no valida.\n", usuario->usuario);
		return mensaje_error(M_ERROR , '0' ,"Error: Contraseña incorrecta!", longitud_respuesta);		}
	// Si no hubo error se inicia la sesion.
	numero_sesion = buscar_sesion_por_usuario(usuario->usuario);
	if(numero_sesion < 0){
		numero_sesion = asignar_numero_sesion();
		agregar_sesion(usuario->usuario, numero_sesion);
	}
	printf("Servidor UDP: El usuario %s ha iniciado sesion.\n", usuario->usuario);
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
			printf("Servidor UDP: Usuario ya existente\n");
			return mensaje_error(M_ERROR , '0' ,"Error: El usuario especificado ya existe!", longitud_respuesta);
	}
	// Intenta agregar al usuario.
	usuario_esta_agregado = agregar_usuario(registro->usuario, registro->clave, registro->nombre_completo, registro->apellido);
	// Si el archivo no pudo ser accedido.
	if( !usuario_esta_agregado ){
		return mensaje_error(M_ERROR , '0' ,"Error: El archivo usuarios.txt no pudo ser abierto", longitud_respuesta);
	}
	printf("Servidor UDP: Se ha registrado un nuevo usuario llamado: %s %s\n", registro->nombre_completo, registro->apellido);
	return mensaje_confirmacion(M_CONFIRMAR ,'0', '0' , "Usuario registrado correctamente!" , longitud_respuesta);
}


/*--------------------------------------------------------------------*
 * Se ocupa de resolver las sub-operaciones del cliente               *
 *--------------------------------------------------------------------*/
void * solicitud(char * mensaje, int * longitud_respuesta){
	SOLICITUD * solicitud = (SOLICITUD *)mensaje;

	// TODO Verificar que no esta al pedo
	if(!validar_usuario( solicitud->ID_Usuario) ){
			return mensaje_error(M_ERROR , '0' ,"Error: Usuario invalido", longitud_respuesta);
	}

	switch(solicitud->ID_SUB_OP){

	case SubOP_Listar_albumes:
		break;
	case SubOP_Crear_album:
		return subOP_crear_album(solicitud,longitud_respuesta);
		break;
	case SubOP_Modificar_album:
		return modificar_album(solicitud, longitud_respuesta);
		break;
	case SubOP_Eliminar_album:
		return subOP_eliminar_album( solicitud,longitud_respuesta );
		break;
	case SubOP_Listar_archivos_album:
		break;
	case SubOP_Subir_archivo_album:
		break;
	case SubOP_Modificar_archivo_album:
		return modificar_archivo(solicitud, longitud_respuesta);
		break;
	case SubOP_Eliminar_archivo_album:
		return subOP_eliminar_archivo(solicitud, longitud_respuesta);
		break;
	case SubOP_Compartir_album_usuario:
		return subOP_compartir_album_usuario(solicitud, longitud_respuesta);
		break;
	case SubOP_Dejar_compartir_album_usuario:
		break;
	case SubOP_Listar_usuarios:
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
	char * nombre_usuario;

	nombre_usuario = buscar_usuario_por_sesion(cerrar_sesion->ID_Usuario - '0');
	usuario_cerro_sesion = cerrar_sesion_usuario(cerrar_sesion->ID_Usuario - '0');

	// Si el usuario no pudo cerrar sesion
	if(!usuario_cerro_sesion ){
		return mensaje_error(M_ERROR , '0' ,"Error: No se pudo cerrar la sesion", longitud_respuesta);
	}
	printf("Servidor UDP: El usuario %s ha cerrado sesion.\n", nombre_usuario);
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
	printf("album_id : %d\n",  solicitud->ID_Album - '0' );
	char * nombre_album = buscar_album_id(nombre_usuario, solicitud->ID_Album - '0');

	if(eliminar_album(nombre_album, nombre_usuario, solicitud->ID_Album - '0') ){
		printf("album %s eliminado \n",nombre_album );
		// eliminar_album_de_lista( solicitud->ID_Album - '0');
		printf("elimino album con id %d\n", solicitud->ID_Album - '0' );

		return mensaje_confirmacion(M_CONFIRMAR ,solicitud->ID_Usuario, SubOP_Eliminar_album , "Album borrado correctamente!", longitud_respuesta);
	}
	return mensaje_error(M_ERROR , '0' ,"Error: No se pudo borrar el album", longitud_respuesta);
}// fin subOP_eliminar_album


void * modificar_album(SOLICITUD * solicitud, int * longitud_respuesta){
	CONFIRMAR * mensaje_confirmacion;
	ERROR * mensaje_error;

	char * nombre_actual;
	char * usuario;

	usuario = buscar_usuario_por_sesion(solicitud->ID_Usuario);

	nombre_actual = buscar_album_id(usuario, solicitud->ID_Album);

	mensaje_error = (ERROR *)malloc(sizeof(ERROR));
	if(renombrar_album(usuario, nombre_actual, solicitud->nombre)){
		if(renombrar_album_registro(usuario, solicitud->ID_Album, solicitud->nombre)){
			mensaje_confirmacion = (CONFIRMAR *)malloc(sizeof(CONFIRMAR));

			mensaje_confirmacion->OP = M_CONFIRMAR;
			mensaje_confirmacion->ID_Usuario = solicitud->ID_Usuario;
			mensaje_confirmacion->ID_SUB_OP = solicitud->ID_SUB_OP;
			strcpy(mensaje_confirmacion->mensaje, "El album ha sido renombrado correctamente!");

			*longitud_respuesta = sizeof(CONFIRMAR);

			return (void *)mensaje_confirmacion;
		}
		else{
			strcpy(mensaje_error->mensaje, "Error al renombrar el album en el registro.");	
		}
	}
	else{
		strcpy(mensaje_error->mensaje, "Error al renombrar el album solicitado.");
	}
	
	mensaje_error->OP = M_ERROR;
	mensaje_error->ID_SUB_OP_Fallo = solicitud->ID_SUB_OP;
	*longitud_respuesta = sizeof(ERROR);

	return (void *)mensaje_error;
}

void * subOP_eliminar_archivo( SOLICITUD * solicitud,int * longitud_respuesta){
	char * usuario;
	char * album;
	char * archivo;
	
	usuario = buscar_usuario_por_sesion(solicitud->ID_Usuario);
	album = buscar_album_id(usuario, solicitud->ID_Album);
	archivo = buscar_archivo_id(usuario, album, solicitud->ID_Archivo);

	if(eliminar_archivo(archivo, usuario, album)){
		if(eliminar_archivo_de_lista(usuario, album, solicitud->ID_Archivo)){
			return mensaje_confirmacion(M_CONFIRMAR ,solicitud->ID_Usuario, SubOP_Eliminar_album , "Archivo borrado correctamente!", longitud_respuesta);
		}
	}
	return mensaje_error(M_ERROR , '0' ,"Error: No se pudo borrar el archivo", longitud_respuesta);
}

void * modificar_archivo(SOLICITUD * solicitud, int * longitud_respuesta){
	CONFIRMAR * mensaje_confirmacion;
	ERROR * mensaje_error;

	char * nombre_actual;
	char * usuario;
	char * album;
	char * extension;
	char * nombre_nuevo;

	usuario = buscar_usuario_por_sesion(solicitud->ID_Usuario);

	album = buscar_album_id(usuario, solicitud->ID_Album);

	nombre_actual = buscar_archivo_id(usuario, album, solicitud->ID_Archivo);

	extension = strrchr(nombre_actual,'.');

	nombre_nuevo = (char *)malloc(sizeof(char) * strlen(extension) + strlen(solicitud->nombre));
	sprintf(nombre_nuevo, "%s%s", solicitud->nombre, extension);

	mensaje_error = (ERROR *)malloc(sizeof(ERROR));
	if(renombrar_archivo(usuario, album, nombre_actual, nombre_nuevo)){
		if(renombrar_archivo_registro(usuario, album, solicitud->ID_Archivo, nombre_nuevo)){
			mensaje_confirmacion = (CONFIRMAR *)malloc(sizeof(CONFIRMAR));

			mensaje_confirmacion->OP = M_CONFIRMAR;
			mensaje_confirmacion->ID_Usuario = solicitud->ID_Usuario;
			mensaje_confirmacion->ID_SUB_OP = solicitud->ID_SUB_OP;
			strcpy(mensaje_confirmacion->mensaje, "El archivo ha sido renombrado correctamente!");

			*longitud_respuesta = sizeof(CONFIRMAR);

			return (void *)mensaje_confirmacion;
		}
		else{
			strcpy(mensaje_error->mensaje, "Error al renombrar el archivo en el registro.");	
		}
	}
	else{
		strcpy(mensaje_error->mensaje, "Error al renombrar el archivo solicitado.");
	}
	
	mensaje_error->OP = M_ERROR;
	mensaje_error->ID_SUB_OP_Fallo = solicitud->ID_SUB_OP;
	*longitud_respuesta = sizeof(ERROR);

	return (void *)mensaje_error;
}

void * subOP_compartir_album_usuario(SOLICITUD * solicitud, int * longitud_respuesta){
	CONFIRMAR * mensaje_confirmacion;
	ERROR * mensaje_error;

	char * usuario_destino;
	char * usuario;
	char * album;

	usuario_destino = (char *)malloc(sizeof(char) * strlen(solicitud->nombre));
	strcpy(usuario_destino, solicitud->nombre);

	usuario = buscar_usuario_por_sesion(solicitud->ID_Usuario);

	album = buscar_album_id(usuario, solicitud->ID_Album);

	if(compartir_album_usuario(usuario, usuario_destino, solicitud->ID_Album)){
		mensaje_confirmacion = (CONFIRMAR *)malloc(sizeof(CONFIRMAR));

		mensaje_confirmacion->OP = M_CONFIRMAR;
		mensaje_confirmacion->ID_Usuario = solicitud->ID_Usuario;
		mensaje_confirmacion->ID_SUB_OP = solicitud->ID_SUB_OP;
		strcpy(mensaje_confirmacion->mensaje, "El album ha sido compartido correctamente!");

		*longitud_respuesta = sizeof(CONFIRMAR);

		return (void *)mensaje_confirmacion;
	}
	
	mensaje_error = (ERROR *)malloc(sizeof(ERROR));

	mensaje_error->OP = M_ERROR;
	mensaje_error->ID_SUB_OP_Fallo = solicitud->ID_SUB_OP;
	strcpy(mensaje_error->mensaje, "Error al compartir el album especificado.");
		
	*longitud_respuesta = sizeof(ERROR);

	return (void *)mensaje_error;
}