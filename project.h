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
    int dia, hora, minutos;
} horarioReserva;


typedef struct Reserva {
    int clientID;
    tipoReserva tipo;
    horarioReserva hora;
    //Lista Ligada do tipo Pre_Reservas
} Reserva;

typedef struct NoListaPre_Reservas {
    Reserva reserva;
    struct NoListaPre_Reservas *next;
} NoListaPre_Reservas;

typedef struct ListaPre_Reservas {
    NoListaPre_Reservas *start;
    int size;
} ListaPre_Reservas;

typedef struct NoListaReservas {
    Reserva reserva;
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
Reserva create_reserva(int clientId, tipoReserva tipoR, horarioReserva horas);

void print_sorted(ListaReservas *lptr);
void menu_inicial();
void print_reservas_dia(ListaReservas *lista, int dia);

int func_comp(const void *a, const void *b);
int check_disponibilidade(ListaReservas *lista, int dia, int hora, int minuto, int duracao);


#endif //PROJETO_PROJECT_H
