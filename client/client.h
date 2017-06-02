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


void * salir(int *);


#endif /* CLIENT_CLIENT_H_ */
