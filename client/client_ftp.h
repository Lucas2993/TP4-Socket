#include "../utils/definitions.h"

int iniciar_cliente_ftp(int, char *);
BOOLEAN subir_archivo(char *, int, int, int);
BOOLEAN descargar_archivo(char *, int, int, int, int);
BOOLEAN listar_albumes_usuario(int, int);
BOOLEAN listar_archivos_usuario(int, int, int);
BOOLEAN listar_usuarios_registrados(int, int);
BOOLEAN listar_albumes_compartidos_usuario(int, int);