
#include "definitions.h"

void * mensaje_solicitud(char id_usuario_ , char codigo_Sub_OP , char id_album , char id_archivo ,char * nombre_solicitud, int * longitud );
void * mensaje_iniciar_sesion(char * usuario , char * clave , int * longitud);
void * mensaje_registro(char * nombre,char * apellido ,char * usuario,char * clave, int * longitud);
void * mensaje_cerrar_sesion(char id , int * longitud );
void * mensaje_solicitud(char id_usuario_ , char codigo_Sub_OP , char id_album , char id_archivo ,char * nombre_solicitud, int * longitud );
void * mensaje_error(char codigo_OP , char codigo_Sub_OP ,char * mensaje , int * longitud_respuesta);
void * mensaje_confirmacion(char codigo_OP ,char id_usuario, char codigo_Sub_OP ,char * mensaje , int * longitud_respuesta);
