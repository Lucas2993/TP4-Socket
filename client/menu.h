
typedef struct item_menu{
	char * nombre;
	void* (*funcion)(int *);
}*ITEM_MENU, MENU_ITEM;

void imprimir_menu(ITEM_MENU, int);
int obtener_opcion(void);
void * realizar_operacion(ITEM_MENU, int, int, int*);