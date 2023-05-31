//
// Created by jonib on 5/4/2023.
//

#include "project.h"

int reserva_autoID = 1;
int pre_reserva_autoID = 1;
char out[256];

// Cria a lista de reservas
ListaReservas *create_lista_reservas() {
    ListaReservas *lista = malloc(sizeof(ListaReservas));
    if (lista) {
        lista->start = NULL;
        lista->size = 0;
    }
    return lista;
}

// Cria a lista de pré-reservas de um bloco específico
ListaPre_Reservas *create_lista_pre_reservas() {
    ListaPre_Reservas *lista = malloc(sizeof(ListaPre_Reservas));
    if (lista) {
        lista->start = NULL;
        lista->size = 0;
    }
    return lista;
}

// Carrega os dados do ficheiro binário para as listas
NoListaReservas *loadLinkedListFromFile(int *mainListSize) {
    FILE *file = fopen("bin_file.bin", "rb");
    if (file == NULL) {
        printf("Failed to open the file for reading.\n");
        return NULL;
    }

    int size = 0;
    NoListaReservas *head = NULL;
    NoListaReservas *current = NULL;

    // Read the data from the file and create nodes for each value
    Reserva mainData;
    int hasAuxList;
    int auxListSize;
    while (fread(&mainData, sizeof(Reserva), 1, file) == 1) {
        NoListaReservas *newNode = (NoListaReservas *) malloc(sizeof(NoListaReservas));
        newNode->reserva = mainData;

        newNode->next = NULL;
        newNode->listaPreReservas = NULL;

        fread(&hasAuxList, sizeof(int), 1, file);
        if (hasAuxList) {
            auxListSize = 0;
            NoListaPre_Reservas *auxHead = NULL;
            NoListaPre_Reservas *auxCurrent = NULL;

            Reserva auxData;
            while (fread(&auxData, sizeof(Reserva), 1, file) == 1) {
                NoListaPre_Reservas *newAuxNode = (NoListaPre_Reservas *) malloc(sizeof(NoListaPre_Reservas));
                newAuxNode->reserva = auxData;
                newAuxNode->next = NULL;

                if (auxHead == NULL) {
                    auxHead = newAuxNode;
                    auxCurrent = auxHead;
                } else {
                    auxCurrent->next = newAuxNode;
                    auxCurrent = auxCurrent->next;
                }
                ++auxListSize;
            }

            newNode->listaPreReservas = (ListaPre_Reservas *) malloc(sizeof(ListaPre_Reservas));
            newNode->listaPreReservas->start = auxHead;
            newNode->listaPreReservas->size = auxListSize;
        }

        if (head == NULL) {
            head = newNode;
            current = head;
        } else {
            current->next = newNode;
            current = current->next;
        }
        ++size;
    }

    *mainListSize = size;


    fclose(file);
    //------------UPDATE-LOG---------------
    update_log("DATA RESTORED FROM BINARY FILE!\n");
    return head;
}

// Busca 1ª pré-reserva que cabe no novo slot disponivel pelo cancelamente da reserva
NoListaPre_Reservas *get_pre_reservation_node(ListaPre_Reservas *lista_pre, int time_to_next, int next_start) {
    NoListaPre_Reservas *current = lista_pre->start;

    while (current) {
        int pre_start = (current->reserva.hora.hora * 60) + current->reserva.hora.minutos;
        int duracao = current->reserva.tipo.duracao;
        if (next_start - pre_start <= time_to_next && next_start - pre_start >= duracao) {
            printf("encontrou\n");
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Cria novo nó para a árvore
Client *createNode(int clientID) {
    Client *newNode = (Client *) malloc(sizeof(Client));
    newNode->clientID = clientID;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

// Insere um cliente novo na arvore
Client *insert(Client *root, int clientID) {
    if (root == NULL) {
        return createNode(clientID);
    }

    if (clientID < root->clientID) {
        root->left = insert(root->left, clientID);
    } else if (clientID > root->clientID) {
        root->right = insert(root->right, clientID);
    }
    return root;
}

// Carrega os dados do ficheiro binário para a árvore de clientes
Client* loadClientTreeFromFile(FILE* file) {
    if (file == NULL) {
        return NULL;
    }

    Client* root = NULL;

    // Read the data from the file until the end is reached
    while (!feof(file)) {
        // Create a new client node
        Client* client = (Client*)malloc(sizeof(Client));

        // Read the client data from the file
        if (fread(client, sizeof(Client), 1, file) != 1) {
            free(client);
            break;
        }

        // Insert the client into the tree
        root = insert(root, client->clientID);
    }
    return root;
}

// Mostra as reservas todas
int print_reservas(ListaReservas *lista, bool flag) {
    if (lista->size > 0) {
        int found = 0;
        printf("\n---------------------------RESERVAS-----------------------\n");
        NoListaReservas *current = lista->start;

        while (current) {
            if (flag == true) {
                char tipoRstr[15];
                strcpy(tipoRstr, current->reserva.tipo.tipoR == Manutencao ? "Manutencao" : "Lavagem");
                printf("\t%d - Dia %d - %02d:%02d -> %s\n", current->reserva.ID, current->reserva.hora.dia,
                       current->reserva.hora.hora,
                       current->reserva.hora.minutos,
                       tipoRstr);
            } else if (flag == false) {
                if (current->listaPreReservas->start) {
                    found = 1;
                    char tipoRstr[15];
                    strcpy(tipoRstr, current->reserva.tipo.tipoR == Manutencao ? "Manutencao" : "Lavagem");
                    printf("\t%d - Dia %d - %02d:%02d -> %s\n", current->reserva.ID, current->reserva.hora.dia,
                           current->reserva.hora.hora,
                           current->reserva.hora.minutos,
                           tipoRstr);
                }
            }
            current = current->next;
        }
        printf("----------------------------------------------------------\n\n");
        if (!found && flag == false) {
            printf("Sem blocos com pre-reservas associados!\n\n");
            return 0;
        }
        return 1;
    } else {
        printf("Sem reservas efetuadas!\n\n");
        usleep(500000);
        return 0;
    }
}


// Mostra as pré-reservas de um slot
int print_pre_reservas(ListaReservas *lista, int reservationID) {
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
                printf("\t%d - Dia %d - %02d:%02d -> %s\n", curr_pre->reserva.ID, curr_pre->reserva.hora.dia,
                       curr_pre->reserva.hora.hora, curr_pre->reserva.hora.minutos, tipoRstr);
                curr_pre = curr_pre->next;
            }
            printf("--------------------------------------------------------------\n\n");
            printf("Que pre-reserva pretende cancelar?\n");

            int pre_reservationID;
            scanf("%d", &pre_reservationID);
            cancela_pre_reserva(current->listaPreReservas, pre_reservationID, true);
            return 1;
        } else {
            printf("Sem pre-reservas para este bloco!\n\n");
            usleep(500000);
            return 0;
        }
    } else {
        printf("ID de reserva nao encontrado!\n\n");
        return 0;
    }
}

// Verifica se ID introduzido exite na lista
int id_exists(ListaReservas *lista, int reservationID) {
    NoListaReservas *current = lista->start;
    int exist = 0;
    while (current) {
        if(current->reserva.ID == reservationID) {
            exist = 1;
            break;
        }
        current = current->next;
    }
    return exist;
}

// Menu de opções
void menu_inicial() {
    printf("\n-------------------------MENU-------------------------\n");
    printf("\t1 - Criar reserva\n");
    printf("\t2 - Cancelar reserva\n");
    printf("\t3 - Cancelar pre-reserva\n");
    printf("\t4 - Listar todas reservas\n");
    printf("\t5 - Listar reservas de cliente\n");
    printf("\t6 - Realizar uma reserva\n");
    printf("\t7 - Salvar reservas\n");
    printf("\t8 - Carregar reservas\n");
    printf("\t9 - QUIT\n");
    printf("------------------------------------------------------\n\n  -> ");
}

// Mostra as reservas do dia desejado
void print_reservas_dia(ListaReservas *lista, int dia) {
    if (lista->size > 0) {
        // Verifica se existem reservas nesse dia
        int flag = 0;
        NoListaReservas *current = lista->start;

        while (current) {
            char tipoRstr[15];
            strcpy(tipoRstr, current->reserva.tipo.tipoR == Manutencao ? "Manutencao" : "Lavagem");

            if (current->reserva.hora.dia == dia && flag == 0) {
                flag = 1;
                printf("Existem as seguintes reservas para o dia %d:\n", dia);
                printf("\n%02d:%02d -> %s (%d minutos)\n", current->reserva.hora.hora, current->reserva.hora.minutos,
                       tipoRstr, current->reserva.tipo.duracao);
            } else if (current->reserva.hora.dia == dia && flag != 0) {
                printf("%02d:%02d -> %s (%d minutos)\n", current->reserva.hora.hora, current->reserva.hora.minutos,
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

// Lista todas as reservas (mais antigas primeiro)
void list_all(ListaReservas *lista) {
    if (lista->size > 0) {
        NoListaReservas *current = lista->start;
        printf("\n---------------------------RESERVAS-----------------------\n");
        while (current) {
            char tipoRstr[15];
            strcpy(tipoRstr, current->reserva.tipo.tipoR == Manutencao ? "Manutencao" : "Lavagem");
            printf("\tDia %d - %02d:%02d -> %s\n", current->reserva.hora.dia,
                   current->reserva.hora.hora, current->reserva.hora.minutos, tipoRstr);

            // Listar também as pré-reservas (caso existam)
            if (current->listaPreReservas) {
                NoListaPre_Reservas *pre_current = current->listaPreReservas->start;
                while (pre_current) {
                    char tipoStr_pre[15];
                    strcpy(tipoStr_pre, pre_current->reserva.tipo.tipoR == Manutencao ? "Manutencao" : "Lavagem");
                    printf("\tDia %d - %02d:%02d -> %s (pre-reserva)\n", pre_current->reserva.hora.dia,
                           pre_current->reserva.hora.hora, pre_current->reserva.hora.minutos, tipoStr_pre);
                    pre_current = pre_current->next;
                }
            }
            current = current->next;
        }
        printf("----------------------------------------------------------\n\n");
        usleep(500000);
    } else {
        printf("Sem reservas registadas!\n");
    }
}

// Lista todas as reservas e pré-reservas de um cliente (mais recentes primeiro)
void list_client(NoListaReservas *node, int clientID) {
    if (node == NULL)
        return;

    // Chamada recursiva
    list_client(node->next, clientID);

    if (node->reserva.clientID == clientID) {
        char tipoRstr[15];
        strcpy(tipoRstr, node->reserva.tipo.tipoR == Manutencao ? "Manutencao" : "Lavagem");
        printf("\tDia %d - %02d:%02d -> %s\n", node->reserva.hora.dia,
               node->reserva.hora.hora, node->reserva.hora.minutos, tipoRstr);
        if (node->listaPreReservas) {
            NoListaPre_Reservas *pre_current = node->listaPreReservas->start;
            while (pre_current) {
                char tipoStr_pre[15];
                strcpy(tipoStr_pre, pre_current->reserva.tipo.tipoR == Manutencao ? "Manutencao" : "Lavagem");
                printf("\tDia %d - %02d:%02d -> %s (pre-reserva)\n", pre_current->reserva.hora.dia,
                       pre_current->reserva.hora.hora, pre_current->reserva.hora.minutos, tipoStr_pre);
                pre_current = pre_current->next;
            }
        }
    }
}

// Função para imprimir a árvore de clientes em ordem
void inorderTraversal(Client *root) {
    if (root != NULL) {
        inorderTraversal(root->left);
        printf("%d\n", root->clientID);
        inorderTraversal(root->right);
    }
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

    if (lista->start == NULL) {
        lista->start = novo_no;
        ++lista->size;
        ++reserva_autoID;
        //------------UPDATE-LOG---------------
        char tipoRstr[15];
        strcpy(tipoRstr, novo_no->reserva.tipo.tipoR == Manutencao ? "Manutencao" : "Lavagem");
        sprintf(out, "NEW RESERVATION CREATED! DAY %d - %s AT %02d:%02d\n\n", dia, tipoRstr, hora, minuto);
        update_log(out);
        return;
    }

    NoListaReservas *current = lista->start;
    NoListaReservas *previous = NULL;

    int new_start = (dia * 31 * 60 + hora * 60 + minuto);
    int new_end = (dia * 31 * 60 + hora * 60 + minuto + tipoRes.duracao);

    // Se a nova reserva acabar depois da atual começar, ou se for no dia seguinte, vai ao next
    // Avança até encontrar um que comece depois da nova acabar OU NO FIM DA LISTA
    //printf("%d -> %d", novo_no->reserva.hora.dia, current->reserva.hora.dia);
    while (current != NULL && (new_end > (current->reserva.hora.dia * 31 * 60 + current->reserva.hora.hora * 60 +
                                          current->reserva.hora.minutos))) {
        previous = current;
        current = current->next;
    }

    if (previous == NULL) {
        // Vaga no inicio -> lista tava vazia
        novo_no->next = lista->start;
        lista->start = novo_no;
        ++lista->size;
        ++reserva_autoID;
        //------------UPDATE-LOG---------------
        char tipoRstr[15];
        strcpy(tipoRstr, novo_no->reserva.tipo.tipoR == Manutencao ? "Manutencao" : "Lavagem");
        sprintf(out, "NEW RESERVATION CREATED! DAY %d - %s AT %02d:%02d\n\n", dia, tipoRstr, hora, minuto);
        update_log(out);
    } else if (current != NULL) {
        int previous_end = (previous->reserva.hora.dia * 31 * 60 + previous->reserva.hora.hora * 60 +
                            previous->reserva.hora.minutos +
                            previous->reserva.tipo.duracao);
        int current_start = (current->reserva.hora.dia * 31 * 60 + current->reserva.hora.hora * 60 +
                             current->reserva.hora.minutos);

        // Se a reserva anterior terminar antes da nova começar AND e a nova acabar antes da atual começar -> SUCESSO
        if ((previous_end <= new_start) && (new_end <= current_start)) {
            previous->next = novo_no;
            novo_no->next = current;
            ++lista->size;
            ++reserva_autoID;
            //------------UPDATE-LOG---------------
            char tipoRstr[15];
            strcpy(tipoRstr, novo_no->reserva.tipo.tipoR == Manutencao ? "Manutencao" : "Lavagem");
            sprintf(out, "NEW RESERVATION CREATED! DAY %d - %s AT %02d:%02d\n\n", dia, tipoRstr, hora, minuto);
            update_log(out);
        }
            // Se a reserva anterior acabar depois da nova começar OR a nova acabar depois da atual começar
        else if ((previous_end > new_start) || (new_end > current_start)) {
            printf("Horario ja ocupado! Pedido movido para pre-reserva\n");
            insert_pre_reserva(current->listaPreReservas, current, clientID, tipoRes, dia, hora, minuto);
            //------------UPDATE-LOG---------------
            char tipoRstr[15];
            strcpy(tipoRstr, novo_no->reserva.tipo.tipoR == Manutencao ? "Manutencao" : "Lavagem");
            sprintf(out, "NO SPOT AVAILABLE! NEW PRE-RESERVATION CREATED! DAY %d - %s AT %02d:%02d\n\n", dia, tipoRstr, hora,
                    minuto);
            update_log(out);
        }
    } else {
        int previous_end = (previous->reserva.hora.dia * 31 * 60 + previous->reserva.hora.hora * 60 +
                            previous->reserva.hora.minutos +
                            previous->reserva.tipo.duracao);
        if (previous_end > new_start) {
            printf("Horario ja ocupado! Pedido movido para pre-reserva\n");
            insert_pre_reserva(previous->listaPreReservas, previous, clientID, tipoRes, dia, hora, minuto);
            //------------UPDATE-LOG---------------
            char tipoRstr[15];
            strcpy(tipoRstr, novo_no->reserva.tipo.tipoR == Manutencao ? "Manutencao" : "Lavagem");
            sprintf(out, "NO SPOT AVAILABLE! NEW PRE-RESERVATION CREATED! DAY %d - %s AT %02d:%02d\n\n", dia, tipoRstr, hora,
                    minuto);
            update_log(out);
        } else {
            previous->next = novo_no;
            novo_no->next = current;
            ++lista->size;
            ++reserva_autoID;
            //------------UPDATE-LOG---------------
            char tipoRstr[15];
            strcpy(tipoRstr, novo_no->reserva.tipo.tipoR == Manutencao ? "Manutencao" : "Lavagem");
            sprintf(out, "NEW RESERVATION CREATED! DAY %d - %s AT %02d:%02d\n\n", dia, tipoRstr, hora, minuto);
            update_log(out);
        }
    }

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
                //-------------------
                if (no_a_remover->next) {
                    int next_start =
                            (no_a_remover->next->reserva.hora.hora * 60) + no_a_remover->next->reserva.hora.minutos;
                    int current_start = (no_a_remover->reserva.hora.hora * 60) + no_a_remover->reserva.hora.minutos;
                    int time_to_next = next_start - current_start;
                    NoListaPre_Reservas *no_preR;
                    no_preR = get_pre_reservation_node(no_a_remover->listaPreReservas, time_to_next, next_start);
                    if (no_preR) {// Passa a pré-reserva correta para a lista de reservas
                        pre_to_reserva(current, no_preR);
                        // Tirar este elemento das pre reservas
                        cancela_pre_reserva(current->listaPreReservas, no_preR->reserva.ID, false);
                        //------------UPDATE-LOG---------------
                        char tipoRstr[15], tipoPRstr[15];
                        strcpy(tipoRstr, no_a_remover->reserva.tipo.tipoR == Manutencao ? "Manutencao" : "Lavagem");
                        strcpy(tipoPRstr, no_preR->reserva.tipo.tipoR == Manutencao ? "Manutencao" : "Lavagem");
                        sprintf(out,
                                "RESERVATION CANCELED! DAY %d - %s AT %02d:%02d\nPRE-RESERVATION MOVED TO NEW AVAILABLE SLOT! DAY %d - %s AT %02d:%02d\n\n",
                                no_a_remover->reserva.hora.dia, tipoRstr, no_a_remover->reserva.hora.hora,
                                no_a_remover->reserva.hora.minutos, no_preR->reserva.hora.dia, tipoPRstr,
                                no_preR->reserva.hora.hora, no_preR->reserva.hora.minutos);
                        update_log(out);

                        printf("Reserva cancelada! Pre-reserva ocupou o slot disponivel!\n");

                    } else {
                        printf("Sem pre-reservas validas para o bloco libertado! Reserva eliminada!\n");
                        // Remover o 1º nó
                        if (previous == NULL)
                            lista->start = no_a_remover->next;
                            // Restantes nós
                        else
                            previous->next = no_a_remover->next;

                        //------------UPDATE-LOG---------------
                        char tipoRstr[15];
                        strcpy(tipoRstr, no_a_remover->reserva.tipo.tipoR == Manutencao ? "Manutencao" : "Lavagem");
                        sprintf(out, "RESERVATION CANCELED! NO VALID PRE-RESERVATIONS! DAY %d - %s AT %02d:%02d\n\n",
                                no_a_remover->reserva.hora.dia, tipoRstr, no_a_remover->reserva.hora.hora,
                                no_a_remover->reserva.hora.minutos);
                        update_log(out);

                        printf("Reserva cancelada!\n");

                        free(no_a_remover);
                        --lista->size;
                    }
                }
            }
                // Se não tiver pré-reservas
            else {
                // Remover o 1º nó
                if (previous == NULL)
                    lista->start = no_a_remover->next;
                    // Restantes nós
                else
                    previous->next = no_a_remover->next;

                //------------UPDATE-LOG---------------
                char tipoRstr[15];
                strcpy(tipoRstr, no_a_remover->reserva.tipo.tipoR == Manutencao ? "Manutencao" : "Lavagem");
                sprintf(out, "RESERVATION CANCELED! NO PRE-RESERVATIONS FOUND! DAY %d - %s AT %02d:%02d\n\n",
                        no_a_remover->reserva.hora.dia, tipoRstr, no_a_remover->reserva.hora.hora,
                        no_a_remover->reserva.hora.minutos);
                update_log(out);

                printf("Reserva cancelada!\n");

                free(no_a_remover);
                --lista->size;
            }
        }
    }
}

// Cancela uma pré-reserva
void cancela_pre_reserva(ListaPre_Reservas *lista_pre, int reservationID, bool isNotFromCancelR) {
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
            printf("ID de pre-reserva nao encontrado\n");
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

            if(isNotFromCancelR) {
                //------------UPDATE-LOG---------------
                char tipoRstr[15];
                strcpy(tipoRstr, no_a_remover->reserva.tipo.tipoR == Manutencao ? "Manutencao" : "Lavagem");
                sprintf(out, "PRE-RESERVATION CANCELED! DAY %d - %s AT %02d:%02d\n\n",
                        no_a_remover->reserva.hora.dia, tipoRstr, no_a_remover->reserva.hora.hora,
                        no_a_remover->reserva.hora.minutos);
                update_log(out);

                printf("Pre-reserva cancelada!\n");

            }

            free(no_a_remover);
            lista_pre->size -= 1;

        }
    }
}

//Passa uma pré-reserva para reserva
void pre_to_reserva(NoListaReservas *current, NoListaPre_Reservas *curr_pre) {
    current->reserva.hora.dia = curr_pre->reserva.hora.dia;
    current->reserva.hora.hora = curr_pre->reserva.hora.hora;
    current->reserva.hora.minutos = curr_pre->reserva.hora.minutos;
    current->reserva.tipo.tipoR = curr_pre->reserva.tipo.tipoR;
    current->reserva.tipo.duracao = curr_pre->reserva.tipo.duracao;
    current->reserva.clientID = curr_pre->reserva.clientID;
}

// Vai receber a reserva a realizar e executa uma a uma todas as anteriores até à desejada, inclusive
// "Executa", aka, elimina da lista e dá um print a dizer que a realizou
void realiza_reserva(ListaReservas *lista, int reservationID) {
    NoListaReservas *current = lista->start;

    // While termina na reserva antes da que queremos realizar
    while (current && current->reserva.ID != reservationID) {
        NoListaReservas *next = current->next;
        //------------UPDATE-LOG---------------
        char tipoRstr[15];
        strcpy(tipoRstr, current->reserva.tipo.tipoR == Manutencao ? "Manutencao" : "Lavagem");
        sprintf(out, "DAY %d - %s AT %02d:%02d FINISHED!\n\n", current->reserva.hora.dia, tipoRstr, current->reserva.hora.hora, current->reserva.hora.minutos);
        update_log(out);

        printf("Reserva %d finalizada (%s -> Dia %d as %02d:%02d)\n", current->reserva.ID, tipoRstr, current->reserva.hora.dia, current->reserva.hora.hora, current->reserva.hora.minutos);
        usleep(500000);
        free(current);
        current = next;
        lista->size--;
    }
    if (current) {
        NoListaReservas *next = current->next;
        //------------UPDATE-LOG---------------
        char tipoRstr[15];
        strcpy(tipoRstr, current->reserva.tipo.tipoR == Manutencao ? "Manutencao" : "Lavagem");
        sprintf(out, "DAY %d - %s AT %02d:%02d EXECUTED!\n\n", current->reserva.hora.dia, tipoRstr,
                current->reserva.hora.hora, current->reserva.hora.minutos);
        update_log(out);

        printf("Realizando reserva %d (%s -> Dia %d as %02d:%02d)\n", current->reserva.ID, tipoRstr,
               current->reserva.hora.dia, current->reserva.hora.hora, current->reserva.hora.minutos);
        usleep(500000);

        free(current);
        current = next;
        lista->size--;
        lista->start = current;
    }
}

// Salva os dados das listas de reservas e pré-reservas num ficheiro binário
void saveLinkedListToFile(NoListaReservas *node) {

    FILE *file = fopen("bin_file.bin", "wb");
    if (file == NULL) {
        printf("Failed to write the file\n");
        return;
    }

    // Percorre a lista principal e escreve cada nó no arquivo
    NoListaReservas *current = node;
    while (current != NULL) {
        // Escreve os dados do nó principal
        fwrite(&(current->reserva), sizeof(Reserva), 1, file);

        // Verifica se a lista auxiliar está presente
        int hasAuxList = (current->listaPreReservas->start != NULL) ? 1 : 0;
        fwrite(&hasAuxList, sizeof(int), 1, file);

        // Se a lista auxiliar estiver presente, escreve seus dados
        if (hasAuxList) {

            NoListaPre_Reservas *auxCurrent = current->listaPreReservas->start;
            while (auxCurrent != NULL) {
                fwrite(&(auxCurrent->reserva), sizeof(Reserva), 1, file);

                auxCurrent = auxCurrent->next;
            }
        }

        current = current->next;
    }

    fclose(file);
    //------------UPDATE-LOG---------------
    update_log("DATA SAVED IN BINARY FILE!\n");
}

// Salva os dados da árvore de clientes no ficheiro binário
void saveClientTreeToFile(Client* root, FILE* file) {
    if (root == NULL) {
        return;
    }

    // Write the current client to the file
    fwrite(root, sizeof(Client), 1, file);

    // Save the left and right subtrees recursively
    saveClientTreeToFile(root->left, file);
    saveClientTreeToFile(root->right, file);
}

// Atualiza ficheiro de texto com informações relevantes
void update_log(char msg[256]) {

    FILE *file = fopen("log.txt", "a");
    if (file == NULL) {
        printf("Failed to update file\n");
        return;
    }
    fputs(msg, file);

    fclose(file);
}
