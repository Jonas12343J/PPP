#include "project.h"

int main() {

    int option = 1;
    int clientID, dia, hora, minuto, sucesso = 0;
    char tipoR;
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

            // Só para o while
            tipoR = 'O';
            while(tipoR != 'M' && tipoR != 'L') {
                scanf("%c", &tipoR);
                if (tipoR == 'M') {
                    tipoRes.tipoR = Manutencao;
                    tipoRes.duracao = 60;
                } else if (tipoR == 'L') {
                    tipoRes.tipoR = Lavagem;
                    tipoRes.duracao = 30;
                }
                printf("Tipo invalido!\nEscolha o tipo de operacao (M/L)\n");
            }

            printf("Qual o dia pretendido?\n");
            scanf("%d", &dia);
            print_reservas_dia(listaReservas, dia);
            while(sucesso == 0 || sucesso == 2) {
                printf("\nEscolha a hora desejada (hora:minutos)\n");
                scanf("%d:%d", &hora, &minuto);
                sucesso = check_disponibilidade(listaReservas, dia, hora, minuto, tipoRes.duracao);
            }
        }

        // ------------------------CANCELAR-RESERVA------------------------
        else if (option == 2) {

        }

        // --------------------------LIST-ALL--------------------------
        else if (option == 3) {

        }

        // --------------------------LIST-CLIENT--------------------------
        else if (option == 4) {

        }

        // --------------------------SAVE--------------------------
        else if (option == 5) {

        }

        // --------------------------LOAD--------------------------
        else if (option == 6) {

        }
    }
    return 0;
}

