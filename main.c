#include "project.h"


int main() {

    int option = 1;
    int clientID, sucesso = 0, reservationID, dia, hora, minuto;
    tipoReserva tipoRes;


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
            while (sucesso == 0 || sucesso == 2) {
                printf("\nEscolha a hora desejada (hora:minutos)\n");
                minuto = 0;
                scanf("%d:%d", &hora, &minuto);


                // TODO refazer esta funcao
                // Procurar o spor certo primeiro maybe
                sucesso = check_disponibilidade(listaReservas, dia, hora, minuto, tipoRes, clientID);
            }
            if (sucesso == 1) {
                insert_reserva(listaReservas, clientID, tipoRes, dia, hora, minuto);
                ++reserva_autoID;
            }
            sucesso = 0;
        }

            // ------------------------CANCELAR-RESERVA------------------------
        else if (option == 2) {
            //PRINT FULL LIST
            if(print_reservas(listaReservas)) {
                printf("Que reserva pretende cancelar?\n");
                scanf("%d", &reservationID);
                cancela_reserva(listaReservas, reservationID);
            }
        }

            // ------------------------CANCELAR-PRE-RESERVA------------------------
        else if (option == 3) {
            // PRINT FULL LIST
            if(print_reservas(listaReservas)) {
                // ESCOLHER A RESERVA
                printf("Escolha uma reserva\n");
                scanf("%d", &reservationID);
                // PRINT PRÉ-RESERVAS DESSA HORA
                print_pre_reservas(listaReservas, reservationID);
            }
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



