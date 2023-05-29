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
void insert_reserva(ListaReservas *lista, int clientID, tipoReserva tipoRes, int dia, int hora, int minuto) {

    // Check input válido
    if (hora < 8 || ((hora * 60 + minuto) + tipoRes.duracao) > 18 * 60 || minuto < 0 || minuto > 59) {
        printf("Fora do horario de funcionamento!\n");
        usleep(500000);
        return;
    }

    NoListaReservas *novo_no = malloc(sizeof(NoListaReservas));
    novo_no->reserva.ID = reserva_autoID;
    novo_no->reserva.clientID = clientID;
    novo_no->reserva.tipo = tipoRes;
    novo_no->reserva.hora.dia = dia;
    novo_no->reserva.hora.hora = hora;
    novo_no->reserva.hora.minutos = minuto;
    novo_no->listaPreReservas = create_lista_pre_reservas();
    novo_no->next = NULL;

    if(lista->start == NULL) {
        lista->start = novo_no;
        ++lista->size;
        ++reserva_autoID;
        return;
    }

    NoListaReservas *current = lista->start;
    NoListaReservas *previous = NULL;

    int new_start = (hora * 60 + minuto);
    int new_end = (hora * 60 + minuto + tipoRes.duracao);

    // Se a nova reserva acabar depois da atual começar, vai ao next
    // Avança até encontrar um que comece depois da nova acabar OU NO FIM DA LISTA
    while (current != NULL && (new_end > (current->reserva.hora.hora * 60 + current->reserva.hora.minutos))) {
        previous = current;
        current = current->next;
    }

    if (previous == NULL) {
        // Vaga no inicio -> lista tava vazia
        novo_no->next = lista->start;
        lista->start = novo_no;
        ++lista->size;
        ++reserva_autoID;
    } else if (current != NULL) {
        int previous_end = (previous->reserva.hora.hora * 60 + previous->reserva.hora.minutos +
                            previous->reserva.tipo.duracao);
        int current_start = (current->reserva.hora.hora * 60 + current->reserva.hora.minutos);

        // Se a reserva anterior terminar antes da nova começar AND e a nova acabar antes da atual começar -> SUCESSO
        if ((previous_end <= new_start) && (new_end <= current_start)) {
            previous->next = novo_no;
            novo_no->next = current;
            ++lista->size;
            ++reserva_autoID;
        }
        // Se a reserva anterior acabar depois da nova começar OR a nova acabar depois da atual começar
        else if ((previous_end > new_start) || (new_end > current_start)) {
            printf("Horario ja ocupado! Pedido movido para pre-reserva\n");
            insert_pre_reserva(current->listaPreReservas, current, clientID, tipoRes, dia, hora, minuto);
        }
    } else {
        int previous_end = (previous->reserva.hora.hora * 60 + previous->reserva.hora.minutos +
                            previous->reserva.tipo.duracao);
        if(previous_end > new_start) {
            printf("Horario ja ocupado! Pedido movido para pre-reserva\n");
            insert_pre_reserva(previous->listaPreReservas, previous, clientID, tipoRes, dia, hora, minuto);
        } else {
            previous->next = novo_no;
            novo_no->next = current;
            ++lista->size;
            ++reserva_autoID;
        }
    }

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

        while(current) {
            char tipoRstr[15];
            strcpy(tipoRstr, current->reserva.tipo.tipoR == Manutencao ? "Manutencao" : "Lavagem");

            if (current->reserva.hora.dia == dia && flag == 0) {
                flag = 1;
                printf("Existem as seguintes reservas para o dia %d:\n", dia);
                printf("\n%d:%d -> %s (%d minutos)\n", current->reserva.hora.hora, current->reserva.hora.minutos,
                       tipoRstr, current->reserva.tipo.duracao);
            } else if (current->reserva.hora.dia == dia && flag != 0) {
                printf("%d:%d -> %s (%d minutos)\n", current->reserva.hora.hora, current->reserva.hora.minutos,
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
        printf("\n---------------------------RESERVAS-----------------------\n");
        NoListaReservas *current = lista->start;

        while(current) {
            char tipoRstr[15];
            strcpy(tipoRstr, current->reserva.tipo.tipoR == Manutencao ? "Manutencao" : "Lavagem");
            printf("\t%d - Dia %d - %d:%d -> %s\n", current->reserva.ID, current->reserva.hora.dia,
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
        if (current->listaPreReservas->size > 0) {
            NoListaPre_Reservas *curr_pre = current->listaPreReservas->start;
            printf("\n-------------------------PRE_RESERVAS-------------------------\n");
            while (curr_pre) {
                char tipoRstr[15];
                strcpy(tipoRstr, curr_pre->reserva.tipo.tipoR == Manutencao ? "Manutencao" : "Lavagem");
                printf("\t%d - Dia %d - %d:%d -> %s\n", curr_pre->reserva.ID, curr_pre->reserva.hora.dia,
                       curr_pre->reserva.hora.hora, curr_pre->reserva.hora.minutos, tipoRstr);
                curr_pre = curr_pre->next;
            }
            printf("----------------------------------------------------------\n\n");
            printf("Que pre-reserva pretende cancelar?\n");

            int pre_reservationID;
            scanf("%d", &pre_reservationID);
            cancela_pre_reserva(current->listaPreReservas, pre_reservationID);
            return;
        } else {
            printf("Sem pre-reservas para este bloco!\n\n");
            usleep(500000);
        }
    } else {
        printf("ID de reserva nao encontrado!\n\n");
    }
}

// Cancela uma reserva
//TODO SEGMENTATION FAULT BUG NO CANCELA
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
            if (no_a_remover->listaPreReservas->start != NULL) {
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
                --lista->size;
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
// Returns: 0 para anterior // 1 para posterior // 2 para igual
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
            } else if (res1.hora.minutos > res2.hora.minutos) {
                return 1;
            } else // Mesmo minuto -> IGUAL
                return 2;
        }
    }
}

/*
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
            while (current->next != NULL && compare_reservas_time(novo_no->reserva, current->next->reserva)) {
                current = current->next;
            }
            novo_no->next = current->next;
            current->next = novo_no;
        }
        lista->size += 1;
    }

}
*/

// Cria uma pré-reserva
void insert_pre_reserva(ListaPre_Reservas *lista_pre, NoListaReservas *current, int clientId, tipoReserva tipoRes, int dia,
                   int hora, int minuto) {
    NoListaPre_Reservas *novo_no = malloc(sizeof(NoListaPre_Reservas));
    if (novo_no) {
        novo_no->reserva.clientID = clientId;
        novo_no->reserva.tipo = tipoRes;
        novo_no->reserva.hora.dia = dia;
        novo_no->reserva.hora.hora = hora;
        novo_no->reserva.hora.minutos = minuto;
        novo_no->reserva.ID = pre_reserva_autoID;

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
        ++lista_pre->size;
        ++pre_reserva_autoID;
    }
}

// Lista todas as reservas (mais antigas primeiro)
void list_all(ListaReservas *lista) {
    if(lista->size > 0) {
        NoListaReservas *current = lista->start;
        printf("\n---------------------------RESERVAS-----------------------\n");
        while (current) {
            char tipoRstr[15];
            strcpy(tipoRstr, current->reserva.tipo.tipoR == Manutencao ? "Manutencao" : "Lavagem");
            printf("\tDia %d - %d:%d -> %s\n", current->reserva.hora.dia,
                   current->reserva.hora.hora, current->reserva.hora.minutos, tipoRstr);
            current = current->next;
        }
        printf("----------------------------------------------------------\n\n");
        usleep(500000);
    }
    else {
        printf("Sem reservas registadas!\n");
    }
}

// Menu de opções
void menu_inicial() {
    printf("\n-------------------------MENU-------------------------\n");
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
