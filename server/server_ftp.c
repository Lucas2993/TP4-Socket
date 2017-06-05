
/* This is the server for a very simple file transfer
   service.  This is a "concurrent server" that can
   handle requests from multiple simultaneous clients.
   For each client:
    - get file name and check if it exists
    - send size of file to client
    - send file to client, a block at a time
    - close connection with client
*/

#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>
#include "../utils/definitions.h"
#include "album.h"
#include "sesion.h"
#include "server_ftp.h"

BOOLEAN iniciar_servidor_ftp(int puerto){

  int sockid, newsd, pid, clilen = 0;
  struct sockaddr_in my_addr, client_addr;   

  printf("Servidor TCP: Creando socket\n");
  if ((sockid = socket(AF_INET,SOCK_STREAM,0)) < 0){
    printf("Servidor TCP: Error creando el socket: %d\n", errno);
    exit(0);
  }

  printf("Servidor TCP: Creando enlace con socket local\n");
  bzero((char *) &my_addr,sizeof(my_addr));
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(puerto);
  my_addr.sin_addr.s_addr = htons(INADDR_ANY);
  if (bind(sockid ,(struct sockaddr *) &my_addr,sizeof(my_addr)) < 0){
    printf("Servidor TCP: Error al crear el enlace: %d\n", errno);
    exit(0);
  }

  printf("Servidor TCP: Escuchando\n");
  if (listen(sockid,5) < 0){
    printf("Servidor TCP: Error al intentar escuchar: %d\n",errno);
    exit(0);
  }                                        

  while(1==1) { 
    /* ACCEPT A CONNECTION AND THEN CREATE A CHILD TO DO THE WORK */
    /* LOOP BACK AND WAIT FOR ANOTHER CONNECTION                  */
    printf("Servidor TCP: Esperando conexion\n");
    if((newsd = accept(sockid ,(struct sockaddr *) &client_addr,&clilen)) < 0){
      printf("Servidor TCP: Error de conexion: %d\n", errno); 
      exit(0);
    }
    printf("Servidor TCP: Conexion establecida, el socket asignado es el: %d\n",newsd);

    if((pid=fork()) == 0) {
      /* CHILD PROC STARTS HERE. IT WILL DO ACTUAL FILE TRANSFER */
      close(sockid);   /* child shouldn't do an accept */
      doftp(newsd);
      close (newsd);
      exit(0);         /* child all done with work */
    }
    /* PARENT CONTINUES BELOW HERE */
    close(newsd);        /* parent all done with client, only child */
  }              /* will communicate with that client from now on */
}
     

/* CHILD PROCEDURE, WHICH ACTUALLY DOES THE FILE TRANSFER */
void doftp(int newsd){       
  int i,fsize,fd,msg_ok,fail,fail1,req,c,ack;
  int no_read,no_writen,num_blks , num_blks1,num_last_blk,num_last_blk1,tmp;
  char fname[MAXLINE];
  char out_buf[MAXSIZE];
  FILE *fp;

  char msg[MAXLINEA];
  int longitud;
  int cantidad_recibido, cantidad_enviado;

  SOLICITUD * mensaje_solicitud;
  ERROR * mensaje_error;
  CONFIRMAR * mensaje_confirmacion;
  ENVIO * mensaje_envio;

  char * usuario;
  char * album;
  char * ruta;
      
  no_read = 0;
  num_blks = 0;
  num_last_blk = 0; 

    
  // Se espera un mensaje de solicitud que proporcione el nombre del achivo, el album
  // y el usuario al que pertenece para determinar el destino del archivo a recibir.

  if((read(newsd,(char *)msg,MAXLINEA)) < 0){
    printf("Servidor TCP: Error de lectura: %d\n",errno);
    exit(0);
  }

  if(msg[0] == M_SOLICITUD){
    mensaje_solicitud = (SOLICITUD *)msg;

    // Obtener nombre usuario.
    usuario = buscar_usuario_por_sesion(mensaje_solicitud->ID_Usuario);
    // Obtener nombre del album.
    album = buscar_album_id(usuario, mensaje_solicitud->ID_Album);
    // Armar ruta completa del archivo a almacenar.
    ruta = crear_ruta(usuario, album, mensaje_solicitud->nombre);

    printf("%s\n", mensaje_solicitud->nombre);
    printf("%s\n", ruta);
  }

  mensaje_confirmacion = (CONFIRMAR *)malloc(sizeof(CONFIRMAR));

  mensaje_confirmacion->OP = M_CONFIRMAR;
  mensaje_confirmacion->ID_Usuario = mensaje_solicitud->ID_Usuario;
  mensaje_confirmacion->ID_SUB_OP = mensaje_solicitud->ID_SUB_OP;
  strcpy(mensaje_confirmacion->mensaje, "OK");

  // Se envia un mensaje al cliente aceptando el archivo y validando los datos.
  if((write(newsd,(void *)mensaje_confirmacion,sizeof(CONFIRMAR))) < 0){
    printf("Servidor TCP: Error al enviar el mensaje de confirmacion de datos: %d\n",errno);
    exit(0);
  }

  // Se recibe un mensaje con la cantidad de bloques a transferir.
  if((read(newsd,(char *)msg,MAXLINEA)) < 0){
    printf("Servidor TCP: Error leyendo la cantidad de bloques: %d\n",errno);
    exit(0);
  }

  if(msg[0] == M_SOLICITUD){
    mensaje_solicitud = (SOLICITUD *)msg;

    num_blks = atoi(mensaje_solicitud->nombre);
  }

  // TODO realizar alguna comprobacion.

  // Se envia un mensaje al cliente aceptando la cantidad de bloques.
  if((write(newsd,(void *)mensaje_confirmacion,sizeof(CONFIRMAR))) < 0){
    printf("Servidor TCP: Error al enviar el mensaje de confirmacion de cantidad de bloques: %d\n",errno);
    exit(0);
  }


  // Se recibe un mensaje con la dimension del ultimo bloque.
  if((read(newsd,(char *)msg,MAXLINEA)) < 0){
    printf("Servidor TCP: Error leyendo el mensaje con la dimension del ultimo bloque: %d\n",errno);
    exit(0);
  }

  if(msg[0] == M_SOLICITUD){
    mensaje_solicitud = (SOLICITUD *)msg;

    num_last_blk = atoi(mensaje_solicitud->nombre);
  }

  // TODO realizar alguna comprobacion.

  // Se envia un mensaje al cliente aceptando la dimension del ultimo bloque.
  if((write(newsd,(void *)mensaje_confirmacion,sizeof(CONFIRMAR))) < 0){
    printf("Servidor TCP: Error al enviar la confirmacion de la dimension del ultimo bloque: %d\n",errno);
    exit(0);
  }

  // Se abre el archivo que sera el destino.
  if((fp = fopen(ruta,"w")) == NULL){
    printf(" Servidor TCP: Error al abrir el archivo de destino\n");
    exit(0);
  }

  // Se comienza con la recepcion del archivo.
  for(i= 0; i < num_blks; i ++) {
    cantidad_recibido = read(newsd,(char *)msg,MAXLINEA);
    // printf("FOR: Cantidad recibido: %d\n", cantidad_recibido);
    if(cantidad_recibido < 0){
      printf("Servidor TCP: Error al leer bloque de datos del socket: %d\n",errno);
      exit(0);
    }
    if(msg[0] == M_ENVIAR){
      mensaje_envio = (ENVIO *)msg;
    }
    else{
      printf("Servidor TCP: Error: No se reconocio el mensaje de envio.\n");
      exit(0);
    }
    no_writen = fwrite(mensaje_envio->Datos,sizeof(char),mensaje_envio->Longitud_Datos,fp);
    if(no_writen == 0){
      printf("Servidor TCP: Error al escribir el archivo de destino.\n");
      exit(0);
    }
    if(no_writen != mensaje_envio->Longitud_Datos){
      printf("Servidor TCP: Error: La cantidad de datos recibida es diferente a la cantidad escrita en el archivo de destino\n");
      exit(0);
    }
    cantidad_enviado = write(newsd,(void *)mensaje_confirmacion,sizeof(CONFIRMAR));
    // printf("FOR: Cantidad enviado: %d\n", cantidad_enviado);
    if(cantidad_enviado < 0){
      printf("Servidor TCP: Error al escribir el mensaje de confirmacion: %d\n",errno);
      exit(0);
    }
    printf(" %d...",i);
  }

  if(num_last_blk > 0) {
    printf("%d\n",num_blks);
    cantidad_recibido = read(newsd,(char *)msg,MAXLINEA);
    // printf("LAST: Cantidad recibido: %d\n", cantidad_recibido); 
    if(cantidad_recibido < 0){
      printf("Servidor TCP: Error al leer bloque de datos del socket: %d\n",errno);
      exit(0);
    }
    if(msg[0] == M_ENVIAR){
      mensaje_envio = (ENVIO *)msg;
    }
    else{
      printf("Servidor TCP: Error: No se reconocio el mensaje de envio.\n");
      exit(0);
    }
    no_writen = fwrite(mensaje_envio->Datos,sizeof(char),mensaje_envio->Longitud_Datos,fp); 
    if(no_writen == 0){
      printf("Servidor TCP: Error al escribir el archivo de destino.\n");
      exit(0);
    }
    // printf("LAST: no_writen: %d\n", no_writen);
    // printf("LAST: Longitud_Datos: %d\n", mensaje_envio->Longitud_Datos);
    if(no_writen != mensaje_envio->Longitud_Datos){
      printf("Servidor TCP: Error: La cantidad de datos recibida es diferente a la cantidad escrita en el archivo de destino\n");
      exit(0);
    }
    cantidad_enviado = write(newsd,(void *)mensaje_confirmacion,sizeof(CONFIRMAR));
    // printf("LAST: Cantidad enviado: %d\n", cantidad_enviado);
    if(cantidad_enviado < 0){
      printf("Servidor TCP: Error al escribir el mensaje de confirmacion: %d\n",errno);
      exit(0);
    }
  }
  else
    printf("\n");

  fclose(fp);

  printf("Servidor TCP: Se ha recibido y almacenado el archivo exitosamente.\n");
  sleep(5);
  close(newsd);
}


/*
  TO TAKE CARE OF THE POSSIBILITY OF BUFFER LIMMITS IN THE KERNEL FOR THE
 SOCKET BEING REACHED (WHICH MAY CAUSE READ OR WRITE TO RETURN FEWER CHARACTERS
  THAN REQUESTED), WE USE THE FOLLOWING TWO FUNCTIONS */  
   
int readn(int sd,char *ptr,int size){
  int no_left,no_read;
  no_left = size;
  while (no_left > 0){
    no_read = read(sd,ptr,no_left);
    if(no_read <0)
      return(no_read);
    if(no_read == 0)
      break;
    no_left -= no_read;
    ptr += no_read;
  }
  return(size - no_left);
}

int writen(int sd,char *ptr,int size){
  int no_left,no_written;
  no_left = size;
  while (no_left > 0){
    no_written = write(sd,ptr,no_left);
    if(no_written <=0)
      return(no_written);
    no_left -= no_written;
    ptr += no_written;
  }
  return(size - no_left);
}