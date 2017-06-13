#include "../utils/definitions.h"

BOOLEAN iniciar_servidor_ftp(int);
void atender_cliente(int);
void recepcion_archivo(int, SOLICITUD *);
void envio_archivo(int, SOLICITUD *);
void doftp(int);
void listar_albumes_usuario(int, SOLICITUD *);
void listar_archivos_album_usuario(int, SOLICITUD *);
void listar_usuarios_registrados(int, SOLICITUD *);
void listar_albumes_compartidos_usuario(int, SOLICITUD *);