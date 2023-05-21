#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "list.h"
#include "Map.h"
#include "stack.h"
#include "heap.h"

typedef struct nodoTarea{
    char nombre[31];
    int prioridad;
    List* tareasPrevias; //Una lista que contendra todas las tareas predecesoras.
    int contPrecedentes; //La cantidad de tareas predecesoras que tiene.
    int contSucesoras; //La cantidad de tareas que tienena esta como predecesora.
    int estado; // 0 = no exploradas (fuera del monticulo) , 1 = explorada, 2 = completada.
}nodoTarea;

int is_equal_string(void * key1, void * key2) 
{
    if (strcmp((char*)key1, (char*)key2)==0) return 1;
    return 0;
}

int lower_than_string(void * key1, void * key2) {
    if (strcmp((char*)key1, (char*)key2) < 0) return 1;
    return 0;
}

nodoTarea* agregarTarea(List* lista){
    char nombreDeLaTarea[30];
    int prioridad;

    nodoTarea* tareaNueva = (nodoTarea*) malloc(sizeof(nodoTarea));
    tareaNueva->tareasPrevias = createList();

    printf("Ingrese el nombre de la tarea\n");
    fflush(stdin);
    scanf("%29s", nombreDeLaTarea);
    strcpy(tareaNueva->nombre, nombreDeLaTarea);
    printf("Ingrese la prioridad de la tarea\n");
    fflush(stdin);
    scanf("%d", &prioridad);
    tareaNueva->prioridad = prioridad;
    tareaNueva->estado = 0;
    tareaNueva->contPrecedentes = 0;
    tareaNueva->contSucesoras = 0;
    pushBack(lista, tareaNueva);
    printf("\n");

    return tareaNueva;
}

void establecerPreferencia(Map* mapaTareas) {
    char tarea1nombre[30];
    char tarea2nombre[30];
    nodoTarea* tarea1 = NULL;
    nodoTarea* tarea2 = NULL;

    printf("Ingrese la tarea que desea realizar primero\n");
    fflush(stdin);
    scanf("%29s", tarea1nombre);
    printf("Ingrese la tarea que quiere realizar despues\n");
    fflush(stdin);
    scanf("%29s", tarea2nombre);

    
    tarea1 = searchMap(mapaTareas, tarea1nombre);
    tarea2 = searchMap(mapaTareas, tarea2nombre);
    tarea1->contSucesoras++;
    tarea2->contPrecedentes++;

    if (tarea1 == NULL || tarea2 == NULL) {
        printf("No se encontro una o ambas tareas\n");
        return;
    }

    pushBack(tarea2->tareasPrevias, tarea1);

    printf("La tarea previa de %s es %s\n", tarea2->nombre, tarea1->nombre);
    printf("\n");
}

void tareasPorHacer(List* listaDeTodos, Map* mapaTareas, int contTareas){
    Heap* heapMinimo = createHeap();
    int contTareasAux = contTareas;
    int contTareasPrevAux;
    nodoTarea* tareaAux;
    nodoTarea* nodoAuxNext;
    List* listaVisitada = createList(); //Lista donde iran todas las tareas en el orden que deben ejecutarse.

    tareaAux = firstList(listaDeTodos);
    while(contTareasAux > 0){
        if(tareaAux->contPrecedentes == 0){
            tareaAux->estado = 1;
            heap_push(heapMinimo, tareaAux, (tareaAux->prioridad * - 1) );
        }
        if(contTareasAux > 1){
            tareaAux = nextList(listaDeTodos);
        }
        contTareasAux--;
    }

    contTareasAux = contTareas;

    while(contTareas > 0){
        tareaAux = heap_top(heapMinimo);
        tareaAux->estado = 2;
        pushBack(listaVisitada, tareaAux);
        heap_pop(heapMinimo);

        if(contTareasAux == contTareas){
            tareaAux = firstList(listaDeTodos);
        }else{
            if(contTareasAux > 1){
                tareaAux = nextList(listaDeTodos);
            }
        }

        contTareasPrevAux = tareaAux->contPrecedentes;

        if(tareaAux->estado == 0){
            nodoAuxNext = firstList(tareaAux->tareasPrevias);

            if(nodoAuxNext != NULL){
                    if(nodoAuxNext->estado == 2){
                    tareaAux->estado = 1;
                    heap_push(heapMinimo, tareaAux, (tareaAux->prioridad * - 1) );
                    }
                    contTareasPrevAux--;
            }
        }

        contTareas--;
    }

    tareaAux = firstList(listaVisitada);
    printf("Tareas en orden:\n");
    printf("%s\n", tareaAux->nombre);
    while(tareaAux != NULL){
        tareaAux = nextList(listaVisitada);
        if(tareaAux != NULL){
            printf("%s\n", tareaAux->nombre);
        }
    } 

}

void completarTarea(Map* mapaTareas){
    char nombreTarea[30];
    int opcion;
    nodoTarea* nodoAux;

    printf("Ingrese el nombre de la tarea que desea completar:\n");
    scanf("%29s", nombreTarea);
    fflush(stdin);

    nodoAux = searchMap(mapaTareas, nombreTarea);

    if (nodoAux == NULL) {
        printf("No se encontro la tarea que se deasea completar\n");
        return;
    }

    if(nodoAux->estado == 2){
        printf("Esta tarea ya se ha completado\n");
        return;
    }

    if(nodoAux->contPrecedentes != 0 || nodoAux->contSucesoras != 0){
        printf("Esta tarea tiene relaciones de precedencia. Esta seguro que desea eliminarla?\n");
        printf("1. Estoy seguro.\n");
        printf("2. Cancelar.\n");
        scanf("%d", &opcion);

        switch(opcion)
        {
        case 1: 
            nodoAux->estado = 2;
            printf("La tarea se ha marcado como completada\n");
            break;
        case 2:
            printf("Saliendo...\n");
            return;
        default:
            printf("Opcion invalida\n");
            sleep(1);
            break;
        }

    }else{
        nodoAux->estado = 2;
        printf("La tarea se ha marcado como completada\n");
    }

    free(nodoAux);
}


int main()
{
    int contTareas = 0;
    nodoTarea* tareaNueva = malloc(sizeof(nodoTarea*));
    List* lista = createList();
    Map* mapaTareas = createMap(is_equal_string);
    setSortFunction(mapaTareas, lower_than_string);

    int opcion = 0;

    do 
    {
        printf("Seleccione una opcion:\n"); 
        printf("1. Agregar tarea\n");
        printf("2. Establecer precedencia entre tareas\n");
        printf("3. Mostrar tareas por hacer\n");
        printf("4. Marcar tarea como completada\n");
        printf("0. Salir\n");

        scanf("%d", &opcion);

        switch (opcion)
        {
        case 0:
            printf("Saliendo...\n");
            break;
        case 1:
            tareaNueva = agregarTarea(lista);
            insertMap(mapaTareas, tareaNueva->nombre, tareaNueva);
            contTareas++;
            break;
        case 2:
            establecerPreferencia(mapaTareas);
            break;
        case 3:
        	tareasPorHacer(lista, mapaTareas, contTareas);
        	break;
        case 4:
            completarTarea(mapaTareas);
        	break;
        default:
            printf("Opcion invalida\n");
            sleep(1);
            break;
        }

    } while (opcion != 0);

    return EXIT_SUCCESS;
}