#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "../utils/definitions.h"
#include "sesion.h"
#include "usuario.h"

USUARIO * buscar_usuario(char * nombre_usuario){
	FILE * archivo;
	char usuario[MAX_USUARIO];
	char clave[MAX_USUARIO];
	char nombre[MAX_NOMBRE];
	char apellido[MAX_APELLIDO];
	USUARIO * usuario_encontrado = NULL;
	BOOLEAN encontrado = FALSE;

	archivo = fopen (ARCHIVO_USUARIOS, "r");

	if(archivo != NULL){
		while(fscanf(archivo, "%s", usuario) > 0 && !encontrado){
			fscanf(archivo, "%s", clave);
			fscanf(archivo, "%s", nombre);
			fscanf(archivo, "%s", apellido);
			// printf("Control usuario: %s\n", usuario);
			// printf("Control clave: %s\n", clave);
			// printf("Control nombre: %s\n", nombre);
			// printf("Control apellido: %s\n", apellido);
			if(strcmp(nombre_usuario, usuario) == 0){
				encontrado = TRUE;
				usuario_encontrado = (USUARIO *)malloc(sizeof(USUARIO));
				strcpy(usuario_encontrado->usuario, usuario);
				strcpy(usuario_encontrado->clave, clave);
				strcpy(usuario_encontrado->nombre, nombre);
				strcpy(usuario_encontrado->apellido, apellido);
			}
		}
		fclose(archivo);
	}

	return usuario_encontrado;
}

BOOLEAN agregar_usuario(char * usuario, char * clave, char * nombre, char * apellido){
	FILE * archivo;
	BOOLEAN resultado = FALSE;
	archivo = fopen (ARCHIVO_USUARIOS, "a");
	// printf("usuario : %s\n",usuario );
	if(archivo != NULL){
		fprintf(archivo, "%s %s %s %s\n", usuario, clave, nombre, apellido);
		fclose(archivo);
		resultado = TRUE;
	}

	return resultado;
}

BOOLEAN crear_carpeta_usuario(char * usuario){
	char folder [] = CARPETA_ALBUMES;
	char * route = (char *)malloc(strlen(folder) + strlen(usuario));
	int resultado;

	strcpy(route, folder);
	strcat(route, usuario);

	resultado = mkdir(route, 0777);
	if(resultado == 0){
		return TRUE;
	}
	return FALSE;
}

BOOLEAN listar_usuarios(FILE * fp){
	FILE * archivo;
	char usuario[MAX_USUARIO];
	char clave[MAX_CLAVE];
	char nombre[MAX_NOMBRE];
	char apellido[MAX_APELLIDO];
	int i = 1;

	BOOLEAN respuesta = TRUE;

	archivo = fopen(ARCHIVO_USUARIOS, "r");

	if(archivo != NULL){
		while(fscanf(archivo, "%s", usuario) > 0){
			fscanf(archivo, "%s", clave);
			fscanf(archivo, "%s", nombre);
			fscanf(archivo, "%s", apellido);

			fprintf(fp, "%d. Usuario: %s, Nombre: %s, Apellido: %s\n", i, usuario, nombre, apellido);
			i++;
		}
		fclose(archivo);
	}

	fclose(fp);

	return respuesta;
}