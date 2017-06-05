/*
 * client.h
 *
 *  Created on: 19 may. 2017
 *      Author: lucas
 */

#ifndef CLIENT_CLIENT_H_
#define CLIENT_CLIENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client.h"
#include "../utils/definitions.h"


#define ITEMS_MENU_PRINCIPAL 3
#define ITEMS_MENU_SESION 12

int principal( FILE *fp, int sockfd, const struct sockaddr *dir, socklen_t sa );
void analizar_respuesta(char *);

void * iniciar_sesion(int *);
void * cerrar_sesion(int *);
void * registro(int *);

void * crear_album(int *);
void * eliminar_album(int *);

void obtener_datos( char * ouput , char * tipo , void * dato );

void obtener_datos_iniciar_sesion( char * usuario , char * clave );
void * mensaje_iniciar_sesion(char * usuario , char * clave , int * longitud);

void obtener_datos_registro(char * nombre , char * apellido , char * usuario , char * clave );
void * mensaje_registro(char * nombre,char * apellido ,char * usuario,char * clave, int * longitud);


void * mensaje_cerrar_sesion(char id , int * longitud );

void * mensaje_solicitud(char id_usuario_ , char codigo_Sub_OP , char id_album , char id_archivo ,char * nombre_solicitud, int * longitud );

void * salir(int *);


#endif /* CLIENT_CLIENT_H_ */
