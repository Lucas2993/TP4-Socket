

/* Client side of an ftp service.  Actions:
   - connect to server and request service
   - send size-of-sile info to server
   - start receiving file from server
   - close connection
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>
#include "../utils/definitions.h"
#include "../utils/ftp_utils.h"
#include "client_ftp.h"

int iniciar_cliente_ftp(int puerto_servidor, char * direccion){
  int sockid, newsockid,i,getfile,ack,msg,msg_2,c,len;
  int no_writen,start_xfer, num_blks,num_last_blk;
  struct sockaddr_in my_addr, server_addr; 
  FILE *fp;
  
  no_writen = 0;
  num_blks = 0;
  num_last_blk = 0;
  // len = strlen(path);
  // printf("Cliente TCP: Creando socket\n");
  if((sockid = socket(AF_INET,SOCK_STREAM,0)) < 0){
    printf("Cliente TCP: Error creando el socket: %d\n", errno);
    exit(0);
  }
                                             
  // printf("Cliente TCP: Empezando conexion\n");
  bzero((char *) &server_addr,sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(direccion);
  server_addr.sin_port = htons(puerto_servidor);
  if(connect(sockid ,(struct sockaddr *) &server_addr,sizeof(server_addr)) < 0){
    printf("Cliente TCP: Error de conexion: %d\n", errno);
    exit(0);
  }

  return sockid;
}

BOOLEAN subir_archivo(char * ruta, int sockid, char * usuario, int id_usuario, int id_album){
  SOLICITUD * mensaje_solicitud;
  CONFIRMAR * mensaje_confirmacion;
  ENVIO * mensaje_envio;
  char msg[MAXLINEA];
  int longitud;
  FILE *fp;
  int fsize, ack, c, no_read,i, cantidad_recibido, cantidad_enviado;
  int num_blks, num_blks1, num_last_blk, num_last_blk1;

  mensaje_solicitud = (SOLICITUD *)malloc(sizeof(SOLICITUD));

  mensaje_solicitud->OP = M_SOLICITUD;
  mensaje_solicitud->ID_Usuario = id_usuario;
  mensaje_solicitud->ID_SUB_OP = SubOP_Subir_archivo_album;
  mensaje_solicitud->ID_Album = id_album;
  mensaje_solicitud->ID_Archivo = 0;

  strcpy(mensaje_solicitud->nombre, usuario);
  strcat(mensaje_solicitud->nombre, ";");
  strcat(mensaje_solicitud->nombre, (strrchr(ruta,'/')+1));

  if((write(sockid,(void *)mensaje_solicitud,sizeof(SOLICITUD))) < 0){
    printf("Cliente TCP: Error de escritura en el socket: %d\n", errno);
    exit(0);
  }

  if((read(sockid,(char *)msg,MAXLINEA))< 0){
    printf("Cliente TCP: Error de lectura en el socket: %d\n", errno);
    exit(0);
  }

  // TODO reconocimiento de mensajes tanto de confirmacion como de error.

  if(msg[0] == M_CONFIRMAR){
    mensaje_confirmacion = (CONFIRMAR *)msg;

    // Hasta aca llegue que es el momento donde me confirma la recepcion de los
    // datos del archivo.
    if(strcmp(mensaje_confirmacion->mensaje, "OK") == 0)
      printf("Cliente TCP: El servidor ha validado los datos sobre el archivo y su destino\n");
  }

  return enviar_archivo_socket("Cliente TCP", sockid, ruta);
}


BOOLEAN descargar_archivo(char * ruta_destino, int sockid, char * usuario, int id_usuario, int id_album, int id_archivo){
  SOLICITUD * mensaje_solicitud;
  CONFIRMAR * mensaje_confirmacion;
  char msg[MAXLINEA];
  char * ruta;

  mensaje_solicitud = (SOLICITUD *)malloc(sizeof(SOLICITUD));

  mensaje_solicitud->OP = M_SOLICITUD;
  mensaje_solicitud->ID_Usuario = id_usuario;
  mensaje_solicitud->ID_SUB_OP = SubOP_Descargar_archivo_album;
  mensaje_solicitud->ID_Album = id_album;
  mensaje_solicitud->ID_Archivo = id_archivo;
  strcpy(mensaje_solicitud->nombre, usuario);

  if((write(sockid,(void *)mensaje_solicitud,sizeof(SOLICITUD))) < 0){
    printf("Cliente TCP: Error de escritura en el socket: %d\n", errno);
    exit(0);
  }

  if((read(sockid,(char *)msg,MAXLINEA))< 0){
    printf("Cliente TCP: Error de escritura en el socket: %d\n", errno);
    exit(0);
  }

  // TODO reconocimiento de mensajes tanto de confirmacion como de error.

  if(msg[0] == M_CONFIRMAR){
    mensaje_confirmacion = (CONFIRMAR *)msg;

    // Hasta aca llegue que es el momento donde me confirma la recepcion de los
    // datos del archivo.
    ruta = (char *)malloc(strlen(ruta_destino) + strlen("/") + strlen(mensaje_confirmacion->mensaje));
    strcpy(ruta, ruta_destino);
    strcat(ruta, "/");
    strcat(ruta, mensaje_confirmacion->mensaje);
  }

  return recibir_archivo_socket("Cliente TCP", sockid, ruta);
}

BOOLEAN listar_albumes_usuario(int sockid, int id_usuario){
  SOLICITUD * mensaje_solicitud;
  CONFIRMAR * mensaje_confirmacion;
  char msg[MAXLINEA];
  char buffer[MAXLINEA];
  char * ruta;
  FILE * fp;

  mensaje_solicitud = (SOLICITUD *)malloc(sizeof(SOLICITUD));

  mensaje_solicitud->OP = M_SOLICITUD;
  mensaje_solicitud->ID_Usuario = id_usuario;
  mensaje_solicitud->ID_SUB_OP = SubOP_Listar_albumes;
  mensaje_solicitud->ID_Album = 0;
  mensaje_solicitud->ID_Archivo = 0;

  if((write(sockid,(void *)mensaje_solicitud,sizeof(SOLICITUD))) < 0){
    printf("Cliente TCP: Error de escritura en el socket: %d\n", errno);
    exit(0);
  }

  if((read(sockid,(char *)msg,MAXLINEA))< 0){
    printf("Cliente TCP: Error de escritura en el socket: %d\n", errno);
    exit(0);
  }

  // TODO reconocimiento de mensajes tanto de confirmacion como de error.

  if(msg[0] == M_CONFIRMAR){
    mensaje_confirmacion = (CONFIRMAR *)msg;

    // Hasta aca llegue que es el momento donde me confirma la recepcion de los
    // datos del archivo.
    ruta = (char *)malloc(strlen(ARCHIVO_TEMPORAL_BASE) + strlen(EXTENSION_ARCHIVO_TEMPORAL));
    strcpy(ruta, ARCHIVO_TEMPORAL_BASE);
    strcat(ruta, EXTENSION_ARCHIVO_TEMPORAL);
  }

  recibir_archivo_socket("-", sockid, ruta);

  if((fp = fopen(ruta, "r")) != NULL){
    printf("\n");
    printf("Albumes propios:\n");
    bzero(buffer, MAXLINEA);
    while(fgets(buffer, MAXLINEA, fp) != NULL){
      printf("%s", buffer);    
    }
    if(strlen(buffer) == 0){
      printf("Usted no tiene albumes registrados.\n");
    }
    close(fp);
    remove(ruta);
  }

  return TRUE;
}

BOOLEAN listar_archivos_usuario(int sockid, int id_usuario, int id_album){
  SOLICITUD * mensaje_solicitud;
  CONFIRMAR * mensaje_confirmacion;
  char msg[MAXLINEA];
  char buffer[MAXLINEA];
  char * ruta;
  FILE * fp;

  mensaje_solicitud = (SOLICITUD *)malloc(sizeof(SOLICITUD));

  mensaje_solicitud->OP = M_SOLICITUD;
  mensaje_solicitud->ID_Usuario = id_usuario;
  mensaje_solicitud->ID_SUB_OP = SubOP_Listar_archivos_album;
  mensaje_solicitud->ID_Album = id_album;
  mensaje_solicitud->ID_Archivo = 0;

  if((write(sockid,(void *)mensaje_solicitud,sizeof(SOLICITUD))) < 0){
    printf("Cliente TCP: Error de escritura en el socket: %d\n", errno);
    exit(0);
  }

  if((read(sockid,(char *)msg,MAXLINEA))< 0){
    printf("Cliente TCP: Error de lectura en el socket: %d\n", errno);
    exit(0);
  }

  // TODO reconocimiento de mensajes tanto de confirmacion como de error.

  if(msg[0] == M_CONFIRMAR){
    mensaje_confirmacion = (CONFIRMAR *)msg;

    // Hasta aca llegue que es el momento donde me confirma la recepcion de los
    // datos del archivo.
    ruta = (char *)malloc(strlen(ARCHIVO_TEMPORAL_BASE) + strlen(EXTENSION_ARCHIVO_TEMPORAL));
    strcpy(ruta, ARCHIVO_TEMPORAL_BASE);
    strcat(ruta, EXTENSION_ARCHIVO_TEMPORAL);
  }

  recibir_archivo_socket("-", sockid, ruta);

  if((fp = fopen(ruta, "r")) != NULL){
    printf("\n");
    printf("Archivos:\n");
    bzero(buffer, MAXLINEA);
    while(fgets(buffer, MAXLINEA, fp) != NULL){
      printf("%s", buffer);    
    }
    if(strlen(buffer) == 0){
      printf("El album seleccionado no posee archivos registrados.\n");
    }
    close(fp);
    remove(ruta);
  }

  return TRUE;
}

BOOLEAN listar_usuarios_registrados(int sockid, int id_usuario){
  SOLICITUD * mensaje_solicitud;
  CONFIRMAR * mensaje_confirmacion;
  char msg[MAXLINEA];
  char buffer[MAXLINEA];
  char * ruta;
  FILE * fp;

  mensaje_solicitud = (SOLICITUD *)malloc(sizeof(SOLICITUD));

  mensaje_solicitud->OP = M_SOLICITUD;
  mensaje_solicitud->ID_Usuario = id_usuario;
  mensaje_solicitud->ID_SUB_OP = SubOP_Listar_usuarios;
  mensaje_solicitud->ID_Album = 0;
  mensaje_solicitud->ID_Archivo = 0;

  if((write(sockid,(void *)mensaje_solicitud,sizeof(SOLICITUD))) < 0){
    printf("Cliente TCP: Error de escritura en el socket: %d\n", errno);
    exit(0);
  }

  if((read(sockid,(char *)msg,MAXLINEA))< 0){
    printf("Cliente TCP: Error de lectura en el socket: %d\n", errno);
    exit(0);
  }

  // TODO reconocimiento de mensajes tanto de confirmacion como de error.

  if(msg[0] == M_CONFIRMAR){
    mensaje_confirmacion = (CONFIRMAR *)msg;

    // Hasta aca llegue que es el momento donde me confirma la recepcion de los
    // datos del archivo.
    ruta = (char *)malloc(strlen(ARCHIVO_TEMPORAL_BASE) + strlen(EXTENSION_ARCHIVO_TEMPORAL));
    strcpy(ruta, ARCHIVO_TEMPORAL_BASE);
    strcat(ruta, EXTENSION_ARCHIVO_TEMPORAL);
  }

  recibir_archivo_socket("-", sockid, ruta);

  if((fp = fopen(ruta, "r")) != NULL){
    printf("\n");
    printf("Usuarios:\n");
    bzero(buffer, MAXLINEA);
    while(fgets(buffer, MAXLINEA, fp) != NULL){
      printf("%s", buffer);    
    }
    if(strlen(buffer) == 0){
      printf("Usted no tiene usuarios registrados.\n");
    }
    close(fp);
    remove(ruta);
  }

  return TRUE;
}

BOOLEAN listar_albumes_compartidos_usuario(int sockid, int id_usuario){
  SOLICITUD * mensaje_solicitud;
  CONFIRMAR * mensaje_confirmacion;
  char msg[MAXLINEA];
  char buffer[MAXLINEA];
  char * ruta;
  FILE * fp;

  mensaje_solicitud = (SOLICITUD *)malloc(sizeof(SOLICITUD));

  mensaje_solicitud->OP = M_SOLICITUD;
  mensaje_solicitud->ID_Usuario = id_usuario;
  mensaje_solicitud->ID_SUB_OP = SubOP_Listar_albumes_compartidos_conmigo;
  mensaje_solicitud->ID_Album = 0;
  mensaje_solicitud->ID_Archivo = 0;

  if((write(sockid,(void *)mensaje_solicitud,sizeof(SOLICITUD))) < 0){
    printf("Cliente TCP: Error de escritura en el socket: %d\n", errno);
    exit(0);
  }

  if((read(sockid,(char *)msg,MAXLINEA))< 0){
    printf("Cliente TCP: Error de lectura en el socket: %d\n", errno);
    exit(0);
  }

  // TODO reconocimiento de mensajes tanto de confirmacion como de error.

  if(msg[0] == M_CONFIRMAR){
    mensaje_confirmacion = (CONFIRMAR *)msg;

    // Hasta aca llegue que es el momento donde me confirma la recepcion de los
    // datos del archivo.
    ruta = (char *)malloc(strlen(ARCHIVO_TEMPORAL_BASE) + strlen(EXTENSION_ARCHIVO_TEMPORAL));
    strcpy(ruta, ARCHIVO_TEMPORAL_BASE);
    strcat(ruta, EXTENSION_ARCHIVO_TEMPORAL);
  }

  recibir_archivo_socket("-", sockid, ruta);

  if((fp = fopen(ruta, "r")) != NULL){
    printf("\n");
    printf("Albumes compartidos conmigo:\n");
    bzero(buffer, MAXLINEA);
    while(fgets(buffer, MAXLINEA, fp) != NULL){
      printf("%s", buffer);    
    }
    if(strlen(buffer) == 0){
      printf("Usted no tiene albumes que se le hayan compartido.\n");
    }
    close(fp);
    remove(ruta);
  }

  return TRUE;
}

BOOLEAN listar_albumes_compartidos_otros(int sockid, int id_usuario){
  SOLICITUD * mensaje_solicitud;
  CONFIRMAR * mensaje_confirmacion;
  char msg[MAXLINEA];
  char buffer[MAXLINEA];
  char * ruta;
  FILE * fp;

  mensaje_solicitud = (SOLICITUD *)malloc(sizeof(SOLICITUD));

  mensaje_solicitud->OP = M_SOLICITUD;
  mensaje_solicitud->ID_Usuario = id_usuario;
  mensaje_solicitud->ID_SUB_OP = SubOP_Listar_albumes_compartidos_otros;
  mensaje_solicitud->ID_Album = 0;
  mensaje_solicitud->ID_Archivo = 0;

  if((write(sockid,(void *)mensaje_solicitud,sizeof(SOLICITUD))) < 0){
    printf("Cliente TCP: Error de escritura en el socket: %d\n", errno);
    exit(0);
  }

  if((read(sockid,(char *)msg,MAXLINEA))< 0){
    printf("Cliente TCP: Error de lectura en el socket: %d\n", errno);
    exit(0);
  }

  // TODO reconocimiento de mensajes tanto de confirmacion como de error.

  if(msg[0] == M_CONFIRMAR){
    mensaje_confirmacion = (CONFIRMAR *)msg;

    // Hasta aca llegue que es el momento donde me confirma la recepcion de los
    // datos del archivo.
    ruta = (char *)malloc(strlen(ARCHIVO_TEMPORAL_BASE) + strlen(EXTENSION_ARCHIVO_TEMPORAL));
    strcpy(ruta, ARCHIVO_TEMPORAL_BASE);
    strcat(ruta, EXTENSION_ARCHIVO_TEMPORAL);
  }

  recibir_archivo_socket("-", sockid, ruta);

  if((fp = fopen(ruta, "r")) != NULL){
    printf("\n");
    printf("Albumes compartidos con otros usuarios:\n");
    bzero(buffer, MAXLINEA);
    while(fgets(buffer, MAXLINEA, fp) != NULL){
      printf("%s", buffer);    
    }
    if(strlen(buffer) == 0){
      printf("Usted no tiene albumes compartidos con otros usuarios.\n");
    }
    close(fp);
    remove(ruta);
  }

  return TRUE;
}