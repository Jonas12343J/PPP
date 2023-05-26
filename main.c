#include "project.h"


int main() {

    int option = 1;
    int clientID, dia, hora, minuto, sucesso = 0, reservationID;
    tipoReserva tipoRes;

    ListaReservas *listaReservas = create_lista_reservas();
    while (option) {
        menu_inicial();
        scanf("%d", &option);

        // --------------------------NOVA-RESERVA--------------------------
        if (option == 1) {
            printf("Insira o ID do cliente\n");
            scanf("%d", &clientID);
            getchar();
            printf("Escolha o tipo de operacao (M/L)\n");

            char tipoR;
            scanf("%c", &tipoR);
            while(tipoR != 'M' && tipoR != 'L') {
                printf("Tipo invalido!\nEscolha o tipo de operacao (M/L)\n");
                scanf("%c", &tipoR);
            }
            if (tipoR == 'M') {
                tipoRes.tipoR = Manutencao;
                tipoRes.duracao = 60;
            } else if (tipoR == 'L') {
                tipoRes.tipoR = Lavagem;
                tipoRes.duracao = 30;
            }

            printf("Qual o dia pretendido?\n");
            scanf("%d", &dia);
            print_reservas_dia(listaReservas, dia);
            while(sucesso == 0 || sucesso == 2) {
                printf("\nEscolha a hora desejada (hora:minutos)\n");
                scanf("%d:%d", &hora, &minuto);
                // Fix temporario
                if(minuto > 59)
                    minuto = 0;
                sucesso = check_disponibilidade(listaReservas, dia, hora, minuto, tipoRes.duracao, tipoRes, clientID);
            }
            if(sucesso == 1) {
                insert_reserva(listaReservas, clientID, tipoRes, dia, hora, minuto);
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

            // ------------------------CANCELAR-RESERVA------------------------
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



