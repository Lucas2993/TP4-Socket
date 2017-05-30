#
# Para compilar, desde línea de comandos:
# 	make -f socket.make
#
# Tener en cuenta:
# 	- las etiquetas (nombre seguido de :) indican el nombre de lo que se quiere construir (ej. el ejecutable).
#	- si se repiten persiste la última aparición (y muestra un warning)
#	- el espacio en blanco antes de cada comando es un tabulador
#	- puede utilizarse cualquier comando del shell
#	- las variables deben definirse e inicializarse al comienzo
#	- toma un sólo objetivo (la primer etiqueta)(ejemplo-cli-ser-eco)
#	- en la línea de la etiqueta van los archivos objeto que deben linkearse para obtener el ejecutable
#	- luego habrá una etiqueta por cada objeto, donde se indica:
#		- las dependencias de archivos (en la misma línea a continuación de la etiqueta)
#		- los comandos necesarios para compilar
#

# ubicación servidores
VIA_SRV = server

OBJS_SRV = $(VIA_SRV)/server.o $(VIA_SRV)/usuario.o $(VIA_SRV)/sesion.o $(VIA_SRV)/album.o

# ubicación clientes
VIA_CLI = client

# ubicacion utilidades
VIA_UTIL = utils

OBJS_CLI = $(VIA_CLI)/client.o $(VIA_CLI)/menu.o

# paquete completo de clientes y servidores
socket: $(OBJS_SRV) $(OBJS_CLI)
	cc $(OBJS_SRV) -o $(VIA_SRV)/server
	cc $(OBJS_CLI) -o $(VIA_CLI)/client
	
# 1.- Servidor de eco iterativo sobre UDP
server.o:	$(VIA_SRV)/server.c $(VIA_SRV)/server.h $(VIA_UTIL)/definitions.h $(VIA_SRV)/usuario.h
	cc -c $(VIA_SRV)/server.c

# 1.- Cliente de eco iterativo sobre UDP
client.o: 	$(VIA_CLI)/client.c $(VIA_CLI)/client.h $(VIA_UTIL)/definitions.h $(VIA_CLI)/menu.h
	cc -c $(VIA_CLI)/client.c

menu.o:	$(VIA_CLI)/menu.c $(VIA_CLI)/menu.h 
	cc -c $(VIA_CLI)/menu.c

usuario.o:	$(VIA_SRV)/usuario.c $(VIA_SRV)/usuario.h $(VIA_UTIL)/definitions.h
	cc -c $(VIA_SRV)/usuario.c

sesion.o:	$(VIA_SRV)/sesion.c $(VIA_SRV)/sesion.h $(VIA_UTIL)/definitions.h
	cc -c $(VIA_SRV)/sesion.c

album.o:	$(VIA_SRV)/album.c $(VIA_SRV)/album.h $(VIA_UTIL)/definitions.h
	cc -c $(VIA_SRV)/album.c