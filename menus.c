#include "menus.h"





// FUNCOES COMPLEMENTARES AOS MENUS
void limpar_ecra ()
{
    for(int i = 0; i < 30; i++)
    {
        printf("\n");
    }
}

void pausar()
{
    printf("\n  Prima ENTER para continuar...");
    while (getchar() != '\n');
}



// MENUS
void menu_principal(Supermercado *sm)
{
    int op;

do{
    printf("  |------------------------------------------|\n");
    printf("  |               SUPERMERCADO               |\n");
    printf("  |------------------------------------------|\n");
    printf("  | 1. Gerir Caixas                          |\n");
    printf("  | 2. Gerir Clientes                        |\n");
    printf("  | 3. Simular Tick de Tempo                 |\n");
    printf("  | 4. Estatísticas                          |\n");
    printf("  | 5. Sistema / Memória                     |\n");
    printf("  | 6. Gravar Relatório                      |\n");
    printf("  | 0. Sair                                  |\n");
    printf("  |------------------------------------------|\n");
    printf("  | Opção: ");
    scanf("%d", &op);
    LIMPAR_BUFFER();

    switch (op)
    {
    case 1: menu_caixas(sm);            break;
    case 2: menu_clientes(sm);          break;
    case 3: printf("DESENVOLVER\n");    break;
    case 4: menu_estatisticas(sm);      break;
    case 5: printf("DESENVOLVER\n");    break;
    case 6: printf("DESENVOLVER\n");    break;
    case 0: break;

    default: printf("  Opcao inválida.\n"); pausar();
    }
    limpar_ecra();
}while(op != 0);

}


void menu_caixas(Supermercado *sm)
{
    limpar_ecra();
    int op;
    do{
    printf("  |------------------------------------------|\n");
    printf("  |             GESTÃO DE CAIXAS             |\n");
    printf("  |------------------------------------------|\n");
    printf("  | 1. Listar todas as caixas                |\n");
    printf("  | 2. Abrir caixa                           |\n");
    printf("  | 3. Fechar caixa                          |\n");
    printf("  | 4. Fechar caixa (redistribuir clientes)  |\n");
    printf("  | 5. Verificar abertura/fecho automatico   |\n");
    printf("  | 0. Voltar                                |\n");
    printf("  |------------------------------------------|\n");
    printf("  | Opção: ");
    scanf("%d", &op);
    LIMPAR_BUFFER();

    switch(op)
    {
    case 1: printf("DESENVOLVER\n");    break;
    case 2: printf("DESENVOLVER\n");    break;
    case 3: printf("DESENVOLVER\n");    break;
    case 4: printf("DESENVOLVER\n");    break;
    case 5: printf("DESENVOLVER\n");    break;
    case 0: break;
    default: printf("  Opcao inválida.\n"); pausar();
    }
    limpar_ecra();

    }while(op != 0);
}


void menu_clientes(Supermercado *sm)
{
    limpar_ecra();
    int op;

    do{
    printf("  |------------------------------------------|\n");
    printf("  |             GESTÃO DE CLIENTES           |\n");
    printf("  |------------------------------------------|\n");
    printf("  | 1. Pesquisar cliente                     |\n");
    printf("  | 2. Mover cliente para outra caixa        |\n");
    printf("  | 3. Listar fila de uma caixa              |\n");
    printf("  | 0. Voltar                                |\n");
    printf("  |------------------------------------------|\n");
    printf("  | Opção: ");
    scanf("%d", &op);
    LIMPAR_BUFFER();

    switch(op)
    {
    case 1: printf("DESENVOLVER\n");    break;
    case 2: printf("DESENVOLVER\n");    break;
    case 3: printf("DESENVOLVER\n");    break;
    case 0: break;
        default: printf("  Opcao inválida.\n"); pausar();
    }
    limpar_ecra();

    }while(op != 0);
}



void menu_estatisticas(Supermercado *sm)
{
    limpar_ecra();
    int op;

    do{
    printf("  |------------------------------------------|\n");
    printf("  |                 ESTATISTICAS             |\n");
    printf("  |------------------------------------------|\n");
    printf("  | 1. Caixa com mais atendimentos           |\n");
    printf("  | 2. Caixa com mais produtos vendidos      |\n");
    printf("  | 3. Listar clientes de uma caixa          |\n");
    printf("  | 4. Operador com menos atendimentos       |\n");
    printf("  | 5. Produtos oferecidos e custo           |\n");
    printf("  | 0. Voltar                                |\n");
    printf("  |------------------------------------------|\n");
    printf("  | Opção: ");
    scanf("%d", &op);
    LIMPAR_BUFFER();

    switch(op)
    {
    case 1: printf("DESENVOLVER\n");    break;
    case 2: printf("DESENVOLVER\n");    break;
    case 3: printf("DESENVOLVER\n");    break;
    case 4: printf("DESENVOLVER\n");    break;
    case 5: printf("DESENVOLVER\n");    break;
    case 0: break;
        default:  printf("  Opcao inválida.\n"); pausar();
    }
    limpar_ecra();

    }while (op != 0);
}








