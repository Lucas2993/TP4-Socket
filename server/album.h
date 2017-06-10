#define ARCHIVO_ALBUM "albumes.txt"
#define ARCHIVO_AUXILIAR_ALBUM "albumes-aux.txt"

BOOLEAN crear_album(char *, char *);
BOOLEAN registrar_album(char *, char *);
int siguiente_id_album(char *);
BOOLEAN crear_carpeta_general_albumes(void);
char * buscar_album_id(char *, int);
char * crear_ruta(char *, char *, char *);
BOOLEAN eliminar_album(char * nombre, char * usuario,int id);
BOOLEAN eliminar_album_de_lista( char * usuario, int id_album );
BOOLEAN listar_albumes(FILE *, char *);