#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>

#include "definitions.h"
#include "ftp_utils.h"

BOOLEAN enviar_archivo_socket(char * identificador_origen, int socket_id, char * ruta_archivo){
	CONFIRMAR * mensaje_confirmacion;
  ENVIO * mensaje_envio;
  char msg[MAXLINEA];
  FILE *fp;
  unsigned int fsize;
  int c, no_read,i, cantidad_recibido, cantidad_enviado;
  int num_blks, numero_bloques;

  num_blks = 0;

  fsize = 0;
  fp = fopen(ruta_archivo,"r");
  while ((c = getc(fp)) != EOF){
    fsize++;
  }

  rewind(fp);

  num_blks = fsize / MAX_DATOS;
  if(fsize % MAX_DATOS > 0){
  	num_blks++;
  }
  // printf("fsize: %d\n", fsize);
  // printf("Cantidad bloques: %d\n", num_blks);
  // printf("Ultimo bloque: %d\n", num_last_blk);

  numero_bloques = htons(num_blks);

  cantidad_enviado = write(socket_id,(char *)&numero_bloques,sizeof(numero_bloques));
  if(cantidad_enviado < 0){
    printf("%s: Error enviando el bloque: %d\n",identificador_origen,errno);
    exit(0);
  }

  // Se comienza la transferencia.
  mensaje_envio = (ENVIO *)malloc(sizeof(ENVIO));

  mensaje_envio->OP = M_ENVIAR;
  mensaje_envio->Tipo_Archivo = 'A';

  // printf("Size of mensaje_envio: %d\n", sizeof(ENVIO));

  for(i= 0; i < num_blks; i ++) {
    mensaje_envio->Longitud_Datos = fread(mensaje_envio->Datos,sizeof(char),MAX_DATOS,fp);
    if (mensaje_envio->Longitud_Datos == 0){
      printf("%s: Error leyendo el archivo\n", identificador_origen);
      exit(0);
    }
    // if (no_read != MAX_DATOS){
    //   printf("Client: file read error : no_read is less\n");
    //   exit(0);
    // }
    cantidad_enviado = write(socket_id,mensaje_envio,sizeof(ENVIO));
    // printf("FOR: Cantidad enviado: %d\n", cantidad_enviado);
    if(cantidad_enviado < 0){
      printf("%s: Error enviando el bloque: %d\n",identificador_origen,errno);
      exit(0);
    }
    cantidad_recibido = read(socket_id,(char *)msg,MAXLINEA);
    // printf("FOR: Cantidad recibido: %d\n", cantidad_recibido);
    if(cantidad_recibido < 0){
      printf("%s: Error leyendo la confirmacion: %d\n",identificador_origen,errno);
      exit(0);
    }
    if(msg[0] == M_CONFIRMAR){
      mensaje_confirmacion = (CONFIRMAR *)msg;

      // Hasta aca llegue que es el momento donde me confirma la recepcion de los
      // datos del archivo.
      if(strcpy(mensaje_confirmacion->mensaje, "OK"))
        printf("%s: El servidor ha validado el bloque enviado.\n",identificador_origen);
    }
    else{
      printf("%s: No se reconocio el mensaje de confirmacion.\n",identificador_origen);
      exit(0);
    }
    printf(" %d...",i);
  }

  fclose(fp);

  printf("%s: Transferencia completada!\n",identificador_origen);

  return TRUE;
}

BOOLEAN recibir_archivo_socket(char * identificador_origen, int socket_id, char * ruta_archivo){
	int i,fsize,fd,req,c;
  int no_writen,num_blks,num_last_blk,tmp,numero_bloques;

  char msg[MAXLINEA];
  int longitud;
  int cantidad_recibido, cantidad_enviado;

  FILE *fp;
  CONFIRMAR * mensaje_confirmacion;
  ENVIO * mensaje_envio;

  num_blks = 0;

  mensaje_confirmacion = (CONFIRMAR *)malloc(sizeof(CONFIRMAR));

  mensaje_confirmacion->OP = M_CONFIRMAR;
  mensaje_confirmacion->ID_Usuario = 0;
  mensaje_confirmacion->ID_SUB_OP = 0;
  strcpy(mensaje_confirmacion->mensaje, "OK");

  cantidad_recibido = read(socket_id,(char *)&numero_bloques,sizeof(numero_bloques));
  if(cantidad_recibido < 0){
    printf("%s: Error al leer bloque de datos del socket: %d\n",identificador_origen,errno);
    exit(0);
  }

  num_blks = ntohs(numero_bloques);

	// Se abre el archivo que sera el destino.
  if((fp = fopen(ruta_archivo,"w")) == NULL){
    printf("%s: Error al abrir el archivo de destino\n",identificador_origen);
    exit(0);
  }

  // Se comienza con la recepcion del archivo.
  for(i= 0; i < num_blks; i ++) {
    cantidad_recibido = read(socket_id,(char *)msg,MAXLINEA);
    // printf("FOR: Cantidad recibido: %d\n", cantidad_recibido);
    if(cantidad_recibido < 0){
      printf("%s: Error al leer bloque de datos del socket: %d\n",identificador_origen,errno);
      exit(0);
    }
    if(msg[0] == M_ENVIAR){
      mensaje_envio = (ENVIO *)msg;
    }
    else{
      printf("%s: Error: No se reconocio el mensaje de envio.\n",identificador_origen);
      exit(0);
    }
    no_writen = fwrite(mensaje_envio->Datos,sizeof(char),mensaje_envio->Longitud_Datos,fp);
    if(no_writen == 0){
      printf("%s: Error al escribir el archivo de destino.\n",identificador_origen);
      exit(0);
    }
    if(no_writen != mensaje_envio->Longitud_Datos){
      printf("%s: Error: La cantidad de datos recibida es diferente a la cantidad escrita en el archivo de destino\n",identificador_origen);
      exit(0);
    }
    cantidad_enviado = write(socket_id,(void *)mensaje_confirmacion,sizeof(CONFIRMAR));
    // printf("FOR: Cantidad enviado: %d\n", cantidad_enviado);
    if(cantidad_enviado < 0){
      printf("%s: Error al escribir el mensaje de confirmacion: %d\n",identificador_origen,errno);
      exit(0);
    }
    printf(" %d...",i);
  }

  fclose(fp);

  printf("%s: Se ha recibido y almacenado el archivo exitosamente.\n",identificador_origen);
  close(socket_id);
}

/*
  TO TAKE CARE OF THE POSSIBILITY OF BUFFER LIMMITS IN THE KERNEL FOR THE
 SOCKET BEING REACHED (WHICH MAY CAUSE READ OR WRITE TO RETURN FEWER CHARACTERS
  THAN REQUESTED), WE USE THE FOLLOWING TWO FUNCTIONS */

// int readn(int sd,char *ptr,int size){
//   int no_left,no_read;
//   no_left = size;
//   while (no_left > 0){
//     no_read = read(sd,ptr,no_left);
//     if(no_read <0)
//       return(no_read);
//     if(no_read == 0)
//       break;
//     no_left -= no_read;
//     ptr += no_read;
//   }
//   return(size - no_left);
// }
//
// int writen(int sd,char *ptr,int size){
//   int no_left,no_written;
//   no_left = size;
//   while (no_left > 0){
//     no_written = write(sd,ptr,no_left);
//     if(no_written <=0)
//       return(no_written);
//     no_left -= no_written;
//     ptr += no_written;
//   }
//   return(size - no_left);
// }
