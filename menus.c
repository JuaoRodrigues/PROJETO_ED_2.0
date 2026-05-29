#include "menus.h"
#include "estatisticas.h"

#ifdef _WIN32
    #include <windows.h>
#endif





// FUNCOES COMPLEMENTARES AOS MENUS
void limpar_ecra ()
{
    #ifdef _WIN32
        system("cls"); // Comando para Windows
    #else
        system("clear"); // Comando para Linux e macOS
    #endif
}

void pausar()
{
    printf("\n  Prima ENTER para continuar...");
    while (getchar() != '\n');
}

// relativo à simulacao da forma que esta
// mover o cursor para o inicio e escrever por cima
void ativarANSI() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
}


void simular (Supermercado *sm, int hora, int minuto, int previstos)
{
    printf("\033[H");       //move o cursor para o inicio sem limpar
    printf("  |------------------------------------------------------\n");
    printf("  |                     SIMULAÇÃO\n");
    printf("  |------------------------------------------------------\n");
    if(hora<24) printf("  | Horas: [%02d:%02d]\n  |\n", hora, minuto);
    else{
        hora -= 24;
        printf("  | Horas: [%02d:%02d]\n  |\n", hora, minuto);
    }

    int total_filas = 0;
    for(int i = 0; i < sm->config.n_caixas; i++)
    {
        Caixa *cai = &sm->caixas[i];
        total_filas += cai->fila.tamanho;

        if(cai->ativa != 0)
        {
            printf("  \033[2K| Caixa %d : ", cai->id);
            for(int j = 0; j < cai->fila.tamanho; j++){printf("#");}

            int espacos = 40 - cai->fila.tamanho;
            for (int j = 0; j < espacos; j++) printf(" ");
            printf("%d\n", cai->fila.tamanho);

        }else printf("  | Caixa %d : Fora de Serviço!                        0\n", cai->id);
    }
    printf("  |------------------------------------------------------\n");
    printf("  | Clientes na Loja    : %02d\n", sm->clientes_na_loja.total_na_loja);
    printf("  | Clientes na Fila    : %02d\n", total_filas);
    printf("  | Clientes Previstos  : %02d\n", previstos);
    printf("  | Produtos oferecidos : %02d\n", sm->produtos_oferecidos_total);
    printf("  |------------------------------------------------------\n");
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
    printf("  | 1. Estatísticas gerais                   |\n");
    printf("  | 2. Taxa de oferta de produtos            |\n");
    printf("  | 3. Listar clientes de uma caixa          |\n");
    printf("  | 4. Listar funcionários                   |\n");
    printf("  | 5. Estatísticas dos clientes             |\n");
    printf("  | 0. Voltar                                |\n");
    printf("  |------------------------------------------|\n");
    printf("  | Opção: ");
    scanf("%d", &op);
    LIMPAR_BUFFER();

    switch(op)
    {
    case 1: limpar_ecra(); estatisticas_gerais(sm); pausar(); limpar_ecra(); break;
    case 2: limpar_ecra(); taxa_oferta(sm); pausar(); limpar_ecra(); break;
    case 3:
        limpar_ecra();
        int n;
        do
        {
            printf("\nIndique a caixa: ");
            scanf("%d", &n);
            LIMPAR_BUFFER();
            if(n <= 0 || n > sm->config.n_caixas)    {printf("\n  Número de caixa inválido! Tente um valor entre 1 e %d\n", sm->config.n_caixas); continue;}
            Caixa *caix = &sm->caixas[n - 1];
            imprimir_historico(caix);
            pausar();
        }while (n <= 0 || n > sm->config.n_caixas);
        limpar_ecra();
        break;
    case 4:
        limpar_ecra();
        printf("\n=== Todos os funcionários do Supermercado\n");
        for(int i = 0; i < sm->config.n_caixas; i++) { printf("CAIXA %d : %d - %s\n", i+1, sm->caixas[i].operador_id, sm->caixas[i].operador_nome); }
        pausar();
        limpar_ecra();
        break;
    case 5:
        limpar_ecra();
        estatisticas_clientes(sm);
        pausar();
        limpar_ecra();
        break;
    case 0: break;
        default:  printf("  Opcao inválida.\n"); pausar();
    }
    //limpar_ecra();

    }while (op != 0);
    limpar_ecra();
}

void pausarSimulacao(Supermercado *sm) {
    int op;
    do {
        printf("  |------------------------------------------|\n");
        printf("  |            SIMULACAO PAUSADA             |\n");
        printf("  |------------------------------------------|\n");
        printf("  | 1. Gerir Caixas                          |\n");
        printf("  | 2. Ver estatisticas do momento           |\n");
        printf("  | 3. Retomar simulacao                     |\n");
        printf("  | 0. Cancelar simulacao                    |\n");
        printf("  |------------------------------------------|\n");
        printf("  | Opcao: ");
        scanf("%d", &op);
        LIMPAR_BUFFER();

        switch (op) {
            case 1: limpar_ecra(); menu_estatisticas(sm); break;
            case 2: printf("DESENVOLVER\n"); break;
            case 3: return;
            case 0: exit(0);
            default: printf("Opcao invalida.\n"); pausar();
        }
    } while (1);
}






