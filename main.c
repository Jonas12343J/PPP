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

    int option = 9;
    int minuto;
    char c_clientID[10], c_reservationID[10], c_dia[5], c_hora[50];
    tipoReserva tipoRes;

    ListaReservas *listaReservas = create_lista_reservas();
    Client *root = NULL;
    while (option) {
        // Inicializa a 9 por ser um valor não presente na lista de opções
        // Caso o input esteja inválido (letra/cadeia) ele pede um novo
        option = 9;
        menu_inicial();
        scanf("%d", &option);

        // ----------------------------NOVA-RESERVA----------------------------
        if (option == 1) {
            printf("Insira o ID do cliente\n");

            scanf("%s", c_clientID);
            while(atoi(c_clientID) == 0){
                printf("Valor nao numerico introduzido!\n");
                scanf("%s", c_clientID);
            }

            root = insert(root, atoi(c_clientID));
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

            scanf("%s", c_dia);
            while(atoi(c_dia) == 0){
                printf("Valor nao numerico introduzido!\n");
                scanf("%s", c_dia);
            }

            // Proteção básica, nao há necessidade de fazer algo mais elaborado para o conceito do exercício
            while (atoi(c_dia) < 1 || atoi(c_dia) > 31) {
                printf("Dia invalido!\nQual o dia pretendido?\n");
                scanf("%s", c_dia);
                while(atoi(c_dia) == 0){
                    printf("Valor nao numerico introduzido!\n");
                    scanf("%s", c_dia);
                }
            }
            print_reservas_dia(listaReservas, atoi(c_dia));

            printf("\nEscolha a hora desejada (hora:minutos)\n");
            minuto = 0;
            scanf("%s:%d", c_hora, &minuto);

            while(atoi(c_hora) == 0){
                printf("Valor nao numerico introduzido!\n");
                scanf("%s", c_hora);
            }

            insert_reserva(listaReservas, atoi(c_clientID), tipoRes, atoi(c_dia), atoi(c_hora), minuto);
        }

        // --------------------------CANCELAR-RESERVA--------------------------
        else if (option == 2) {
            //PRINT FULL LIST
            if(print_reservas(listaReservas, true)) {
                printf("Que reserva pretende cancelar?\n");

                scanf("%s", c_reservationID);
                while(atoi(c_reservationID) == 0){
                    printf("Valor nao numerico introduzido!\n");
                    scanf("%s", c_reservationID);
                }
                cancela_reserva(listaReservas, atoi(c_reservationID));
            }
        }

        // ------------------------CANCELAR-PRE-RESERVA------------------------
        else if (option == 3) {
            // PRINT FULL LIST
            if(print_reservas(listaReservas, false)) {
                // ESCOLHER A RESERVA
                printf("Escolha uma reserva\n");

                scanf("%s", c_reservationID);
                while(atoi(c_reservationID) == 0){
                    printf("Valor nao numerico introduzido!\n");
                    scanf("%s", c_reservationID);
                }

                // PRINT PRÉ-RESERVAS DESSA HORA (O CANCEL É CHAMADO NELE)
                if (print_pre_reservas(listaReservas, atoi(c_reservationID))) {
                    printf("Pre-reserva cancelada!\n");
                }
            }
        }

        // ------------------------------LIST-ALL------------------------------
        else if (option == 4) {
            list_all(listaReservas);
        }

        // ------------------------------LIST-CLIENT---------------------------
        else if (option == 5) {
            if (listaReservas->size > 0){
                printf("\n-------------------------CLIENTES-------------------------\n");
                inorderTraversal(root);
                printf("----------------------------------------------------------\n\n");
                printf("Escolha um cliente\n");
                scanf("%s", c_clientID);
                while(atoi(c_clientID) == 0){
                    printf("Valor nao numerico introduzido!\n");
                    scanf("%s", c_clientID);
                }
                printf("\n---------------------------RESERVAS-----------------------\n");
                list_client(listaReservas->start, atoi(c_clientID));
                printf("----------------------------------------------------------\n\n");
            }
            else {
                printf("Sem reservas registadas!\n");
            }
        }

        // --------------------------REALIZAR-RESERVA--------------------------
        else if (option == 6) {
            //PRINT FULL LIST
            if(print_reservas(listaReservas, true)) {
                printf("Que reserva pretende realizar?\n");

                scanf("%s", c_reservationID);
                while(atoi(c_reservationID) == 0){
                    printf("Valor nao numerico introduzido!\n");
                    scanf("%s", c_reservationID);
                }

                realiza_reserva(listaReservas, atoi(c_reservationID));
            }
        }

        // --------------------------------SAVE--------------------------------
        else if (option == 7) {
            saveLinkedListToFile(listaReservas->start);
            printf("\nDados salvos no ficheiro binario!\n");
        }

        // --------------------------------LOAD--------------------------------
        else if (option == 8) {
            listaReservas->start = loadLinkedListFromFile(&listaReservas->size);
            printf("\nDados do ficheiro binario carregados!\n");
        }

        // ----------------------------WRONG-INPUT-----------------------------
        else
            printf("Valor introduzido invalido!\n");

    }
    return 0;
}



