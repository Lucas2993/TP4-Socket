#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../utils/definitions.h"
#include "album.h"

BOOLEAN crear_album(char * nombre, char * usuario){
	char folder [] = CARPETA_ALBUMES;
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
	char folder [] = CARPETA_ALBUMES;
	char * route = (char *)malloc(strlen(folder)+ strlen(usuario) + strlen(ARCHIVO_ALBUMES));
	int id;
	FILE * archivo;

	strcpy(route, folder);
	strcat(route, usuario);
	strcat(route, ARCHIVO_ALBUMES);

	id = siguiente_id_album(route);

	archivo = fopen(route, "a");

	if(archivo != NULL){
		fprintf(archivo, "%s %d\n", nombre, id);
		fclose(archivo);

		return TRUE;
	}
	return FALSE;
}

int siguiente_id_album(char * path){
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
	char folder [] = CARPETA_ALBUMES;

	if(mkdir(folder, 0777) == 0){
		return TRUE;
	}
	return FALSE;
}

char * buscar_album_id(char * usuario, int id_album){
	FILE * archivo;
	char album[MAX_NOMBRE_SOLICITUD];
	int id = -1;
	char folder [] = CARPETA_ALBUMES;
	char * route = (char *)malloc(strlen(folder)+ strlen(usuario) + strlen(ARCHIVO_ALBUMES));
	char * respuesta = NULL;

	strcpy(route, folder);
	strcat(route, usuario);
	strcat(route, ARCHIVO_ALBUMES);

	archivo = fopen(route, "r");

	if(archivo != NULL){
		while(fscanf(archivo, "%s", album) > 0){
			fscanf(archivo, "%d", &id);
			if(id == id_album){
				respuesta = (char *)malloc(sizeof(char) * strlen(album));
				strcpy(respuesta,album);
				break;
			}
		}
		fclose(archivo);
	}

	return respuesta;
}

char * crear_ruta(char * usuario, char * album, char * archivo){
	char folder [] = CARPETA_ALBUMES;
	int cantidad_barras = 2;
	char * ruta = (char *)malloc(strlen(folder) + strlen(usuario) + strlen(album) + strlen(archivo) + cantidad_barras);

	strcpy(ruta, folder);
	strcat(ruta, usuario);
	strcat(ruta, "/");
	strcat(ruta, album);
	strcat(ruta, "/");
	strcat(ruta, archivo);

	return ruta;
}

BOOLEAN eliminar_album(char * nombre, char * usuario,int id){
	char folder [] = "rm -R albumes/";
	char * route = (char *)malloc(strlen(folder)+ strlen("/") + strlen(nombre) + strlen(usuario));

	strcpy(route, folder);
	strcat(route, usuario);
	strcat(route, "/");
	strcat(route, nombre);

	if(system(route) == 0){
		eliminar_album_de_lista(usuario,id);
		return TRUE;
	}
	return FALSE;
}

BOOLEAN eliminar_album_de_lista( char * usuario, int id_album ){
	FILE * archivo;
	FILE * aux;
	BOOLEAN resultado = FALSE;
	char album[MAX_NOMBRE_SOLICITUD];
	int id = -1;

	char folder [] = "albumes/";
	char * route = (char *)malloc(strlen(folder)+ strlen("/") + strlen(usuario)+ strlen(ARCHIVO_ALBUM ));

	strcpy(route, folder);
	strcat(route, usuario);
	strcat(route, "/");
	strcat(route, ARCHIVO_ALBUM);
	printf("route : %s\n",route );
	archivo = fopen(route, "r");
	aux = fopen(ARCHIVO_AUXILIAR_ALBUM, "w");
	if(archivo != NULL && aux != NULL){
		while(fscanf(archivo, "%s", album) > 0){
			fscanf(archivo, "%d", &id);
			printf("------------------- %s : %d\n",album,id );
			if(id != id_album){
				printf("%s : %d\n",album,id );
				fprintf(aux, "%s %d\n", album, id);
			}
		}
		fclose(archivo);
		fclose(aux);

		remove(route);
		rename(ARCHIVO_AUXILIAR_ALBUM, route);
		remove(ARCHIVO_AUXILIAR_ALBUM);
		resultado = TRUE;
	}
	return resultado;
}

BOOLEAN listar_albumes(FILE * fp, char * usuario){
	FILE * archivo;
	char album[MAX_NOMBRE_SOLICITUD];
	int id = -1;
	char folder [] = CARPETA_ALBUMES;
	char * route = (char *)malloc(strlen(folder)+ strlen(usuario) + strlen(ARCHIVO_ALBUMES));
	BOOLEAN respuesta = TRUE;

	strcpy(route, folder);
	strcat(route, usuario);
	strcat(route, ARCHIVO_ALBUMES);

	archivo = fopen(route, "r");

	if(archivo != NULL){
		while(fscanf(archivo, "%s", album) > 0){
			fscanf(archivo, "%d", &id);
			fprintf(fp, "%d- %s\n", id, album);
		}
		fclose(archivo);
	}

	fclose(fp);

	return respuesta;
}

BOOLEAN renombrar_album(char * usuario, char * nombre_actual, char * nuevo_nombre){
	char folder [] = CARPETA_ALBUMES;
	char * old_route = (char *)malloc(strlen(folder)+ strlen("/") + strlen(nombre_actual) + strlen(usuario));
	char * new_route = (char *)malloc(strlen(folder)+ strlen("/") + strlen(nuevo_nombre) + strlen(usuario));

	strcpy(old_route, folder);
	strcat(old_route, usuario);
	strcat(old_route, "/");
	strcat(old_route, nombre_actual);

	strcpy(new_route, folder);
	strcat(new_route, usuario);
	strcat(new_route, "/");
	strcat(new_route, nuevo_nombre);

	if(rename(old_route, new_route) == 0){
		return TRUE;
	}
	return FALSE;
}

BOOLEAN renombrar_album_registro(char * usuario, int id_album, char * nuevo_nombre){
	FILE * archivo;
	FILE * archivo_auxiliar;
	char album[MAX_NOMBRE_SOLICITUD];
	int id = -1;
	char folder [] = CARPETA_ALBUMES;
	char * route = (char *)malloc(strlen(folder)+ strlen(usuario) + strlen(ARCHIVO_ALBUMES));
	char * route_aux = (char *)malloc(strlen(folder)+ strlen(usuario) + strlen(ARCHIVO_AUXILIAR_ALBUM));

	strcpy(route, folder);
	strcat(route, usuario);
	strcat(route, ARCHIVO_ALBUMES);

	strcpy(route_aux, folder);
	strcat(route_aux, usuario);
	strcat(route_aux, ARCHIVO_AUXILIAR_ALBUM);

	archivo = fopen(route, "r");
	archivo_auxiliar = fopen(route_aux, "w");

	if(archivo != NULL && archivo_auxiliar != NULL){
		while(fscanf(archivo, "%s", album) > 0){
			fscanf(archivo, "%d", &id);
			if(id == id_album){
				fprintf(archivo_auxiliar, "%s %d\n", nuevo_nombre, id);
			}
			else{
				fprintf(archivo_auxiliar, "%s %d\n", album, id);
			}
		}
		fclose(archivo);
		fclose(archivo_auxiliar);
	}

	remove(route);
	rename(route_aux,route);

	return TRUE;
}