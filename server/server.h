/*
 * server.h
 *
 *  Created on: 19 may. 2017
 *      Author: lucas
 */

#ifndef SERVER_SERVER_H_
#define SERVER_SERVER_H_

#include <string.h>
#include <stdlib.h>
#include "../utils/definitions.h"

void procesar(int, struct sockaddr *, socklen_t);

void * resolver(int, char *, int *);
void * iniciar_sesion(char *, int *);
void * registrar(char *, int *);
void * solicitud(char *, int *);
void * cerrar_sesion(char *, int *);

void * subOP_crear_album( SOLICITUD * ,int * );

void * subOP_eliminar_album( SOLICITUD *,int * );

void * mensaje_error(char codigo_OP , char codigo_Sub_OP ,char * mensaje , int * longitud_respuesta);
void * mensaje_confirmacion(char codigo_OP ,char id_usuario, char codigo_Sub_OP ,char * mensaje , int * longitud_respuesta);

#endif /* SERVER_SERVER_H_ */
