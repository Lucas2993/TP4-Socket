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

#define M_INICIAR_SESION 1
#define M_REGISTRO 2

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


#endif /* DEFINITIONS_H_ */
