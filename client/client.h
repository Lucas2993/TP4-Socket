/*
 * client.h
 *
 *  Created on: 19 may. 2017
 *      Author: lucas
 */

#ifndef CLIENT_CLIENT_H_
#define CLIENT_CLIENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client.h"
#include "../utils/eco.h"
#include "../utils/definitions.h"


int principal( FILE *fp, int sockfd, const struct sockaddr *dir, socklen_t sa );


#endif /* CLIENT_CLIENT_H_ */
