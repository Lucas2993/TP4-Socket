#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../utils/definitions.h"
#include "album.h"

BOOLEAN crear_album(char * nombre, char * usuario){
	char folder [] = "albumes/";
	char * route = (char *)malloc(strlen(folder)+ strlen("/") + strlen(nombre) + strlen(usuario));

	strcpy(route, folder);
	strcat(route, usuario);
	strcat(route, "/");
	strcat(route, nombre);

	if(mkdir(route, 0777) == 0){
		return TRUE;
	}
	return FALSE;
}

BOOLEAN registrar_album(char * nombre, char * usuario){
	char folder [] = "albumes/";
	char * route = (char *)malloc(strlen(folder)+ strlen(usuario) + strlen("/albumes.txt"));
	int id;
	FILE * archivo;

	strcpy(route, folder);
	strcat(route, usuario);
	strcat(route, "/albumes.txt");

	id = siguiente_id(route);

	archivo = fopen(route, "a");

	if(archivo != NULL){
		fprintf(archivo, "%s %d\n", nombre, id);
		fclose(archivo);

		return TRUE;
	}
	return FALSE;
}

int siguiente_id(char * path){
	FILE * archivo;
	char album[MAX_NOMBRE_SOLICITUD];
	int id = -1;
	int mayor = 0;

	archivo = fopen(path, "r");

	if(archivo != NULL){
		while(fscanf(archivo, "%s", album) > 0){
			fscanf(archivo, "%d", &id);
			if(id > mayor){
				mayor = id;
			}
		}
		fclose(archivo);
	}

	return mayor+1;
}

BOOLEAN crear_carpeta_general_albumes(void){
	char folder [] = "albumes";

	if(mkdir(folder, 0777) == 0){
		return TRUE;
	}
	return FALSE;
}

BOOLEAN eliminar_album(char * nombre, char * usuario){
	char folder [] = "albumes/";
	char * route = (char *)malloc(strlen(folder)+ strlen("/") + strlen(nombre) + strlen(usuario));

	strcpy(route, folder);
	strcat(route, usuario);
	strcat(route, "/");
	strcat(route, nombre);

	if(remove(route) == 0){
		return TRUE;
	}
	return FALSE;
}
