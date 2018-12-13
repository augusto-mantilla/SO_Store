void main(void){
    printf("\t=====================Monitor=======================\n");
    printf("\tEstado actual => %s\n", state);
    printf("\tClientes: %d\n", cli_total);
    printf("\tClientes na fila: %d\n", num_cli_row);
    printf("\tEmpregados: %d\n", num_empl);
    printf("\tEmpregados ao balcao: %d\n", num_empl_balc);
    printf("\tRepositores: %d\n", num_empl_stock);
    printf("\tDesistencias: %d\n", num_quits);
    printf("\tClientes atendidos: %d\n", num_happy_cli);

    printf("\t-------------------Prateleiras-------------------\n");
    printf("\tQuant. Produto %c: %d\n", prod_name, prod_qt_shelf);

    printf("\t---------------------Stock-----------------------\n");
    printf("\tQuant. Produto %c: %d\n", prod_name, prod_qt_stock);

    printf("\tTempo medio de espera: %d min\n", wait_time);
    printf("\t===================================================\n");



    printf("=====================Simulator=====================\n");
    printf("Chegou um cliente. Numero %d\n", cli_num);
    printf("O cliente %d pede o produto %c\n", cli_num, prod_name);
    printf("O cliente %d recebe produto %c\n", cli_num, prod_name);
    printf("O cliente %d saiu da loja feliz\n", cli_num);
    printf("O cliente %d desistiu\n", cli_num);
    printf("Novo empregado começa a servir\n");
    printf("Empregado acaba de servir\n");
    printf("Novo repositor começa a encher prateleiras\n");
    printf("Repositor acaba de trabalhar\n");
    
    printf("Acabou o produto %c\n", prod_name);
    printf("Repositor enche a prateleira do produto %c\n", prod_name);
    printf("Foram repostas %d unidades do produto %c\n", prod_qt_stocked, prod_name);
    
}