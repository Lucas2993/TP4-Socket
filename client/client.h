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

#include "../utils/definitions.h"


#define ITEMS_MENU_PRINCIPAL 3
#define ITEMS_MENU_SESION 13

int principal( FILE *fp, int sockfd, const struct sockaddr *dir, socklen_t sa );
void analizar_respuesta(char *);

void * iniciar_sesion(int *);
void * cerrar_sesion(int *);
void * registro(int *);

void * crear_album(int *);
void * subir_archivo_album(int *);
void * descargar_archivo_album(int *);
void * eliminar_album(int *);
void * listar_albumes(int *);

void obtener_datos( char * ouput , char * tipo , void * dato );

void obtener_datos_iniciar_sesion( char * usuario , char * clave );

void obtener_datos_registro(char * nombre , char * apellido , char * usuario , char * clave );

void * salir(int *);


#endif /* CLIENT_CLIENT_H_ */
