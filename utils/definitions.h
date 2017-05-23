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

#define MAX_USUARIO 16
#define MAX_CLAVE 15
#define MAX_NOMBRE 16
#define MAX_APELLIDO 16

#define MAX_MENSAJE_CONFIRMACION 61

#define MAX_MENSAJE_ERROR 62

#define MAX_NOMBRE_SOLICITUD 59

#define MAX_DATOS 509

#define M_INICIAR_SESION 1
#define M_REGISTRO 2
#define M_CONFIRMAR 3
#define M_ERROR 4

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
	char ID_usuario;
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
	char Longitud_Datos;
	char Datos [MAX_DATOS];
}ENVIO;

typedef struct cerrar_sesion{
	char OP;
	char ID_Usuario;
}CERRAR_SESION;


#endif /* DEFINITIONS_H_ */
