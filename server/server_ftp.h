#include "../utils/definitions.h"

#define MY_PORT_ID 6081
#define MAXLINE 256
#define MAXSIZE 512   

#define ACK                   2
#define NACK                  3
#define REQUESTFILE           100
#define COMMANDNOTSUPPORTED   150
#define COMMANDSUPPORTED      160
#define BADFILENAME           200
#define FILENAMEOK            400

int writen(int sd,char *ptr,int size);
int readn(int sd,char *ptr,int size);
BOOLEAN iniciar_servidor_ftp(int);
void doftp(int);