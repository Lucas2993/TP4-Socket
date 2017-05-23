/*
 * server.h
 *
 *  Created on: 19 may. 2017
 *      Author: lucas
 */

#ifndef SERVER_SERVER_H_
#define SERVER_SERVER_H_

#include <string.h>
#include <stdlib.h>
#include "../utils/eco.h"
#include "../utils/definitions.h"

void procesar( int descriptor, struct sockaddr *dir_cli_p, socklen_t longcli );


#endif /* SERVER_SERVER_H_ */
