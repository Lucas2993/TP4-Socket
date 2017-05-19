/*
 * definitions.h
 *
 *  Created on: 12 may. 2017
 *      Author: lucas
 */

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

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

struct inciar_sesion{
	char OP;
	char usuario [MAX_USUARIO];
	char clave [MAX_CLAVE];
};

struct registro{
	char OP;
	char usuario [MAX_USUARIO];
	char clave [MAX_CLAVE];
	char nombre_completo[MAX_NOMBRE];
	char apellido [MAX_APELLIDO];
};

struct confirmar{
	char OP;
	char ID_usuario;
	char ID_SUB_OP;
	char mensaje [MAX_MENSAJE_CONFIRMACION];
};

struct error{
	char OP;
	char ID_SUB_OP_Fallo;
	char mensaje [MAX_MENSAJE_ERROR];
};

struct solicitud{
	char OP;
	char ID_Usuario;
	char ID_SUB_OP;
	char ID_Album;
	char ID_Archivo;
	char nombre [MAX_NOMBRE_SOLICITUD];
};

struct envio{
	char OP;
	char Tipo_Archivo;
	char Longitud_Datos;
	char Datos [MAX_DATOS];
};

struct cerrar_sesion{
	char OP;
	char ID_Usuario;
};


#endif /* DEFINITIONS_H_ */
