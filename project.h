//
// Created by jonib on 5/4/2023.
//
#include <malloc.h>
#include <string.h>
#include <stdio.h>

#ifndef PROJETO_PROJECT_H
#define PROJETO_PROJECT_H

enum tipo {
    Manutencao, Lavagem
};

typedef struct tipoReserva {
    enum tipo tipoR;
    int duracao; //60 para manutencao; 30 para lavagem
} tipoReserva;

typedef struct horarioReserva {
    int dia, mes, ano, hora, minutos;
} horarioReserva;

typedef struct Reservas {
    char cliente[50];
    tipoReserva tipo;
    horarioReserva hora;
    //Lista Ligada do tipo Pre_Reservas
} Reservas;

typedef struct NoListaPre_Reservas {
    Reservas reserva;
    struct NoListaPre_Reservas *next;
} NoListaPre_Reservas;

typedef struct ListaPre_Reservas {
    NoListaPre_Reservas *start;
    int size;
} ListaPre_Reservas;

typedef struct NoListaReservas {
    Reservas reserva;
    ListaPre_Reservas listaPreReservas;
    struct NoListaReservas *next;
} NoListaReservas;

typedef struct ListaReservas {
    NoListaReservas *start;
    int size;
} ListaReservas;

//funcoes
ListaReservas *create_lista_reservas();
ListaPre_Reservas *create_lista_pre_reservas();

void print_sorted(ListaReservas *lptr);
void menu_inicial();
int func_comp(const void *a, const void *b);

#endif //PROJETO_PROJECT_H
