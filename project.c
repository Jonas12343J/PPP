//
// Created by jonib on 5/4/2023.
//

#include "project.h"

ListaPre_Reservas *create_lista_pre_reservas() {
    ListaPre_Reservas *lista = malloc(sizeof(ListaPre_Reservas));
    if (lista) {
        lista->start = NULL;
        lista->size = 0;
    }
    return lista;
}

ListaReservas *create_lista_reservas() {
    ListaReservas *lista = malloc(sizeof(ListaReservas));
    if (lista) {
        lista->start = NULL;
        lista->size = 0;
    }
    return lista;
}

//no meu insert: lista->start->listaPreReservas = create_lista_pre_reservas();

int func_comp(const void *a, const void *b) {
    NoListaReservas *v1 = *((NoListaReservas **) a);
    NoListaReservas *v2 = *((NoListaReservas **) b);
    //return *v1 - *v2;
    //TODO nao é por clientes
    return strcmp(v1->reserva.cliente, v2->reserva.cliente);
}

//Array de pointers para dar print ordenado
void print_sorted(ListaReservas *lista) {
    if (lista->size > 0) {
        NoListaReservas **ptr = malloc(sizeof(NoListaReservas *) * lista->size);
        NoListaReservas *current = lista->start;

        for (int i = 0; i < lista->size; ++i) {
            ptr[i] = current;
            current = current->next;
        }
        qsort(ptr, lista->size, sizeof(NoListaReservas *), func_comp);
        for (int i = 0; i < lista->size; ++i)
            //TODO nao é por clientes
            printf("[%s: %s]", ptr[i]->reserva.cliente, ptr[i]->reserva.cliente);

        printf("\n");
        free(ptr);
    }
}
