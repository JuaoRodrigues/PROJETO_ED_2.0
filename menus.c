#include "menus.h"
#include "supermercado.h"
#include "tempo.h"

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
    printf("  |                 SIMULAÇÃO DIA %d\n", sm->dia);
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

        if(cai->ativa != 0 && cai->ativa != 4)
        {
            printf("  \033[2K| Caixa " VERDE "%d" RESET" : ", cai->id);
            for(int j = 0; j < cai->fila.tamanho; j++){printf("#");}

            int espacos = 40 - cai->fila.tamanho;
            for (int j = 0; j < espacos; j++) printf(" ");
            printf("%d\n", cai->fila.tamanho);

        }else {
            if(cai->ativa == 4)     printf("  | Caixa " VERMELHO "%d" RESET " : "VERMELHO"Fora de Serviço!"RESET"                        0\n", cai->id);
            else printf("  | Caixa " VERMELHO "%d" RESET " : Fechada!                                0\n", cai->id);
        }



    }
    printf("  |------------------------------------------------------\n");
    printf("  | DETALHES\n");
    printf("  | Clientes na Loja    : %02d\n", sm->clientes_na_loja.total_na_loja);
    printf("  | Clientes na Fila    : %02d\n", total_filas);
    printf("  | Clientes Previstos  : %02d\n", previstos);
    printf("  |\n");
    printf("  | 'P' para pausar simulação\n");
    printf("  |------------------------------------------------------\n");
}


// MENUS
/*
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
*/

void menu_caixas(Supermercado *sm)
{
    limpar_ecra();
    int op;
    do{
    printf("  |------------------------------------------|\n");
    printf("  |             GESTÃO DE CAIXAS             |\n");
    printf("  |------------------------------------------|\n");
    printf("  | 1. Listar fila duma caixa                |\n");
    printf("  | 2. Abrir caixa definitivamente           |\n");
    printf("  | 3. Fechar caixa definitivamente          |\n");
    printf("  | 4. Voltar a gestão automárica            |\n");
    printf("  | 5. Gestão automática de caixa específica |\n");
    //printf("  | 6. Mover cliente de caixa                |\n");
    printf("  | 0. Voltar                                |\n");
    printf("  |------------------------------------------|\n");
    printf("  | Opção: ");
    scanf("%d", &op);
    LIMPAR_BUFFER();

    int n;
    switch(op)
    {
    case 1:
        limpar_ecra();
        do{
            printf("\n\n  Número da caixa que pretende listar os clientes: ");
            scanf("%d", &n);
            LIMPAR_BUFFER();
            if(n <= 0 || n > sm->config.n_caixas)
            {
                printf("  Número de caixa inválido! Tente novamente com um valor entre 1 e %d", sm->config.n_caixas);
                continue;
            }
            imprimirFila(sm, n);
            pausar();
        }while (n <= 0 || n > sm->config.n_caixas);
        limpar_ecra();
        break;
    case 2:
        limpar_ecra();
        do{
            printf("\n\n  Número da caixa que pretende abrir: ");
            scanf("%d", &n);
            LIMPAR_BUFFER();
            if(n <= 0 || n > sm->config.n_caixas)
            {
                printf("  Número de caixa inválido! Tente novamente com um valor entre 1 e %d", sm->config.n_caixas);
                continue;
            }
            abrirCaixaDefinitiva(sm, n);
            pausar();
        }while (n <= 0 || n > sm->config.n_caixas);
        limpar_ecra();
        break;

    case 3:
        limpar_ecra();
        do{
            printf("\n\n  Número da caixa que pretende fechar: ");
            scanf("%d", &n);
            LIMPAR_BUFFER();
            if(n <= 0 || n > sm->config.n_caixas)
            {
                printf("  Número de caixa inválido! Tente novamente com um valor entre 1 e %d", sm->config.n_caixas);
                continue;
            }
            fecharCaixaDefinitiva(sm, n);
            pausar();
        }while (n <= 0 || n > sm->config.n_caixas);
        limpar_ecra();
        break;

    case 4:
        caixasAutomaticas(sm, 0);
        pausar();
        limpar_ecra();
        break;

    case 5:
        do{
            printf("\n\n  Número da caixa que pretende: ");
            scanf("%d", &n);
            LIMPAR_BUFFER();
            if(n < 0 || n > sm->config.n_caixas)
                printf("  Número de caixa inválido! Tente novamente com um valor entre 1 e %d", sm->config.n_caixas);
        }while(n < 0 || n > sm->config.n_caixas);
        caixasAutomaticas(sm, n);
        pausar();
        limpar_ecra();
        break;
    case 0:
        limpar_ecra(); break;
    default: printf("  Opcao inválida.\n"); pausar();
    }
    limpar_ecra();

    }while(op != 0);
}


void menu_clientes(Supermercado *sm)
{
    limpar_ecra();
    int op;
    int id;

    do{
    printf("  |------------------------------------------|\n");
    printf("  |             GESTÃO DE CLIENTES           |\n");
    printf("  |------------------------------------------|\n");
    printf("  | 1. Pesquisar cliente                     |\n");
    printf("  | 2. Mover cliente para outra caixa        |\n");
    printf("  | 3. Banir Cliente                         |\n");
    printf("  | 4. Desbanir Cliente                      |\n");
    printf("  | 5. Listar Cliente Banidos                |\n");
    printf("  | 0. Voltar                                |\n");
    printf("  |------------------------------------------|\n");
    printf("  | Opção: ");
    scanf("%d", &op);
    LIMPAR_BUFFER();

    switch(op)
    {
    case 1:
        limpar_ecra();
        do
        {
            printf("\n  Indique o ID do cliente que pretende pesquisar");
            printf("\n  ID: ");
            scanf("%d", &id);
            LIMPAR_BUFFER();
            if(id <= 0 || id > 999999)   {printf("\n\n  ID inválido!\n  Tente um valor superior a 0 com, no máximo 6 algarismos!"); continue;}
            pesquisarCliente(sm, id);
        }while (id <= 0 || id > 999999);     // todos os IDS tem um maximo de 6 algarismos
        pausar();
        limpar_ecra();
        break;
    case 2:
        limpar_ecra();
        int origem;
        do{
            printf("\n  De qual caixa pretende mover o cliente?\n");
            printf("  Caixa: ");
            scanf("%d", &origem);
            LIMPAR_BUFFER();
            if (origem <= 0 || origem > sm->config.n_caixas)
            {
                printf("\n  Número de caixa inválido!");
                printf("\n  Tente um valor entre 1 e %d", sm->config.n_caixas);
            }
        }while(origem <= 0 || origem > sm->config.n_caixas);
        moverClientePorId(sm, origem);
        pausar();
        limpar_ecra();
        break;
    case 3:
        limpar_ecra();
        do
        {
            printf("\n  Indique o ID do cliente que pretende banir.");
            printf("\n  ID: ");
            scanf("%d", &id);
            LIMPAR_BUFFER();
            if(id<=0 || id > 999999)    {printf("\n\n  ID inválido!\n  Tente um valor superior a 0 com, no máximo 6 algarismos!"); continue;}
            banirCliente(sm, id);
        }while(id<=0 || id > 999999);       // todos os IDS tem um maximo de 6 algarismos
        pausar();
        limpar_ecra();
        break;

    case 4:
        limpar_ecra();
        int ban;
        listarBanidos(sm);
        if(sm->banidos.total != 0)
        {
            do
            {
                printf("\n\n  Pretende desbanir algum dos clientes banidos?");
                printf("\n  1. Sim");
                printf("\n  2. Não");
                printf("\n  Resposta: ");
                scanf("%d", &ban);
                if(ban != 1 && ban != 2)    {printf("\n  Opção inválida!\n  Tente novamente"); continue;}
            }while (ban != 1 && ban != 2);

            if(ban == 1)
            {
               do
               {
                    printf("\n  Indique o ID do cliente que pretende desbanir.");
                    printf("\n  ID: ");
                    scanf("%d", &id);
                    LIMPAR_BUFFER();
                    if(id<=0 || id > 999999)    {printf("\n\n  ID inválido!\n  Tente um valor superior a 0 com, no máximo 6 algarismos!"); continue;}
                    desbanirCliente(sm, id);
               }while (id<=0 || id > 999999);       // todos os IDS tem um maximo de 6 algarismos
            }
        }
        pausar();
        limpar_ecra();
        break;
    case 0: break;
        default: printf("  Opcao inválida.\n"); pausar();
    }
    limpar_ecra();

    }while(op != 0);
}

void menu_estatisticas_anteriores(Supermercado *sm)
{
    limpar_ecra();
    int op;
    do{
    printf("  |------------------------------------------|\n");
    printf("  |                 ESTATISTICAS             |\n");
    printf("  |------------------------------------------|\n");
    printf("  | 1. Estatísticas gerais                   |\n");
    printf("  | 2. Taxa de oferta de produtos            |\n");
    printf("  | 3. Listar funcionários                   |\n");
    printf("  | 4. Estatísticas dos clientes             |\n");
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
        printf("\n=== Todos os funcionários do Supermercado\n");
        for(int i = 0; i < sm->config.n_caixas; i++) { printf("CAIXA %d : %d - %s\n", i+1, sm->caixas[i].operador_id, sm->caixas[i].operador_nome); }
        pausar();
        limpar_ecra();
        break;
    case 4:
        limpar_ecra();
        estatisticas_clientes(sm);
        pausar();
        limpar_ecra();
        break;
    case 0: limpar_ecra(); break;
        default:  printf("  Opcao inválida.\n"); pausar();
    }
    //limpar_ecra();

    }while (op != 0);
    limpar_ecra();
}




void menu_estatisticas(Supermercado *sm)
{
    limpar_ecra();
    int op;
    int n;
    do{
    printf("  |------------------------------------------|\n");
    printf("  |                 ESTATISTICAS             |\n");
    printf("  |------------------------------------------|\n");
    printf("  | 1. Estatísticas gerais                   |\n");
    printf("  | 2. Taxa de oferta de produtos            |\n");
    printf("  | 3. Listar histórico de uma caixa         |\n");
    printf("  | 4. Listar fila de uma caixa              |\n");
    printf("  | 5. Listar funcionários                   |\n");
    printf("  | 6. Estatísticas dos clientes             |\n");
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
        do
        {
            printf("\nIndique a caixa: ");
            scanf("%d", &n);
            LIMPAR_BUFFER();
            if(n <= 0 || n > sm->config.n_caixas)    {printf("\n  Número de caixa inválido! Tente um valor entre 1 e %d\n", sm->config.n_caixas); continue;}
            imprimirFila(sm, n);
            pausar();
        }while (n <= 0 || n > sm->config.n_caixas);
        limpar_ecra();
        break;
    case 5:
        limpar_ecra();
        printf("\n=== Todos os funcionários do Supermercado\n");
        for(int i = 0; i < sm->config.n_caixas; i++) { printf("CAIXA %d : %d - %s\n", i+1, sm->caixas[i].operador_id, sm->caixas[i].operador_nome); }
        pausar();
        limpar_ecra();
        break;
    case 6:
        limpar_ecra();
        estatisticas_clientes(sm);
        pausar();
        limpar_ecra();
        break;
    case 0: limpar_ecra(); break;
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
        printf("  | 2. Gerir Clientes                        |\n");
        printf("  | 3. Ver estatisticas do momento           |\n");
        printf("  | 4. Retomar simulacao                     |\n");
        printf("  | 0. Cancelar simulacao                    |\n");
        printf("  |------------------------------------------|\n");
        printf("  | Opcao: ");
        scanf("%d", &op);
        LIMPAR_BUFFER();

        switch (op) {
            case 1:
                limpar_ecra();
                menu_caixas(sm);
                limpar_ecra();
                break;

            case 2:
                limpar_ecra();
                menu_clientes(sm);
                break;

            case 3:
                limpar_ecra();
                menu_estatisticas(sm);
                break;

            case 4: return;
            case 0:
                limpar_ecra();
                iniciarDia(sm);
                sm->dia--;

                menu_inicial(sm);
                break;

            default: printf("Opcao invalida.\n"); pausar();
        }
    } while (op != 4 || op != 0);
}


void menu_fim_simulacao (Supermercado *sm)
{
    int op;
    do
    {
        printf("  |------------------------------------------|\n");
        printf("  |            SIMULAÇÃO TERMINADA           |\n");
        printf("  |------------------------------------------|\n");
        printf("  | 1. Ver estatísticas do dia               |\n");
        printf("  | 2. Voltar ao início                      |\n");
        printf("  |------------------------------------------|\n");
        printf("  | Opcao: ");
        scanf("%d", &op);
        LIMPAR_BUFFER();

        switch (op)
        {
        case 1:
            limpar_ecra();
            menu_estatisticas(sm);
            break;
        case 2:
            limpar_ecra();
            menu_inicial(sm);
            break;

            default: printf("Opção inválida.\n"); pausar();
        }
    }while (op != 1 || op!= 2);
}



void menu_configurar_tempo(Supermercado *sm)
{
    int op;
    do{
        printf("  |------------------------------------------|\n");
        printf("  |             CONFIGURAR TEMPO             |\n");
        printf("  |------------------------------------------|\n");
        printf("  | 1. Ver simulação em 1 minuto real        |\n");
        printf("  | 2. Ver simulação em 2 minutos reais      |\n");
        printf("  | 3. Escolher um tempo                     |\n");
        printf("  | 0. Cancelar                              |\n");
        printf("  |------------------------------------------|\n");
        printf("  | Opcao: ");
        scanf("%d", &op);
        LIMPAR_BUFFER();

        switch (op)
        {
        case 1:
            sm->st = configurarTempo(&sm->config, 1);
            correrSimulacao(sm);

        case 2:
            sm->st = configurarTempo(&sm->config, 2);
            correrSimulacao(sm);
        case 3:
            sm->st = configurarTempo(&sm->config, 0);
            correrSimulacao(sm);
        case 0:
            limpar_ecra();
            menu_inicial(sm);
            break;
        default: printf("Opção inválida.\n"); pausar();

        }

    }while(op != 0);

}



void menu_inicial (Supermercado *sm)
{
    int op;
    Supermercado sm_temp = {0};
    iniciarDia(sm);
    do
    {
        printf("  |------------------------------------------|\n");
        printf("  |                   INÍCIO                 |\n");
        printf("  |------------------------------------------|\n");
        printf("  | 1. Simular dia %3d                       |\n", sm->dia);
        printf("  | 2. Ver estatísticas do dia anterior      |\n");
        printf("  | 3. Ver estatísticas dum dia especifico   |\n");
        //printf("  | 4. Gerir caixas antes da simulação       |\n");
        //printf("  | 5. Gerir clientes antes da simulação     |\n");
        printf("  | 0. Terminar programa                     |\n");     // USAR exit(0)
        printf("  |------------------------------------------|\n");
        printf("  | Opcao: ");
        scanf("%d", &op);
        LIMPAR_BUFFER();

        switch(op)
        {
        case 1:
            limpar_ecra();
            menu_configurar_tempo(sm);
            break;
        case 2:
            int dia_anterior = sm->dia - 1;
            if (dia_anterior < 1)
                printf("\n  Nao existe dia anterior.\n");
            else if (carregarDia(dia_anterior, &sm_temp))
                menu_estatisticas_anteriores(&sm_temp);
            else {printf("\n  Dia %d nao encontrado no historico.\n", dia_anterior); pausar();}
            limpar_ecra();
            break;
        case 3:
            int dia_pretendido;
            printf("\n  Qual o dia que pretende consultar? ");
            scanf("%d", &dia_pretendido);
            LIMPAR_BUFFER();
            limpar_ecra();
            if (carregarDia(dia_pretendido, &sm_temp))
                menu_estatisticas_anteriores(&sm_temp);
            else{printf("\n  Dia %d não encontrado no histórico.\n", dia_pretendido); pausar();}
            limpar_ecra();
            break;
        /*
        case 4:
            printf("\nDESENVOLVER");
        case 5:
            printf("\nDESENVOLVER");
        */
        case 0:
            // guardar dados em ficheiro FALTA
            libertarMemoria(sm);
            limpar_ecra();
            printf("\n  A terminar programa!");
            exit(0);
        default:    printf("  |\n  |Opção inválida.\n"); pausar(); limpar_ecra();

        }
    }while(op != 0);
}
