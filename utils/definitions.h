/*
 * definitions.h
 *
 *  Created on: 12 may. 2017
 *      Author: lucas
 */

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

typedef enum boolean{
	FALSE, TRUE
}BOOLEAN;

#define MAXLINEA 512
#define MAXCLI   10
#define MAXPATH 100

#define MAX_USUARIO 16
#define MAX_CLAVE 15
#define MAX_NOMBRE 16
#define MAX_APELLIDO 16

#define MAX_MENSAJE_CONFIRMACION 61

#define MAX_MENSAJE_ERROR 62

#define MAX_NOMBRE_SOLICITUD 59

#define MAX_DATOS 502

#define M_INICIAR_SESION 1
#define M_REGISTRO 2
#define M_CONFIRMAR 3
#define M_ERROR 4
#define M_SOLICITUD 5
#define M_ENVIAR 6
#define M_CERRAR_SESION 7

#define SubOP_Listar_albumes 1
#define SubOP_Crear_album 2
#define SubOP_Modificar_album 3
#define SubOP_Eliminar_album 4
#define SubOP_Listar_archivos_album 5
#define SubOP_Subir_archivo_album 6
#define SubOP_Modificar_archivo_album 7
#define SubOP_Eliminar_archivo_album 8
#define SubOP_Compartir_album_usuario 9
#define SubOP_Dejar_compartir_album_usuario 10
#define SubOP_Listar_usuario 11

typedef struct inciar_sesion{
	char OP;
	char usuario [MAX_USUARIO];
	char clave [MAX_CLAVE];
}INICIAR_SESION;

typedef struct registro{
	char OP;
	char usuario [MAX_USUARIO];
	char clave [MAX_CLAVE];
	char nombre_completo[MAX_NOMBRE];
	char apellido [MAX_APELLIDO];
}REGISTRO;

typedef struct confirmar{
	char OP;
	char ID_Usuario;
	char ID_SUB_OP;
	char mensaje [MAX_MENSAJE_CONFIRMACION];
}CONFIRMAR;

typedef struct error{
	char OP;
	char ID_SUB_OP_Fallo;
	char mensaje [MAX_MENSAJE_ERROR];
}ERROR;

typedef struct solicitud{
	char OP;
	char ID_Usuario;
	char ID_SUB_OP;
	char ID_Album;
	char ID_Archivo;
	char nombre [MAX_NOMBRE_SOLICITUD];
}SOLICITUD;

typedef struct envio{
	char OP;
	char Tipo_Archivo;
	int Longitud_Datos;
	char Datos [MAX_DATOS];
}ENVIO;

typedef struct cerrar_sesion{
	char OP;
	char ID_Usuario;
}CERRAR_SESION;


#endif /* DEFINITIONS_H_ */
