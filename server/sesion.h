#include "../utils/definitions.h"

#define ARCHIVO_SESIONES "sesiones.txt"
#define ARCHIVO_AUXILIAR "sesiones-aux.txt"

BOOLEAN agregar_sesion(char *, int);
int buscar_sesion_por_usuario(char *);
int asignar_numero_sesion(void);
BOOLEAN cerrar_sesion_usuario(int);