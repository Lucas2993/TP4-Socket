#define ARCHIVO_COMPARTIDOS_CONMIGO "/compartidos_conmigo.txt"
#define ARCHIVO_COMPARTIDOS_OTROS "/compartidos_otros.txt"
#define ARCHIVO_COMPARTIDOS_CONMIGO_AUX "/compartidos_conmigo_aux.txt"
#define ARCHIVO_COMPARTIDOS_OTROS_AUX "/compartidos_otros_aux.txt"

BOOLEAN compartir_album_usuario(char *, char *, char *, int);
BOOLEAN dejar_compartir_album_usuario(char *, char *, char *, int);
BOOLEAN listar_albumes_compartidos_conmigo(FILE *, char *);
BOOLEAN listar_albumes_compartidos_otro(FILE *, char *);
BOOLEAN crear_archivos_compartir(char *);