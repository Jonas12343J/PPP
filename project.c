//
// Created by jonib on 5/4/2023.
//

#include "project.h"

int reserva_autoID = 0;
int pre_reserva_autoID = 0;

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

// Busca 1ª pré-reserva que cabe no novo slot disponivel pelo cancelamente da reserva
NoListaPre_Reservas *get_pre_reservation_node(ListaPre_Reservas *lista_pre, int time_to_next) {
    NoListaPre_Reservas *current = lista_pre->start;
    if (time_to_next >= 30 && time_to_next < 60) {
        while (current->reserva.tipo.tipoR != Lavagem) {
            current = current->next;
        }
    }
    return current;
}

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
int check_disponibilidade(ListaReservas *lista, int dia, int hora, int minuto, tipoReserva tipoRes, int clientID) {

    // Check input válido
    if (hora < 8 || ((hora * 60 + minuto) + tipoRes.duracao) > 18 * 60 || minuto < 0 || minuto > 59) {
        printf("Fora do horario de funcionamento!\n");
        return 0;
    }

    if (lista->size > 0) {
        NoListaReservas *current = lista->start;
        NoListaReservas *previous = NULL;

        for (int i = 0; i < lista->size; ++i) {
            if (current->reserva.hora.dia == dia) {
                // Se a hora que quero a reserva < (antes) do fim da atual (hora da atual + duracao dela) EEEEE se começar depois da atual
                // -------------------------------------OR----------------------------------------
                // Se hora que quero a reserva terminar > (depois) do início proximo ja reservado (com next != NULL!!!!!)

                //printf("%d < %d\n%d > %d\n", (hora * 60 + minuto), (current->reserva.hora.hora * 60 + current->reserva.hora.minutos + current->reserva.tipo.duracao),(hora * 60 + minuto), (current->reserva.hora.hora * 60 + current->reserva.hora.minutos));
                //printf("%d > %d\n", (hora * 60 + minuto + tipoRes.duracao), (current->next->reserva.hora.hora * 60 + current->next->reserva.hora.minutos));

                if ((previous != NULL && ((hora * 60 + minuto) <
                                          (previous->reserva.hora.hora * 60 + previous->reserva.hora.minutos +
                                           previous->reserva.tipo.duracao) && (hora * 60 + minuto) >
                                                                              (previous->reserva.hora.hora * 60 +
                                                                               previous->reserva.hora.minutos)))
                    || ((current != NULL) && (hora * 60 + minuto + tipoRes.duracao) >
                                             (current->reserva.hora.hora * 60 + current->reserva.hora.minutos))) {
                    printf("Horario ja ocupado! Deseja ficar em lista de pre-reserva? (y/n) ");
                    getchar();
                    char decisao[2];
                    scanf("%s", decisao);
                    if (strcmp(decisao, "y") == 0) {
                        // TODO function bellow
                        insert_pre_reserva(current->listaPreReservas, current, clientID, tipoRes, dia, hora, minuto);
                        return 3;

                    } else {
                        printf("Introduza uma nova hora\n");
                        return 2;
                    }
                }

            }
            previous = current;
            current = current->next;
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
            char tipoRstr[10];
            strcpy(tipoRstr, current->reserva.tipo.tipoR == Manutencao ? "Manutencao" : "Lavagem");

            if (current->reserva.hora.dia == dia && flag == 0) {

                flag = 1;
                printf("Existem as seguintes reservas para o dia %d:\n", dia);
                printf("\n%d:%d -> %s (%d minutos)\n", current->reserva.hora.hora, current->reserva.hora.minutos,
                       tipoRstr, current->reserva.tipo.duracao);
            } else if (current->reserva.hora.dia == dia && flag != 0) {
                printf("\n%d:%d -> %s (%d minutos)\n", current->reserva.hora.hora, current->reserva.hora.minutos,
                       tipoRstr, current->reserva.tipo.duracao);
            }
            current = current->next;
        }
        printf("\n");
        if (flag == 0) {
            printf("Sem reservas para esse dia!\n");
        }
    } else
        printf("Sem reservas para esse dia!\n");
}

// Mostra as reservas todas
int print_reservas(ListaReservas *lista) {
    if (lista->size > 0) {

        printf("-------------------------RESERVAS-------------------------\n");
        NoListaReservas *current = lista->start;

        for (int i = 0; i < lista->size; ++i) {
            char tipoRstr[10];
            strcpy(tipoRstr, current->reserva.tipo.tipoR == Manutencao ? "Manutencao" : "Lavagem");
            printf("\n  %d - Dia %d - %d:%d -> %s\n", current->reserva.ID, current->reserva.hora.dia,
                   current->reserva.hora.hora,
                   current->reserva.hora.minutos,
                   tipoRstr);
            current = current->next;
        }
        printf("----------------------------------------------------------\n\n");
        return 1;
    } else {
        printf("Sem reservas efetuadas!\n\n");
        usleep(500000);
        return 0;
    }
}

// Mostra as pré-reservas de um slot
void print_pre_reservas(ListaReservas *lista, int reservationID) {
    int found = 0;
    NoListaReservas *current = lista->start;
    while (current) {
        if (current->reserva.ID == reservationID) {
            found = 1;
            break;
        }
        current = current->next;
    }

    if (found) {
        if(current->listaPreReservas->size > 0) {
            NoListaPre_Reservas *curr_pre = current->listaPreReservas->start;
            printf("-------------------------PRE_RESERVAS-------------------------\n");
            for (int i = 0; i < lista->size; ++i) {
                char tipoRstr[10];
                strcpy(tipoRstr, curr_pre->reserva.tipo.tipoR == Manutencao ? "Manutencao" : "Lavagem");
                printf("\n  %d - Dia %d - %d:%d -> %s\n", curr_pre->reserva.ID, curr_pre->reserva.hora.dia,
                       curr_pre->reserva.hora.hora,
                       curr_pre->reserva.hora.minutos,
                       tipoRstr);
                curr_pre = curr_pre->next;
            }
            printf("----------------------------------------------------------\n\n");
            int pre_reservationID;
            scanf("%d", &pre_reservationID);
            cancela_pre_reserva(current->listaPreReservas, pre_reservationID);
            return;
        }
        else {
            printf("Sem pre-reservas para este bloco!\n\n");
            usleep(500000);
        }
    }
    else {
        printf("ID de reserva nao encontrado!\n\n");
    }
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

        if (!no_a_remover) {
            printf("ID de reserva nao encontrado\n");
            return;
        }

            // Encontrou a reserva a ser cancelada
        else {
            // Se tiver pré-reservas
            if (no_a_remover->listaPreReservas->start) {
                // Time
                int time_to_next =
                        ((no_a_remover->next->reserva.hora.hora * 60) + no_a_remover->next->reserva.hora.minutos) -
                        ((no_a_remover->reserva.hora.hora * 60) + no_a_remover->reserva.hora.minutos);
                // Tirar este elemento das pre reservas

                NoListaPre_Reservas *no_preR;
                no_preR = get_pre_reservation_node(no_a_remover->listaPreReservas, time_to_next);
                cancela_pre_reserva(current->listaPreReservas, no_preR->reserva.ID);
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

// Cancela uma pré-reserva
void cancela_pre_reserva(ListaPre_Reservas *lista_pre, int reservationID) {
    if (lista_pre->size > 0) {
        NoListaPre_Reservas *previous, *current, *no_a_remover;
        previous = no_a_remover = NULL;
        current = lista_pre->start;
        while (current) {
            if (current->reserva.ID == reservationID) {
                no_a_remover = current;
                break;
            }
            previous = current;
            current = current->next;
        }

        if (!no_a_remover) {
            printf("ID de pré-reserva nao encontrado\n");
            return;
        }

            // Encontrou a reserva a ser cancelada
        else {
            // Remover o 1º nó
            if (previous == NULL)
                lista_pre->start = no_a_remover->next;
                // Restantes nós
            else
                previous->next = no_a_remover->next;

            free(no_a_remover);
            lista_pre->size -= 1;

        }
    }
}

// Cancela uma pré-reserva dando o seu ID
void cancela_pre_reserva2(ListaReservas *lista, int pre_reservationID) {

}

// Funcão que vai verificar se uma reserva é antes ou depois (temporalmente) em relação a outra
// Returns: 0 para anterior // 1 para posterior
int compare_reservas_time(Reserva res1, Reserva res2) {
    if (res1.hora.dia < res2.hora.dia) {
        return 0;
    } else if (res1.hora.dia > res2.hora.dia) {
        return 1;
    } else {  //Mesmo dia
        if (res1.hora.hora < res2.hora.hora) {
            return 0;
        } else if (res1.hora.hora > res2.hora.hora) {
            return 1;
        } else { // Mesma hora
            if (res1.hora.minutos < res2.hora.minutos) {
                return 0;
            } else {
                return 1;
            }
        }
    }
}

// Criar uma reserva
void insert_reserva(ListaReservas *lista, int clientId, tipoReserva tipoRes, int dia, int hora, int minuto) {
    NoListaReservas *novo_no = malloc(sizeof(NoListaReservas));
    if (novo_no) {
        novo_no->reserva.ID = reserva_autoID;
        novo_no->reserva.clientID = clientId;
        novo_no->reserva.tipo = tipoRes;
        novo_no->reserva.hora.dia = dia;
        novo_no->reserva.hora.hora = hora;
        novo_no->reserva.hora.minutos = minuto;
        novo_no->listaPreReservas = create_lista_pre_reservas();
        novo_no->next = NULL;

        // Caso a lista esteja vazia ou a nova reserva seja anterior à primeira reserva
        if (lista->start == NULL || !compare_reservas_time(novo_no->reserva, lista->start->reserva)) {
            novo_no->next = lista->start;
            lista->start = novo_no;
        } else {
            NoListaReservas *current = lista->start;
            // while: Existir um next e esse next for (temporalmente) depois do nosso novo_no
            while (current->next != NULL && compare_reservas_time(current->next->reserva, novo_no->reserva)) {
                current = current->next;
            }
            novo_no->next = current->next;
            current->next = novo_no;
        }

        lista->size += 1;
    }
    /*
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
        // restantes casos
    else {
        novo_no->next = current;
        previous->next = novo_no;
    }
}
lista->size += 1;
     */
}

// Cria uma pré-reserva
void
insert_pre_reserva(ListaPre_Reservas *lista_pre, NoListaReservas *current, int clientId, tipoReserva tipoRes, int dia,
                   int hora, int minuto) {
    NoListaPre_Reservas *novo_no = malloc(sizeof(NoListaPre_Reservas));
    if (novo_no) {
        novo_no->reserva.clientID = clientId;
        novo_no->reserva.tipo = tipoRes;
        novo_no->reserva.hora.dia = dia;
        novo_no->reserva.hora.hora = hora;
        novo_no->reserva.hora.minutos = minuto;

        NoListaPre_Reservas *current_pre, *previous_pre;
        previous_pre = NULL;
        current_pre = current->listaPreReservas->start;
        while (current_pre) {
            previous_pre = current_pre;
            current_pre = current_pre->next;
        }
        // se a inserçao for como 1º nó
        if (previous_pre == NULL) {
            novo_no->next = lista_pre->start;
            lista_pre->start = novo_no;
        }
            // restantes casos
        else {
            novo_no->next = current_pre;
            previous_pre->next = novo_no;
        }
    }
    ++lista_pre->size;
    ++pre_reserva_autoID;
}


// Menu de opções
void menu_inicial() {
    printf("-------------------------MENU-------------------------\n");
    printf("\t1 - Efetuar reserva\n");
    printf("\t2 - Cancelar reserva\n");
    printf("\t3 - Cancelar pre-reserva\n");
    printf("\t4 - Listar todas reservas\n");
    printf("\t5 - Listar reservas de cliente\n");
    printf("\t6 - Salvar reservas\n");
    printf("\t7 - Carregar reservas\n");
    printf("\t0 - QUIT\n");
    printf("------------------------------------------------------\n\n  -> ");
}
