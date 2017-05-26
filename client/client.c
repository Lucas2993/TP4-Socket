#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <stdio.h>
#include <string.h>
#include "../utils/definitions.h"
#include "client.h"

int main(int argc, char ** argv) {
	int descriptor;
	struct sockaddr_in dir;

	/*---------------------------------------------------------------------*
	 * Verificar los argumentos
	 *---------------------------------------------------------------------*/
	if (argc < 3) {
		printf("Uso: cliente <direccion>\n");
		printf("     Donde: <direccion> = <ip> <puerto>\n");
		exit(-1);
	}

	/*--------------------------------------------------------------------* 
	 * Establecer la dirección del servidor y conectarse
	 *--------------------------------------------------------------------*/
	bzero((char *) &dir, sizeof(dir)); // Se blanquea toda la estructura
	dir.sin_family = AF_INET; // (Address Family)
	if (inet_pton( AF_INET, argv[1], &dir.sin_addr) <= 0) { // Se trasforma la ip de texto a tipo ip (binario).
		perror("Error en la función inet_pton:");
		exit(-1);
	}
	dir.sin_port = htons(atoi(argv[2])); // Se convierte el numero de puerto a un dato de la red.

	if ((descriptor = socket( AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("ERROR SOCKET:");
		exit(-1);
	}

	/*---------------------------------------------------------------------*
	 * Realizar la función del cliente
	 *---------------------------------------------------------------------*/
	principal( stdin, descriptor, (struct sockaddr *) &dir, sizeof(dir));
	return (0);
}

/*-------------------------------------------------------------------------*
 * principal()
 *-------------------------------------------------------------------------*/
int principal(FILE *fp, int sockfd, const struct sockaddr *dir, socklen_t sa) {
	int n;
	char linea_env[MAXLINEA], linea_rcb[MAXLINEA + 1];
	char nombre[] = "Lucas";

	SOLICITUD prueba;

	prueba.OP = '5';
	prueba.ID_Usuario = '1';
	prueba.ID_SUB_OP = '2';
	prueba.ID_Album = '0';
	prueba.ID_Archivo = '0';
	strcpy(prueba.nombre, nombre);

	int opcion;

	for (;;) {
	BEGIN:;
	system("clear");
	printf("Bienvenido a TuAlbum\n");
	printf("\n\n 1 - Iniciar Sesion.\n 2 - Registrarce.\n 0 - Salir.\n\n   ");
	scanf("%d", &opcion);
	printf("%d\n", opcion);

	INICIAR_SESION* iniciar_sesion;


		switch (opcion) {
		case 1:
			/*-------------------------------------------------------------------*/
			/* 							Iniciar Sesión  					   	 */
			/*-------------------------------------------------------------------*/

			iniciar_sesion = (INICIAR_SESION *) malloc(sizeof(INICIAR_SESION));
			iniciar_sesion->OP = M_INICIAR_SESION;

			system("clear");
			printf("Por favor, ingrese su nombre de usuario\n");
			scanf("%s", iniciar_sesion->usuario);

			system("clear");
			printf("Ingrese contraseña\n");
			scanf("%s", iniciar_sesion->clave);

			system("clear");
			printf("%d : %s -> %s\n", iniciar_sesion->OP,
					iniciar_sesion->usuario, iniciar_sesion->clave);

			sendto(sockfd, (void *) iniciar_sesion, sizeof(iniciar_sesion), 0,
					dir, sa);

			n = recvfrom(sockfd, linea_rcb, MAXLINEA, 0, NULL, NULL);
			linea_rcb[n] = '\0';
			printf("Recibido: %d\n", linea_rcb[0]);

			if (linea_rcb[0] == M_CONFIRMAR) {
				CONFIRMAR* confirmar = (CONFIRMAR *) linea_rcb;
				printf("confirmación : %s\n", confirmar->mensaje);
				int subopcion;
				for(;;){
					system("clear");
					printf("Bienvenido a TuAlbum %s\n",iniciar_sesion->usuario);
					printf("\n\n 1 - Listar Album.\n 2 - Ver Album.\n 3 - Crear Album.\n 4 - Modificar Album.\n 5 - Borrar Album.\n 0 - Salir.\n\n   ");
					scanf("%d",&subopcion);

					switch (subopcion) {
					case 0:
						goto BEGIN;
					case 1:
						//TODO: Listar album
						break;
					case 2:
						// TODO: Ver ALbum
						break;
					case 3:
						// TODO: Crear Album
						break;
					case 4:
						// TODO: Modificar Album
						break;
					case 5:
						// TODO: Borrar Album
						break;

					default:
						printf("\n Ingrese una opcion valida\n");
						break;

					}
				}
			} else {
				ERROR *error = (ERROR *) linea_rcb;
				printf("error : %s\n", error->mensaje);
				break;
			}
			break;
		case 2:
			/*-------------------------------------------------------------------*/
			/* 							Registrar        					   	 */
			/*-------------------------------------------------------------------*/
			printf("registrar\n");

			break;
		case 0:
			printf("salir\n");
			return FALSE;
		default:
			printf("\n Ingrese una opcion valida");
			break;
		}

		/* TODO Se necesita crear el servidor que reciba este mensaje y lo pueda leer.
		 * TODO Tambien se necesita crear un mensaje de respuesta que pueda ser interpretado de este lado.
		 */
//	sendto(sockfd, (void *) &prueba, sizeof(prueba), 0, dir, sa);
//
//	n = recvfrom(sockfd, linea_rcb, MAXLINEA, 0, NULL, NULL);
//	linea_rcb[n] = '\0';
//	printf("Recibido: %s\n", linea_rcb);
	}

	return 0;

}