

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

int iniciar_cliente_ftp(int puerto_cliente, int puerto_servidor, char * direccion){
  int sockid, newsockid,i,getfile,ack,msg,msg_2,c,len;
  int no_writen,start_xfer, num_blks,num_last_blk;
  struct sockaddr_in my_addr, server_addr; 
  FILE *fp;
  char in_buf[MAXSIZE];
  
  no_writen = 0;
  num_blks = 0;
  num_last_blk = 0;
  // len = strlen(path);
  printf("client: creating socket\n");
  if((sockid = socket(AF_INET,SOCK_STREAM,0)) < 0){
    printf("client: socket error : %d\n", errno);
    exit(0);
  }
  
  printf("client: binding my local socket\n");
  bzero((char *) &my_addr,sizeof(my_addr));
  my_addr.sin_family = AF_INET;
  my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  my_addr.sin_port = htons(puerto_cliente);
  if(bind(sockid ,(struct sockaddr *) &my_addr,sizeof(my_addr)) < 0){
    printf("client: bind  error :%d\n", errno);
    exit(0);
  }
                                             
  printf("client: starting connect\n");
  bzero((char *) &server_addr,sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(direccion);
  server_addr.sin_port = htons(puerto_servidor);
  if(connect(sockid ,(struct sockaddr *) &server_addr,sizeof(server_addr)) < 0){
    printf("client: connect  error :%d\n", errno);
    exit(0);
  }

  return sockid;
}

BOOLEAN subir_archivo(char * ruta, int sockid, int id_usuario, int id_album){
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
  strcpy(mensaje_solicitud->nombre, (strrchr(ruta,'/')+1));

  if((write(sockid,(void *)mensaje_solicitud,sizeof(SOLICITUD))) < 0){
    printf("client: write  error :%d\n", errno);
    exit(0);
  }

  if((read(sockid,(char *)msg,MAXLINEA))< 0){
    printf("client: read  error :%d\n", errno);
    exit(0);
  }

  // TODO reconocimiento de mensajes tanto de confirmacion como de error.

  if(msg[0] == M_CONFIRMAR){
    mensaje_confirmacion = (CONFIRMAR *)msg;

    // Hasta aca llegue que es el momento donde me confirma la recepcion de los
    // datos del archivo.
    if(strcpy(mensaje_confirmacion->mensaje, "OK"))
      printf("El servidor ha validado los datos sobre el archivo y su destino\n");
  }

  return enviar_archivo_socket("Cliente", sockid, ruta);
}


BOOLEAN descargar_archivo(char * ruta_destino, int sockid, int id_usuario, int id_album, int id_archivo){
  // SOLICITUD * mensaje_solicitud;

  // mensaje_solicitud = (SOLICITUD *)malloc(sizeof(SOLICITUD));

  // mensaje_solicitud->OP = M_SOLICITUD;
  // mensaje_solicitud->ID_Usuario = id_usuario;
  // mensaje_solicitud->ID_SUB_OP = SubOP_Descargar_archivo_album;
  // mensaje_solicitud->ID_Album = id_album;
  // mensaje_solicitud->ID_Archivo = id_archivo;

  // if((write(sockid,(void *)mensaje_solicitud,sizeof(SOLICITUD))) < 0){
  //   printf("client: write  error :%d\n", errno);
  //   exit(0);
  // }

  // if((read(sockid,(char *)msg,MAXLINEA))< 0){
  //   printf("client: read  error :%d\n", errno);
  //   exit(0);
  // }

  // // TODO reconocimiento de mensajes tanto de confirmacion como de error.

  // if(msg[0] == M_CONFIRMAR){
  //   mensaje_confirmacion = (CONFIRMAR *)msg;

  //   // Hasta aca llegue que es el momento donde me confirma la recepcion de los
  //   // datos del archivo.
  //   if(strcpy(mensaje_confirmacion->mensaje, "OK"))
  //     printf("El servidor ha validado los datos sobre el archivo y su destino\n");
  // }
}