#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mensajes_utils.h"
#include "definitions.h"

void * mensaje_solicitud(char id_usuario_ , char codigo_Sub_OP , char id_album , char id_archivo ,char * nombre_solicitud, int * longitud ){
		SOLICITUD * mensaje_solicitud;
		mensaje_solicitud = (SOLICITUD *)malloc(sizeof(SOLICITUD));

		mensaje_solicitud->OP = M_SOLICITUD;
		mensaje_solicitud->ID_Usuario = id_usuario_;
		mensaje_solicitud->ID_SUB_OP = codigo_Sub_OP;
		mensaje_solicitud->ID_Album = id_album;
		mensaje_solicitud->ID_Archivo = id_archivo;
		strcpy(mensaje_solicitud->nombre, nombre_solicitud);

		*longitud = sizeof(SOLICITUD);
		return (void *)mensaje_solicitud;
}

void * mensaje_iniciar_sesion(char * usuario , char * clave , int * longitud){
		INICIAR_SESION * mensaje_iniciar_sesion;
		mensaje_iniciar_sesion = (INICIAR_SESION *)malloc(sizeof(INICIAR_SESION));

		mensaje_iniciar_sesion->OP = M_INICIAR_SESION;
		strcpy(mensaje_iniciar_sesion->usuario, usuario);
		strcpy(mensaje_iniciar_sesion->clave, clave);

		*longitud = sizeof(INICIAR_SESION);

		return (void *)mensaje_iniciar_sesion;
}

void * mensaje_registro(char * nombre,char * apellido ,char * usuario,char * clave, int * longitud){
		REGISTRO * mensaje_registro;
		mensaje_registro = (REGISTRO *)malloc(sizeof(REGISTRO));

		mensaje_registro->OP = M_REGISTRO;
		strcpy(mensaje_registro->nombre_completo, nombre);
		strcpy(mensaje_registro->apellido, apellido);
		strcpy(mensaje_registro->usuario, usuario);
		strcpy(mensaje_registro->clave, clave);

		*longitud = sizeof(REGISTRO);
		return (void *)mensaje_registro;
}


void * mensaje_cerrar_sesion(char id , int * longitud ){
	CERRAR_SESION * mensaje_cerrar_sesion;
	mensaje_cerrar_sesion = (CERRAR_SESION *)malloc(sizeof(CERRAR_SESION));

	mensaje_cerrar_sesion->OP = M_CERRAR_SESION;
	mensaje_cerrar_sesion->ID_Usuario = id;

	*longitud = sizeof(CERRAR_SESION);
	return (void *)mensaje_cerrar_sesion;
}

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
