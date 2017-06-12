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

BOOLEAN eliminar_archivo_de_lista(char * usuario, char * album, int id_archivo){
	FILE * archivo_archivos;
	FILE * aux;
	BOOLEAN resultado = FALSE;
	char archivo[MAX_NOMBRE_SOLICITUD];
	int id = -1;

	char folder [] = CARPETA_ALBUMES;
	char * route = (char *)malloc(strlen(folder) + strlen("/") + strlen(usuario)+ strlen(album)+ strlen(ARCHIVO_ARCHIVOS));
	char * route_aux = (char *)malloc(strlen(folder) + strlen("/") + strlen(usuario) + strlen(album) + strlen(ARCHIVO_AUXILIAR_ARCHIVOS));

	strcpy(route, folder);
	strcat(route, usuario);
	strcat(route, "/");
	strcat(route, album);
	strcat(route, ARCHIVO_ARCHIVOS);

	strcpy(route_aux, folder);
	strcat(route_aux, usuario);
	strcat(route_aux, "/");
	strcat(route_aux, album);
	strcat(route_aux, "/");
	strcat(route_aux, ARCHIVO_AUXILIAR_ARCHIVOS);


	archivo_archivos = fopen(route, "r");
	aux = fopen(route_aux, "w");
	if(archivo_archivos != NULL && aux != NULL){
		while(fscanf(archivo_archivos, "%s", archivo) > 0){
			fscanf(archivo_archivos, "%d", &id);
			if(id != id_archivo){
				fprintf(aux, "%s %d\n", archivo, id);
			}
		}
		fclose(archivo_archivos);
		fclose(aux);

		remove(route);
		rename(route_aux, route);

		resultado = TRUE;
	}
	return resultado;
}

BOOLEAN renombrar_archivo(char * usuario, char * album, char * nombre_actual, char * nuevo_nombre){
	char folder [] = CARPETA_ALBUMES;
	char * old_route = (char *)malloc(strlen(folder)+ strlen("/") + strlen(nombre_actual) + strlen(usuario) + strlen(album));
	char * new_route = (char *)malloc(strlen(folder)+ strlen("/") + strlen(nuevo_nombre) + strlen(usuario) + strlen(album));

	strcpy(old_route, folder);
	strcat(old_route, usuario);
	strcat(old_route, "/");
	strcat(old_route, album);
	strcat(old_route, "/");
	strcat(old_route, nombre_actual);

	strcpy(new_route, folder);
	strcat(new_route, usuario);
	strcat(new_route, "/");
	strcat(new_route, album);
	strcat(new_route, "/");
	strcat(new_route, nuevo_nombre);

	if(rename(old_route, new_route) == 0){
		return TRUE;
	}
	return FALSE;
}

BOOLEAN renombrar_archivo_registro(char * usuario, char * album, int id_archivo, char * nuevo_nombre){
	FILE * archivo_archivos;
	FILE * archivo_auxiliar;
	char archivo[MAX_NOMBRE_SOLICITUD];
	int id = -1;
	char folder [] = CARPETA_ALBUMES;
	char * route = (char *)malloc(strlen(folder)+ strlen("//") + strlen(album) + strlen(usuario) + strlen(ARCHIVO_ARCHIVOS));
	char * route_aux = (char *)malloc(strlen(folder)+ strlen("//") + strlen(album) + strlen(usuario) + strlen(ARCHIVO_AUXILIAR_ARCHIVOS));

	strcpy(route, folder);
	strcat(route, usuario);
	strcat(route, "/");
	strcat(route, album);
	strcat(route, "/");
	strcat(route, ARCHIVO_ARCHIVOS);

	strcpy(route_aux, folder);
	strcat(route_aux, usuario);
	strcat(route_aux, "/");
	strcat(route_aux, album);
	strcat(route_aux, "/");
	strcat(route_aux, ARCHIVO_AUXILIAR_ARCHIVOS);

	archivo_archivos = fopen(route, "r");
	archivo_auxiliar = fopen(route_aux, "w");

	if(archivo_archivos != NULL && archivo_auxiliar != NULL){
		while(fscanf(archivo_archivos, "%s", archivo) > 0){
			fscanf(archivo_archivos, "%d", &id);
			if(id == id_archivo){
				fprintf(archivo_auxiliar, "%s %d\n", nuevo_nombre, id);
			}
			else{
				fprintf(archivo_auxiliar, "%s %d\n", archivo, id);
			}
		}
		fclose(archivo_archivos);
		fclose(archivo_auxiliar);
	}

	remove(route);
	rename(route_aux,route);

	return TRUE;
}