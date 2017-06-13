#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../utils/definitions.h"
#include "compartir.h"

BOOLEAN compartir_album_usuario(char * usuario, char * usuario_destino, int album){
	FILE * compatidos_conmigo;
	FILE * compatidos_otros;
	char folder [] = CARPETA_ALBUMES;
	char * route_compartidos_otros = (char *)malloc(strlen(folder) + strlen(usuario) + strlen(ARCHIVO_COMPARTIDOS_OTROS));
	char * route_compartidos_conmigo = (char *)malloc(strlen(folder) + strlen(usuario_destino) + strlen(ARCHIVO_COMPARTIDOS_CONMIGO));

	strcpy(route_compartidos_otros, folder);
	strcat(route_compartidos_otros, usuario);
	strcat(route_compartidos_otros, ARCHIVO_COMPARTIDOS_OTROS);

	strcpy(route_compartidos_conmigo, folder);
	strcat(route_compartidos_conmigo, usuario_destino);
	strcat(route_compartidos_conmigo, ARCHIVO_COMPARTIDOS_CONMIGO);

	compatidos_conmigo = fopen(route_compartidos_conmigo, "a");
	compatidos_otros = fopen(route_compartidos_otros, "a");

	if(compatidos_conmigo != NULL && compatidos_otros != NULL){
		fprintf(compatidos_conmigo, "%s %d\n", usuario, album);
		fprintf(compatidos_otros, "%s %d\n", usuario_destino, album);
		fclose(compatidos_conmigo);
		fclose(compatidos_otros);
	}

	return TRUE;
}

BOOLEAN dejar_compartir_album_usuario(char * usuario, char * usuario_destino, int album){
	FILE * compatidos_conmigo;
	FILE * compatidos_otros;
	FILE * compatidos_conmigo_aux;
	FILE * compatidos_otros_aux;
	char folder [] = CARPETA_ALBUMES;
	char usuario_archivo[MAX_USUARIO];
	int album_id;

	char * route_compartidos_otros = (char *)malloc(strlen(folder) + strlen(usuario) + strlen(ARCHIVO_COMPARTIDOS_OTROS));
	char * route_compartidos_conmigo = (char *)malloc(strlen(folder) + strlen(usuario_destino) + strlen(ARCHIVO_COMPARTIDOS_CONMIGO));
	char * route_compartidos_otros_aux = (char *)malloc(strlen(folder) + strlen(usuario) + strlen(ARCHIVO_COMPARTIDOS_OTROS_AUX));
	char * route_compartidos_conmigo_aux = (char *)malloc(strlen(folder) + strlen(usuario_destino) + strlen(ARCHIVO_COMPARTIDOS_CONMIGO_AUX));

	strcpy(route_compartidos_otros, folder);
	strcat(route_compartidos_otros, usuario);
	strcat(route_compartidos_otros, ARCHIVO_COMPARTIDOS_OTROS);

	strcpy(route_compartidos_conmigo, folder);
	strcat(route_compartidos_conmigo, usuario_destino);
	strcat(route_compartidos_conmigo, ARCHIVO_COMPARTIDOS_CONMIGO);

	strcpy(route_compartidos_otros_aux, folder);
	strcat(route_compartidos_otros_aux, usuario);
	strcat(route_compartidos_otros_aux, ARCHIVO_COMPARTIDOS_OTROS_AUX);

	strcpy(route_compartidos_conmigo_aux, folder);
	strcat(route_compartidos_conmigo_aux, usuario_destino);
	strcat(route_compartidos_conmigo_aux, ARCHIVO_COMPARTIDOS_CONMIGO_AUX);

	compatidos_conmigo = fopen(route_compartidos_conmigo, "r");
	compatidos_otros = fopen(route_compartidos_otros, "r");
	compatidos_conmigo_aux = fopen(route_compartidos_conmigo_aux, "a");
	compatidos_otros_aux = fopen(route_compartidos_otros_aux, "a");

	if(compatidos_conmigo != NULL && compatidos_otros != NULL && compatidos_conmigo_aux != NULL && compatidos_otros_aux != NULL){
		while(fscanf(compatidos_conmigo, "%s", usuario_archivo) > 0){
			fscanf(compatidos_conmigo, "%d", album_id);
			if(album != album_id && strcmp(usuario_archivo, usuario) != 0){
				fprintf(compatidos_conmigo_aux, "%s %s\n", usuario_archivo, album_id);
			}
		}
		fclose(compatidos_conmigo);
		fclose(compatidos_conmigo_aux);

		while(fscanf(compatidos_otros, "%s", usuario_archivo) > 0){
			fscanf(compatidos_otros, "%s", album_id);
			if(album != album_id && strcmp(usuario_archivo, usuario) != 0){
				fprintf(compatidos_otros_aux, "%s %s\n", usuario_archivo, album_id);
			}
		}
		fclose(compatidos_otros);
		fclose(compatidos_otros_aux);
	}

	remove(route_compartidos_conmigo);
	remove(route_compartidos_otros);
	rename(route_compartidos_conmigo_aux, route_compartidos_conmigo);
	rename(route_compartidos_otros_aux, route_compartidos_otros);

	return TRUE;
}