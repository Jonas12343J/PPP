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
    //return strcmp(v1->reserva.client.name, v2->reserva.client.name);
}

// Verifica se tem slot disponivel na hora pretendida
// Se sim, marca, se não pergunta se quer pré-reserva ou novo horário
// 0 -> invalid // 1 -> vaga // 2 -> ocupado
int check_disponibilidade(ListaReservas *lista, int dia, int hora, int minuto, int duracao) {
    char decisao;

    // Check input válido
    if (hora < 8 || ((hora * 60 + minuto) + duracao) > 18 * 60) {
        printf("Fora do horario de funcionamento!\n");
        return 0;
    }

    if (lista->size > 0) {
        NoListaReservas *current = lista->start;

        for (int i = 0; i < lista->size; ++i) {
            if (current->reserva.hora.dia == dia) {
                // Se hora que quero a reserva < (antes) do fim da atual (hora da atual + duracao dela)
                // -------------------------------------OR----------------------------------------
                // Se hora que quero a reserva terminar > (depois) do início proximo ja reservado
                if ((hora * 60 + minuto) <
                    (current->reserva.hora.hora * 60 + current->reserva.hora.minutos + current->reserva.tipo.duracao)
                    || (hora * 60 + minuto + duracao) >
                       (current->next->reserva.hora.hora * 60 + current->next->reserva.hora.minutos)) {

                    printf("Horario já ocupado! Deseja ficar em lista de pré-reserva? (y/n)");
                    scanf("%c", &decisao);
                    if (decisao == 'y') {
                        // INSERT NA LISTA DE PRE RESERVA
                    } else {
                        printf("Introduza uma nova hora\n");
                        return 2;
                    }
                }
            }
        }
    }
    return 1;
}

// Array de pointers para dar print ordenado
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
            //printf("[%s: %s]", ptr[i]->reserva.client.name, ptr[i]->reserva.client.name);

            printf("\n");
        free(ptr);
    }
}

// Mostra as reservas do dia desejado
void print_reservas_dia(ListaReservas *lista, int dia) {
    if (lista->size > 0) {
        // Verifica se existem reservas nesse dia
        int flag = 0;
        NoListaReservas *current = lista->start;

        for (int i = 0; i < lista->size; ++i) {
            if (current->reserva.hora.dia == dia && flag == 0) {
                flag = 1;
                printf("Existem as seguintes reservas para o dia %d:\n", dia);
                printf("\n%d:%d -> %u (%d minutos)\n", current->reserva.hora.hora, current->reserva.hora.minutos,
                       current->reserva.tipo.tipoR, current->reserva.tipo.duracao);
            } else if (current->reserva.hora.dia == dia && flag != 0) {
                printf("\n%d:%d -> %u (%d minutos)\n", current->reserva.hora.hora, current->reserva.hora.minutos,
                       current->reserva.tipo.tipoR, current->reserva.tipo.duracao);
            }
            current = current->next;
        }
        if (flag == 0) {
            printf("Não existem reservas para este dia\n");
        }
    }
}

// Menu de opções
void menu_inicial() {
    printf("-------------------------MENU-------------------------\n");
    printf("\t1 - Efetuar reserva\n");
    printf("\t2 - Cancelar reserva\n");
    printf("\t3 - Listar todas reservas\n");
    printf("\t4 - Listar reservas de cliente\n");
    printf("\t5 - Salvar reservas\n");
    printf("\t6 - Carregar reservas\n");
    printf("\t0 - QUIT\n");
    printf("------------------------------------------------------\n\n\t->");
}