#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../utils/definitions.h"
#include "sesion.h"

BOOLEAN agregar_sesion(char * nombre_usuario, int id){
	FILE * archivo;
	BOOLEAN resultado = FALSE;

	archivo = fopen(ARCHIVO_SESIONES, "a");
	if(archivo != NULL){
		fprintf(archivo, "%s %d\n", nombre_usuario, id);
		fclose(archivo);

		resultado = TRUE;
	}

	return resultado;
}

int buscar_sesion_por_usuario(char * nombre_usuario){
	FILE * archivo;
	BOOLEAN encontrado = FALSE;
	char usuario[MAX_USUARIO];
	int id = -1;
	int respuesta = -1;

	archivo = fopen(ARCHIVO_SESIONES, "r");

	if(archivo != NULL){
		while(fscanf(archivo, "%s", usuario) > 0 && !encontrado){
			fscanf(archivo, "%d", &id);
			if(strcmp(nombre_usuario, usuario) == 0){
				encontrado = TRUE;
				respuesta = id;
			}
		}
		fclose(archivo);
	}

	return respuesta;
}

int asignar_numero_sesion(void){
	FILE * archivo;
	char usuario[MAX_USUARIO];
	int id = -1;
	int mayor = 0;

	archivo = fopen(ARCHIVO_SESIONES, "r");

	if(archivo != NULL){
		while(fscanf(archivo, "%s", usuario) > 0){
			fscanf(archivo, "%d", &id);
			if(id > mayor){
				mayor = id;
			}
		}
		fclose(archivo);
	}

	return mayor+1;
}

BOOLEAN cerrar_sesion_usuario(int id_usuario){
	FILE * archivo;
	FILE * aux;
	BOOLEAN resultado = FALSE;
	char usuario[MAX_USUARIO];
	int id = -1;

	archivo = fopen(ARCHIVO_SESIONES, "r");
	aux = fopen(ARCHIVO_AUXILIAR, "w");
	if(archivo != NULL && aux != NULL){
		while(fscanf(archivo, "%s", usuario) > 0){
			fscanf(archivo, "%d", &id);
			if(id != id_usuario){
				fprintf(aux, "%s %d\n", usuario, id);
			}
		}
		fclose(aux);
		fclose(archivo);

		remove(ARCHIVO_SESIONES);
		rename(ARCHIVO_AUXILIAR, ARCHIVO_SESIONES);

		resultado = TRUE;
	}

	return resultado;
}

char * buscar_usuario_por_sesion(int id_usuario){
	FILE * archivo;
	BOOLEAN encontrado = FALSE;
	char usuario[MAX_USUARIO];
	int id = -1;
	char * respuesta = NULL;

	archivo = fopen(ARCHIVO_SESIONES, "r");

	if(archivo != NULL){
		while(fscanf(archivo, "%s", usuario) > 0 && !encontrado){
			fscanf(archivo, "%d", &id);
			// printf("%s y %d y %d\n", usuario, id, id_usuario);
			if(id == id_usuario){
				encontrado = TRUE;
				respuesta = (char *)malloc(sizeof(char) * strlen(usuario));
				strcpy(respuesta, usuario);
			}
		}
		fclose(archivo);
	}

	return respuesta;
}
