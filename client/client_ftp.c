

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

BOOLEAN enviar_archivo(char * ruta, int sockid, int id_usuario){
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
  mensaje_solicitud->ID_Album = 1;
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

  num_blks = 0;
  num_last_blk = 0; 

  fsize = 0;
  ack = 0;
  fp = fopen(ruta,"r");
  while ((c = getc(fp)) != EOF){
    fsize++;
  }
  num_blks = fsize / MAX_DATOS;
  num_last_blk = fsize % MAX_DATOS;
  // printf("fsize: %d\n", fsize);
  // printf("Cantidad bloques: %d\n", num_blks);
  // printf("Ultimo bloque: %d\n", num_last_blk);


  // Se conserva el mensaje de solicitud tal cual esta y solo se cambia el
  // campo del nombre.
  sprintf(mensaje_solicitud->nombre, "%d", num_blks);

  if((write(sockid,(void *)mensaje_solicitud,sizeof(SOLICITUD))) < 0){
    printf("Client: write error :%d\n",errno);
    exit(0);
  }

  printf("Client: told client there are %d blocks\n", num_blks);  
  if((read(sockid,(char *)msg,MAXLINEA)) < 0){
    printf("Client: ack read error :%d\n",errno);
    exit(0);
  }

  if(msg[0] == M_CONFIRMAR){
    mensaje_confirmacion = (CONFIRMAR *)msg;

    // Hasta aca llegue que es el momento donde me confirma la recepcion de los
    // datos del archivo.
    if(strcpy(mensaje_confirmacion->mensaje, "OK"))
      printf("El servidor ha validado los cantidad de bloques.\n");
  }

  sprintf(mensaje_solicitud->nombre, "%d", num_last_blk);

  if((write(sockid,(void *)mensaje_solicitud,sizeof(SOLICITUD))) < 0){
    printf("Client: write error :%d\n",errno);
    exit(0);
  }

  printf("Client: told client %d bytes in last block\n", num_last_blk);
  if((read(sockid,(char *)msg,MAXLINEA)) < 0){
    printf("Client: ack read error :%d\n",errno);
    exit(0);
  }

  if(msg[0] == M_CONFIRMAR){
    mensaje_confirmacion = (CONFIRMAR *)msg;

    // Hasta aca llegue que es el momento donde me confirma la recepcion de los
    // datos del archivo.
    if(strcpy(mensaje_confirmacion->mensaje, "OK"))
      printf("El servidor ha validado los cantidad de bytes del ultimo bloque.\n");
  }

  rewind(fp);

  // Se comienza la transferencia.
  mensaje_envio = (ENVIO *)malloc(sizeof(ENVIO));
  
  mensaje_envio->OP = M_ENVIAR;
  mensaje_envio->Tipo_Archivo = 'A';
  mensaje_envio->Longitud_Datos = MAX_DATOS;

  // printf("Size of mensaje_envio: %d\n", sizeof(ENVIO));

  for(i= 0; i < num_blks; i ++) { 
    no_read = fread(mensaje_envio->Datos,sizeof(char),MAX_DATOS,fp);
    if (no_read == 0){
      printf("Client: file read error\n");
      exit(0);
    }
    if (no_read != MAX_DATOS){
      printf("Client: file read error : no_read is less\n");
      exit(0);
    }
    cantidad_enviado = write(sockid,mensaje_envio,sizeof(ENVIO));
    // printf("FOR: Cantidad enviado: %d\n", cantidad_enviado);
    if(cantidad_enviado < 0){
      printf("Client: error sending block:%d\n",errno);
      exit(0);
    }
    cantidad_recibido = read(sockid,(char *)msg,MAXLINEA);
    // printf("FOR: Cantidad recibido: %d\n", cantidad_recibido);
    if(cantidad_recibido < 0){
      printf("Client: ack read  error :%d\n",errno);
      exit(0);
    }
    if(msg[0] == M_CONFIRMAR){
      mensaje_confirmacion = (CONFIRMAR *)msg;

      // Hasta aca llegue que es el momento donde me confirma la recepcion de los
      // datos del archivo.
      if(strcpy(mensaje_confirmacion->mensaje, "OK"))
        printf("El servidor ha validado el bloque enviado.\n");
    }
    else{
      printf("No se reconocio el mensaje de confirmacion.\n");
      exit(0);
    }
    printf(" %d...",i);
  }

  mensaje_envio->Longitud_Datos = num_last_blk;
  printf("Se comienza con la recepcion del ultimo bloque.\n");
  if(num_last_blk > 0) { 
    printf("%d\n",num_blks);
    no_read = fread(mensaje_envio->Datos,sizeof(char),MAX_DATOS,fp); 
    if(no_read == 0){
      printf("server: file read error\n");
      exit(0);
    }
    if (no_read != mensaje_envio->Longitud_Datos){
      printf("server: file read error : no_read is less 2\n");
      exit(0);
    }
    cantidad_enviado = write(sockid,mensaje_envio,sizeof(ENVIO));
    // printf("LAST: Cantidad enviado: %d\n", cantidad_enviado);
    if(cantidad_enviado < 0){
      printf("server: file transfer error %d\n",errno);
      exit(0);
    }
    cantidad_recibido = read(sockid,(char *)msg,MAXLINEA);
    // printf("LAST: Cantidad recibido: %d\n", cantidad_recibido);
    if(cantidad_recibido < 0){
      printf("Client: ack read  error :%d\n",errno);
      exit(0);
    }
    if(msg[0] == M_CONFIRMAR){
      mensaje_confirmacion = (CONFIRMAR *)msg;

      // Hasta aca llegue que es el momento donde me confirma la recepcion de los
      // datos del archivo.
      if(strcpy(mensaje_confirmacion->mensaje, "OK"))
        printf("El servidor ha validado el bloque enviado.\n");
    }
    else{
      printf("No se reconocio el mensaje de confirmacion.\n");
      exit(0);
    }
  }
  else
    printf("\n");
                                                  
  fclose(fp);

  printf("Transferencia completada!\n");

  return TRUE;
}


//   /* Once we are here, we've got a connection to the server */

//   /* tell server that we want to get a file */
//   getfile = htons(REQUESTFILE);
//   printf("client: sending command request to ftp server\n");
//   if((writen(sockid,(char *)&getfile,sizeof(getfile))) < 0){
//     printf("client: write  error :%d\n", errno);
//     exit(0);
//   } 

//   /* want for go-ahead from server */
//   msg = 0;  
//   if((readn(sockid,(char *)&msg,sizeof(msg)))< 0){
//     printf("client: read  error :%d\n", errno);
//     exit(0);
//   }
//   msg = ntohs(msg);   
//   if(msg==COMMANDNOTSUPPORTED){
//     printf("client: server refused command. goodbye\n");
//     exit(0);
//   }
//   else
//     printf("client: server replied %d, command supported\n",msg);

//   /* send file name to server */
//   printf("client: sending filename\n");
//   if((writen(sockid,path,len))< 0){
//     printf("client: write  error :%d\n", errno);
//     exit(0);
//   }
//   /* see if server replied that file name is OK */
//   msg_2 = 0;
//   if((readn(sockid,(char *)&msg_2,sizeof(msg_2)))< 0){
//     printf("client: read  error :%d\n", errno);
//     exit(0);
//   }   
//   msg_2 = ntohs(msg_2);
//   if(msg_2 == BADFILENAME) {
//     printf("client: server reported bad file name. goodbye.\n");
//     exit(0);
//   }
//   else
//     printf("client: server replied %d, filename OK\n",msg_2);


//   /* CLIENT KNOWS SERVER HAS BEEN ABLE TO OPEN THE FILE IN READ 
//      MODE AND IS ASKING FOR GO-AHEAD*/
//   /* CLIENT NOW OPENS A COPY OF THE FILE IN WRITE MODE AND SENDS 
//      THE GOAHEAD TO SERVER*/ 
//   printf("client: sending start transfer command\n");
//   start_xfer = STARTTRANSFER;
//   start_xfer = htons(start_xfer);
//   if((writen(sockid,(char *)&start_xfer,sizeof(start_xfer)))< 0){
//     printf("client: write  error :%d\n", errno);
//     exit(0);
//   }
//   if((fp = fopen(path,"w")) == NULL){
//     printf(" client: local open file error \n");
//     exit(0);
//   }


//   /*NOW THE CLIENT IS READING INFORMATION FROM THE SERVER REGARDING HOW MANY
//     FULL BLOCKS OF SIZE MAXSIZE IT CAN EXPECT. IT ALSO RECEIVES THE NUMBER
//    OF BYTES REMAINING IN THE LAST PARTIALLY FILLED BLOCK, IF ANY */  

//   if((readn(sockid,(char *)&num_blks,sizeof(num_blks))) < 0){
//     printf("client: read error on nblocks :%d\n",errno);
//     exit(0);
//   }
//   num_blks = ntohs(num_blks);
//   printf("client: server responded: %d blocks in file\n",num_blks);
//   ack = ACK;  
//   ack = htons(ack);
//   if((writen(sockid,(char *)&ack,sizeof(ack))) < 0){
//     printf("client: ack write error :%d\n",errno);
//     exit(0);
//   }

   
//   if((readn(sockid,(char *)&num_last_blk,sizeof(num_last_blk))) < 0){
//     printf("client: read error :%d on nbytes\n",errno);
//     exit(0);
//   }
//   num_last_blk = ntohs(num_last_blk);  
//   printf("client: server responded: %d bytes last blk\n",num_last_blk);
//   if((writen(sockid,(char *)&ack,sizeof(ack))) < 0){
//     printf("client: ack write error :%d\n",errno);
//     exit(0);
//   }


//   /* BEGIN READING BLOCKS BEING SENT BY SERVER */
//   printf("client: starting to get file contents\n");
//   for(i= 0; i < num_blks; i ++) {
//     if((readn(sockid,in_buf,MAXSIZE)) < 0){
//       printf("client: block error read: %d\n",errno);
//       exit(0);
//     }
//     no_writen = fwrite(in_buf,sizeof(char),MAXSIZE,fp);
//     if(no_writen == 0){
//       printf("client: file write error\n");
//       exit(0);
//     }
//     if(no_writen != MAXSIZE){
//       printf("client: file write  error : no_writen is less\n");
//       exit(0);
//     }
//     /* send an ACK for this block */
//     if((writen(sockid,(char *)&ack,sizeof(ack))) < 0){
//       printf("client: ack write  error :%d\n",errno);
//       exit(0);
//     }
//     printf(" %d...",i);
//   }


// /*IF THERE IS A LAST PARTIALLY FILLED BLOCK, READ IT */

//   if(num_last_blk > 0) {
//     printf("%d\n",num_blks);      
//     if((readn(sockid,in_buf,num_last_blk)) < 0){
//       printf("client: last block error read :%d\n",errno);
//       exit(0);
//     }
//     no_writen = fwrite(in_buf,sizeof(char),num_last_blk,fp); 
//     if(no_writen == 0){
//       printf("client: last block file write err :%d\n",errno);
//       exit(0);
//     }
//     if(no_writen != num_last_blk){
//       printf("client: file write error : no_writen is less 2\n");
//       exit(0);
//     }
//     if((writen(sockid,(char *)&ack,sizeof(ack))) < 0){
//       printf("client :ack write  error  :%d\n",errno);
//       exit(0);
//     }
//   }
//   else
//     printf("\n");
      

//   /*FILE TRANSFER ENDS. CLIENT TERMINATES AFTER  CLOSING ALL ITS FILES
//     AND SOCKETS*/ 
//   fclose(fp);
//   printf("client: FILE TRANSFER COMPLETE\n");
  // close(sockid);
// }      
  
/* DUE TO THE FACT THAT BUFFER LIMITS IN KERNEL FOR THE SOCKET MAY BE 
   REACHED, IT IS POSSIBLE THAT READ AND WRITE MAY RETURN A POSITIVE VALUE
   LESS THAN THE NUMBER REQUESTED. HENCE WE CALL THE TWO PROCEDURES
   BELOW TO TAKE CARE OF SUCH EXIGENCIES */

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
