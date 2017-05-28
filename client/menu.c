#include <stdio.h>
#include "menu.h"

void imprimir_menu(ITEM_MENU menu, int cantidad_items){
	int i;
	for(i = 0;i < cantidad_items;++menu,i++){
		printf("%d - %s\n",i+1, menu->nombre);
	}
}

int obtener_opcion(void){
	int opcion;
	printf("Opcion: ");
	scanf("%d", &opcion);

	return opcion;
}

void * realizar_operacion(ITEM_MENU menu, int cantidad_items, int opcion, int * longitud){
	void * respuesta = NULL;
	if(opcion-1 < cantidad_items){
		respuesta = (menu + opcion-1)->funcion(longitud);
	}
	return respuesta;
}