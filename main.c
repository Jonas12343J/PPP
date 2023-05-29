#include "project.h"


ListaPre_Reservas *get_list_pre(ListaReservas *pReservas, int id);

int main() {

    FILE *log, *bin_file;


    int option = 1;
    int clientID, reservationID, pre_reservationID, dia, hora, minuto;
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

        // --------------------------SAVE--------------------------
        else if (option == 6) {
            bin_file = fopen("bin_file.bin", "wb");
            if(bin_file == NULL) {
                printf("Failed to write the file\n");
                return 1;
            }
            // Write the struct to the file
            fwrite(&listaReservas, sizeof(ListaReservas), 1, bin_file);
            //fwrite(&listaReservas, sizeof(ListaReservas) * listaReservas->size, 1, bin_file);

            // Close the file
            fclose(bin_file);

            printf("\nDados salvos no ficheiro binario!\n");
        }

        // --------------------------LOAD--------------------------
        else if (option == 7) {
            bin_file = fopen("bin_file.bin", "rb");
            if (bin_file == NULL) {
                printf("Failed to read the file\n");
                return 1;
            }

            // Read the struct from the file
            fread(&listaReservas, sizeof(ListaReservas), 1, bin_file);
            //fread(&listaReservas, sizeof(ListaReservas) * listaReservas->size, 1, bin_file);

            // Close the file
            fclose(bin_file);

        }
    }
    return 0;
}




