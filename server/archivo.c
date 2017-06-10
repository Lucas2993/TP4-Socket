#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../utils/definitions.h"
#include "archivo.h"

BOOLEAN registrar_archivo(char * nombre, char * usuario, char * album){
	char folder [] = CARPETA_ALBUMES;
	char * route = (char *)malloc(strlen(folder)+ strlen(usuario) + strlen("/") + strlen(album) + strlen(ARCHIVO_ARCHIVOS));
	int id;
	FILE * archivo;

	strcpy(route, folder);
	strcat(route, usuario);
	strcat(route, "/");
	strcat(route, album);
	strcat(route, ARCHIVO_ARCHIVOS);

	id = siguiente_id_archivo(route);

	archivo = fopen(route, "a");

	if(archivo != NULL){
		fprintf(archivo, "%s %d\n", nombre, id);
		fclose(archivo);

		return TRUE;
	}
	return FALSE;
}

int siguiente_id_archivo(char * path){
	FILE * fp;
	char archivo[MAX_NOMBRE_SOLICITUD];
	int id = -1;
	int mayor = 0;

	fp = fopen(path, "r");

	if(fp != NULL){
		while(fscanf(fp, "%s", archivo) > 0){
			fscanf(fp, "%d", &id);
			if(id > mayor){
				mayor = id;
			}
		}
		fclose(fp);
	}

	return mayor+1;
}

char * buscar_archivo_id(char * usuario, char * album, int id_archivo){
	FILE * fp;
	char archivo[MAX_NOMBRE_SOLICITUD];
	int id = -1;
	char folder [] = CARPETA_ALBUMES;
	char * route = (char *)malloc(strlen(folder)+ strlen(usuario) + strlen("/") + strlen(album) + strlen(ARCHIVO_ARCHIVOS));
	char * respuesta = NULL;

	strcpy(route, folder);
	strcat(route, usuario);
	strcat(route, "/");
	strcat(route, album);
	strcat(route, ARCHIVO_ARCHIVOS);

	fp = fopen(route, "r");

	if(fp != NULL){
		while(fscanf(fp, "%s", archivo) > 0){
			fscanf(fp, "%d", &id);
			if(id == id_archivo){
				respuesta = (char *)malloc(sizeof(char) * strlen(archivo));
				strcpy(respuesta,archivo);
				break;
			}
		}
		fclose(fp);
	}

	return respuesta;
}

BOOLEAN eliminar_archivo(char * nombre, char * usuario, char * album){
	char folder [] = CARPETA_ALBUMES;
	char * route = (char *)malloc(strlen(folder)+ strlen(usuario) + strlen("/") + strlen(album) + strlen("/") + strlen(nombre));

	strcpy(route, folder);
	strcat(route, usuario);
	strcat(route, "/");
	strcat(route, album);
	strcat(route, "/");
	strcat(route, nombre);

	if(remove(route) == 0){
		return TRUE;
	}
	return FALSE;
}

BOOLEAN listar_archivos(FILE * fp, char * usuario, char * album){
	FILE * archivo_archivos;
	char archivo[MAX_NOMBRE_SOLICITUD];
	int id = -1;
	char folder [] = CARPETA_ALBUMES;
	int cantidad_barras = 1;
	char * route = (char *)malloc(strlen(folder)+ strlen(usuario) + (sizeof(char) * cantidad_barras) + strlen(album) + strlen(ARCHIVO_ARCHIVOS));
	BOOLEAN respuesta = TRUE;

	strcpy(route, folder);
	strcat(route, usuario);
	strcat(route, "/");
	strcat(route, album);
	strcat(route, ARCHIVO_ARCHIVOS);

	archivo_archivos = fopen(route, "r");

	if(archivo_archivos != NULL){
		while(fscanf(archivo_archivos, "%s", archivo) > 0){
			fscanf(archivo_archivos, "%d", &id);
			fprintf(fp, "%d - %s\n", id, archivo);
		}
		fclose(archivo_archivos);
	}

	fclose(fp);

	return respuesta;
}