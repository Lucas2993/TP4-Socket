#define ARCHIVO_AUXILIAR_ARCHIVOS "archivo-aux.txt"

BOOLEAN registrar_archivo(char *, char *, char *);
int siguiente_id_archivo(char *);
char * buscar_archivo_id(char *, char *, int);
BOOLEAN eliminar_archivo(char *, char *, char *);
BOOLEAN listar_archivos(FILE *, char *, char *);
BOOLEAN eliminar_archivo_de_lista(char *, char *, int);