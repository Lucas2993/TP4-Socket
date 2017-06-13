#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../utils/definitions.h"
#include "compartir.h"

BOOLEAN compartir_album_usuario(char * usuario, char * usuario_destino, char * album, int album_id){
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
		fprintf(compatidos_conmigo, "%s %s %d\n", usuario, album, album_id);
		fprintf(compatidos_otros, "%s %s %d\n", usuario_destino, album, album_id);
		fclose(compatidos_conmigo);
		fclose(compatidos_otros);
	}

	return TRUE;
}

BOOLEAN dejar_compartir_album_usuario(char * usuario, char * usuario_destino, char * album, int album_id){
	FILE * compatidos_conmigo;
	FILE * compatidos_otros;
	FILE * compatidos_conmigo_aux;
	FILE * compatidos_otros_aux;
	char folder [] = CARPETA_ALBUMES;
	char usuario_archivo[MAX_USUARIO];
	char album_archivo[MAX_NOMBRE_SOLICITUD];
	int album_id_archivo;

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
			fscanf(compatidos_conmigo, "%s", album_archivo);
			fscanf(compatidos_conmigo, "%d", &album_id_archivo);
			if(album_id != album_id_archivo && strcmp(usuario_archivo, usuario) != 0){
				fprintf(compatidos_conmigo_aux, "%s %s %d\n", usuario_archivo, album_archivo, album_id);
			}
		}
		fclose(compatidos_conmigo);
		fclose(compatidos_conmigo_aux);

		while(fscanf(compatidos_otros, "%s", usuario_archivo) > 0){
			fscanf(compatidos_otros, "%s", album_archivo);
			fscanf(compatidos_otros, "%d", &album_id_archivo);
			if(album_id != album_id_archivo && strcmp(usuario_archivo, usuario) != 0){
				fprintf(compatidos_otros_aux, "%s %s %d\n", usuario_archivo, album_archivo, album_id);
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

BOOLEAN listar_albumes_compartidos_conmigo(FILE * fp, char * usuario){
	FILE * archivo;
	char folder [] = CARPETA_ALBUMES;
	char * route = (char *)malloc(strlen(folder)+ strlen(usuario) + strlen(ARCHIVO_COMPARTIDOS_CONMIGO));
	char usuario_archivo[MAX_NOMBRE_SOLICITUD];
	char album_archivo[MAX_NOMBRE_SOLICITUD];
	int id_album;
	BOOLEAN respuesta = TRUE;

	strcpy(route, folder);
	strcat(route, usuario);
	strcat(route, ARCHIVO_COMPARTIDOS_CONMIGO);

	archivo = fopen(route, "r");

	if(archivo != NULL){
		while(fscanf(archivo, "%s", usuario_archivo) > 0){
			fscanf(archivo, "%s", album_archivo);
			fscanf(archivo, "%d", &id_album);
			fprintf(fp, "Propietario: %s, Nombre del album: %s, Identificador: %d\n", usuario_archivo, album_archivo, id_album);
		}
		fclose(archivo);
	}

	fclose(fp);

	return respuesta;
}