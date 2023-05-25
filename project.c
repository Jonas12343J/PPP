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
// 0 -> invalid // 1 -> vaga // 2 -> ocupado // 3 -> ficou em pré reserva
int check_disponibilidade(ListaReservas *lista, int dia, int hora, int minuto, int duracao, tipoReserva tipoRes, int clientID) {
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
                        // TODO MALLOC (COMO NO INSERT)
                        current->listaPreReservas.end->reserva.hora.dia = dia;
                        current->listaPreReservas.end->reserva.hora.hora = hora;
                        current->listaPreReservas.end->reserva.hora.minutos = minuto;
                        current->listaPreReservas.end->reserva.tipo = tipoRes;
                        current->listaPreReservas.end->reserva.ID = pre_reserva_autoID;
                        current->listaPreReservas.end->reserva.clientID = clientID;
                        ++pre_reserva_autoID;
                        return 3;
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

// Mostra as reservas todas
void print_reservas(ListaReservas *lista) {
    if (lista->size > 0) {

        printf("-------------------------RESERVAS-------------------------\n");
        NoListaReservas *current = lista->start;

        for (int i = 0; i < lista->size; ++i) {
            printf("\n  %d - Dia %d - %d:%d -> %u\n",current->reserva.ID, current->reserva.hora.dia, current->reserva.hora.hora,
                   current->reserva.hora.minutos,
                   current->reserva.tipo.tipoR);
            current = current->next;
        }
    }
    printf("----------------------------------------------------------\n\n");
}

// Cancela uma reserva
void cancela_reserva(ListaReservas *lista, int reservationID) {
    if (lista->size > 0) {
        NoListaReservas *previous, *current, *no_a_remover;
        previous = no_a_remover = NULL;
        current = lista->start;
        while (current) {
            if (current->reserva.ID == reservationID) {
                no_a_remover = current;
                break;
            }
            previous = current;
            current = current->next;
        }

        if(!no_a_remover){
            printf("ID de reserva não encontrado\n");
            return;
        }

        // Encontrou a reserva a ser cancelada
        else {
            // Se tiver pré-reservas
            if(no_a_remover->listaPreReservas.start) {
                no_a_remover = (NoListaReservas *) no_a_remover->listaPreReservas.start;
                // TODO tirar este elemento das pre reservas
            }
            // Se não tiver pré-reservas
            else {
                // Remover o 1º nó
                if (previous == NULL)
                    lista->start = no_a_remover->next;
                    // Restantes nós
                else
                    previous->next = no_a_remover->next;

                free(no_a_remover);
                lista->size -= 1;
            }
        }
    }
}

// Criar uma reserva
void insert_reserva(ListaReservas *lista, int clientId, tipoReserva tipoRes, int dia, int hora, int minuto) {
    NoListaReservas *novo_no = malloc(sizeof(NoListaReservas));
    if (novo_no) {
        novo_no->reserva.clientID = clientId;
        novo_no->reserva.tipo = tipoRes;
        novo_no->reserva.hora.dia = dia;
        novo_no->reserva.hora.hora = hora;
        novo_no->reserva.hora.minutos = minuto;
        // TODO create lista pre reservas
        novo_no->listaPreReservas =create_lista_pre_reservas();

        NoListaReservas *previous, *current;
        previous = NULL;
        current = lista->start;
        while (current) {
            previous = current;
            current = current->next;
        }
        // se a inserçao for como 1º nó
        if (previous == NULL) {
            novo_no->next = lista->start;
            lista->start = novo_no;
        }
            //restantes casos
        else {
            novo_no->next = current;
            previous->next = novo_no;
        }
    }
    lista->size += 1;
}




// Menu de opções
void menu_inicial() {
    printf("-------------------------MENU-------------------------\n");
    printf("\t1 - Efetuar reserva\n");
    printf("\t2 - Cancelar reserva\n");
    printf("\t2 - Cancelar pré-reserva\n");
    printf("\t3 - Listar todas reservas\n");
    printf("\t4 - Listar reservas de cliente\n");
    printf("\t5 - Salvar reservas\n");
    printf("\t6 - Carregar reservas\n");
    printf("\t0 - QUIT\n");
    printf("------------------------------------------------------\n\n\t->");
}
