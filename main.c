#include "project.h"


int main() {

    int option = 1;
    int clientID, sucesso = 0, reservationID;
    tipoReserva tipoRes;
    char dia[2], hora[2], minuto[2];


    ListaReservas *listaReservas = create_lista_reservas();
    while (option) {
        menu_inicial();
        scanf("%d", &option);

        // --------------------------NOVA-RESERVA--------------------------
        if (option == 1) {
            printf("Insira o ID do cliente\n");
            scanf("%d", &clientID);
            printf("Escolha o tipo de operacao (M/L)\n");
            char tipoR[2];
            scanf("%s", tipoR);
            while(strcmp(tipoR, "M") != 0 && strcmp(tipoR, "L") != 0) {
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
            scanf("%s", dia);
            // Proteção básica, nao há necessidade de fazer algo mais elaborado para o conceito do exercício
            while(atoi(dia) < 1 || atoi(dia) > 31){
                printf("Dia invalido!\nQual o dia pretendido?\n");
                scanf("%s", dia);
            }
            print_reservas_dia(listaReservas, atoi(dia));
            while(sucesso == 0 || sucesso == 2) {
                printf("\nEscolha a hora desejada (hora:minutos)\n");
                scanf("%s:%s", hora, minuto);
                sucesso = check_disponibilidade(listaReservas, atoi(dia), atoi(hora), atoi(minuto), tipoRes, clientID);
            }
            if(sucesso == 1) {
                insert_reserva(listaReservas, clientID, tipoRes, atoi(dia), atoi(hora), atoi(minuto));
                ++reserva_autoID;
            }
            sucesso = 0;
        }

        // ------------------------CANCELAR-RESERVA------------------------
        else if (option == 2) {
            //PRINT FULL LIST
            print_reservas(listaReservas);
            printf("Que reserva pretende cancelar?\n");
            scanf("%d", &reservationID);
            cancela_reserva(listaReservas, reservationID);
        }

            // ------------------------CANCELAR-PRE-RESERVA------------------------
        else if (option == 3) {
            // PRINT FULL LIST
            // ESCOLHER A RESERVA
            // PRINT PRÉ-RESERVAS DESSA HORA

        }

        // --------------------------LIST-ALL--------------------------
        else if (option == 4) {

        }

        // --------------------------LIST-CLIENT--------------------------
        else if (option == 5) {

        }

        // --------------------------SAVE--------------------------
        else if (option == 6) {

        }

        // --------------------------LOAD--------------------------
        else if (option == 7) {

        }
    }
    return 0;
}



