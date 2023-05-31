#include "project.h"

// NOTA: FICHEIROS BINÁRIOS E DE TEXTO SÃO CRIADOS NA PASTA cmake-build-debug!!!

int main() {

    // DIRECTORY
    char cwd[1024];
    //Finds current working directory
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        printf("getcwd error\n");
        return 0;
    }

    //Works on the current directory
    if (chdir(cwd) != 0) {
        printf("chdir error\n");
        return 0;
    }
    int option = 1;
    int clientID, reservationID, dia, hora, minuto;
    tipoReserva tipoRes;

    ListaReservas *listaReservas = create_lista_reservas();
    Client *root = NULL;
    while (option) {
        menu_inicial();
        fflush(stdin);
        scanf("%d", &option);

        // --------------------------NOVA-RESERVA--------------------------
        if (option == 1) {
            printf("Insira o ID do cliente\n");
            scanf("%d", &clientID);
            root = insert(root, clientID);
            printf("Escolha o tipo de operacao (M/L)\n");
            char tipoR[2];
            scanf("%s", tipoR);
            while (strcmp(tipoR, "M") != 0 && strcmp(tipoR, "L") != 0) {
                printf("Tipo invalido!\nEscolha o tipo de operacao (M/L)\n");
                scanf("%s", tipoR);
            }
            if (strcmp(tipoR, "M") == 0) {
                tipoRes.tipoR = Manutencao;
                tipoRes.duracao = 60;
            } else if (strcmp(tipoR, "L") == 0) {
                tipoRes.tipoR = Lavagem;
                tipoRes.duracao = 30;
            }

            printf("Qual o dia pretendido?\n");
            scanf("%d", &dia);
            // Proteção básica, nao há necessidade de fazer algo mais elaborado para o conceito do exercício
            while (dia < 1 || dia > 31) {
                printf("Dia invalido!\nQual o dia pretendido?\n");
                scanf("%d", &dia);
            }
            print_reservas_dia(listaReservas, dia);

            printf("\nEscolha a hora desejada (hora:minutos)\n");
            minuto = 0;
            scanf("%d:%d", &hora, &minuto);

            insert_reserva(listaReservas, clientID, tipoRes, dia, hora, minuto);
        }

            // ------------------------CANCELAR-RESERVA------------------------
        else if (option == 2) {
            //PRINT FULL LIST
            if(print_reservas(listaReservas, true)) {
                printf("Que reserva pretende cancelar?\n");
                scanf("%d", &reservationID);
                cancela_reserva(listaReservas, reservationID);
            }
        }

            // ------------------------CANCELAR-PRE-RESERVA------------------------
        else if (option == 3) {
            // PRINT FULL LIST
            if(print_reservas(listaReservas, false)) {
                // ESCOLHER A RESERVA
                printf("Escolha uma reserva\n");
                scanf("%d", &reservationID);
                // PRINT PRÉ-RESERVAS DESSA HORA (O CANCEL É CHAMADO NELE)
                if (print_pre_reservas(listaReservas, reservationID)) {
                    printf("Pre-reserva cancelada!\n");
                }
            }
        }

            // --------------------------LIST-ALL--------------------------
        else if (option == 4) {
            list_all(listaReservas);
        }

            // --------------------------LIST-CLIENT--------------------------
        else if (option == 5) {
            if (listaReservas->size > 0){
                printf("\n-------------------------CLIENTES-------------------------\n");
                inorderTraversal(root);
                printf("----------------------------------------------------------\n\n");
                printf("Escolha um cliente\n");
                scanf("%d", &clientID);
                printf("\n---------------------------RESERVAS-----------------------\n");
                list_client(listaReservas->start, clientID);
                printf("----------------------------------------------------------\n\n");
            }
            else {
                printf("Sem reservas registadas!\n");
            }
        }
            // --------------------REALIZAR-RESERVA--------------------
        else if (option == 6) {
            //PRINT FULL LIST
            if(print_reservas(listaReservas, true)) {
                printf("Que reserva pretende realizar?\n");
                scanf("%d", &reservationID);
                realiza_reserva(listaReservas, reservationID);
            }

        }

            // --------------------------SAVE--------------------------
        else if (option == 7) {
            saveLinkedListToFile(listaReservas->start);
            printf("\nDados salvos no ficheiro binario!\n");
        }

            // --------------------------LOAD--------------------------
        else if (option == 8) {
            listaReservas->start = loadLinkedListFromFile(&listaReservas->size);
            printf("\nDados do ficheiro binario carregados!\n");
        }
    }
    return 0;
}



