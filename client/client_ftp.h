#include "../utils/definitions.h"

#define MAXSIZE 512

#define ACK                   2
#define NACK                  3
#define REQUESTFILE           100
#define COMMANDNOTSUPPORTED   150
#define COMMANDSUPPORTED      160
#define BADFILENAME           200
#define FILENAMEOK            400
#define STARTTRANSFER         500

int readn(int sd,char *,int);
int writen(int sd,char *,int);
int iniciar_cliente_ftp(int, int, char *);
BOOLEAN enviar_archivo(char *, int, int);