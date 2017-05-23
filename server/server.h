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
#include "../utils/eco.h"
#include "../utils/definitions.h"

void procesar(int, struct sockaddr *, socklen_t);

void * resolver(int, char *);

void * iniciar_sesion(char *);
void * registrar(char *);
void * solicitud(char *);
void * cerrar_sesion(char *);

#endif /* SERVER_SERVER_H_ */
