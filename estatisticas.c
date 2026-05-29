#include "estatisticas.h"





void registar_cliente_atendido (Caixa *cai, Cliente *cli)
{
    NodoClienteAtendido *novo = malloc(sizeof(NodoClienteAtendido));
    if(!novo)   return;
    novo->atendido = *cli;          // copia o cliente inteiro
    novo->atendido.proximo = NULL;  // limpa o ponteiro da fila
    novo->proximo = NULL;

    if(!cai->historico.inicio)
    {
        cai->historico.inicio = novo;
    }else{
        // adiciona ao inicio da lista
        // a lista vai ficar ordenada do mais recente para o mais antigo
        novo->proximo = cai->historico.inicio;
        cai->historico.inicio = novo;
    }
    cai->historico.total ++;
}

void imprimir_historico (Caixa *cai)
{
    printf("\n=== Caixa %d | Total atendidos: %d\n", cai->id, cai->historico.total);

    if(cai->historico.total != 0)
    {
        NodoClienteAtendido *n = cai->historico.inicio;
        while(n)
        {
            printf("  Cliente %06d (%s) | Quantidade de produtos: %d\n", n->atendido.id, n->atendido.nome, n->atendido.n_produtos);
            n = n->proximo;
        }
    }
}


void taxa_oferta (Supermercado *sm)
{
    for(int i = 0; i < sm->config.n_caixas; i++)
    {
        Caixa *cai = &sm->caixas[i];
        if(cai->total_clientes_atendidos == 0)  continue;

        // simplifica a fracao
        // exemplo: 2/6 = 1/6
        int oferecidos = cai->produtos_oferecidos;
        int total =      cai->total_clientes_atendidos;

        // calcula o maximo divisor comum
        int b = total, a = oferecidos;
        while(b != 0)
        {
            int t = b;
            b = a % b;
            a = t;
        }
        int maxDiv = a;
        float percentagem = (float)oferecidos / total * 100.0;

        if (maxDiv == 0 || oferecidos == 0)             printf("  Caixa %d | Nenhum produto oferecido                                " VERDE "0,0%%" RESET "\n", cai->id);
        else {
            if(percentagem >= 50)                       printf("  Caixa %d | %3d em cada %3d clientes recebeu uma oferta             " VERMELHO "%.1f%%" RESET "\n", cai->id, oferecidos / maxDiv, total / maxDiv, percentagem);
            if(percentagem < 50 && percentagem >= 25)   printf("  Caixa %d | %3d em cada %3d clientes recebeu uma oferta             " AMARELO "%.1f%%" RESET "\n", cai->id, oferecidos / maxDiv, total / maxDiv, percentagem);
            if(percentagem < 25)                        printf("  Caixa %d | %3d em cada %3d clientes recebeu uma oferta             " VERDE "%.1f%%" RESET "\n", cai->id, oferecidos / maxDiv, total / maxDiv, percentagem);
        }
    }
    // L -> loja
    int oferecidosL = sm->produtos_oferecidos_total;
    int totalL = sm->clientesDia;

    int b = totalL, a = oferecidosL;
    while(b)
    {
        int t = b;
        b = a % b;
        a = t;
    }
    int maxDivL = a;
    float percentagemL = (float)oferecidosL / totalL * 100.0;
    if (maxDivL == 0 || oferecidosL == 0)               printf("\n  No Supermercado | Nenhum produto oferecido                       " VERDE "0,0%%" RESET "\n");
    if(percentagemL >= 50)                              printf("\n  No Supermercado | %3d em cada %3d clientes recebeu uma oferta    " VERMELHO "%.1f%%" RESET "\n", oferecidosL / maxDivL, totalL / maxDivL, percentagemL);
    if(percentagemL < 50 && percentagemL >= 25)         printf("\n  No Supermercado | %3d em cada %3d clientes recebeu uma oferta    " AMARELO "%.1f%%" RESET "\n", oferecidosL / maxDivL, totalL / maxDivL, percentagemL);
    if(percentagemL < 25)                               printf("\n  No Supermercado | %3d em cada %3d clientes recebeu uma oferta    " VERDE "%.1f%%" RESET "\n", oferecidosL / maxDivL, totalL / maxDivL, percentagemL);
}




void estatisticas_gerais (Supermercado *sm)
{
    // A -> atendidos
    // P -> produtos vendidos
    // R -> rendimento (total vendas em euros)
    // T -> tempo aberta
    // O -> oferecidos
    Caixa *maisA = NULL, *menosA = NULL;
    Caixa *maisP = NULL, *menosP = NULL;
    Caixa *maisR = NULL, *menosR = NULL;
    Caixa *maisT = NULL, *menosT = NULL;
    Caixa *maisO = NULL, *menosO = NULL;

    for(int i = 0; i < sm->config.n_caixas; i++)
    {
        Caixa *cai = &sm->caixas[i];
        if(cai->total_clientes_atendidos == 0)  continue;

        if(!maisA  || cai->total_clientes_atendidos > maisA->total_clientes_atendidos)      maisA  = cai;
        if(!menosA || cai->total_clientes_atendidos < menosA->total_clientes_atendidos)     menosA = cai;

        if(!maisP  || cai->total_produtos_vendidos > maisP->total_produtos_vendidos)        maisP  = cai;
        if(!menosP || cai->total_produtos_vendidos < menosP->total_produtos_vendidos)       menosP = cai;

        if(!maisR  || cai->total_valor_vendido > maisR->total_valor_vendido)                maisR  = cai;
        if(!menosR || cai->total_valor_vendido < menosR->total_valor_vendido)               menosR = cai;

        if(!maisT  || cai->ticks_aberta > maisT->ticks_aberta)                              maisT  = cai;
        if(!menosT || cai->ticks_aberta < menosT->ticks_aberta)                             menosT = cai;

        if(!maisO  || cai->produtos_oferecidos > maisO->produtos_oferecidos)                maisO  = cai;
        if(!menosO || cai->produtos_oferecidos < maisO->produtos_oferecidos)                menosO = cai;
    }
    // A -> abertura
    // F -> fecho
    // E -> esperado

    int horaA = sm->config.hora_abertura,  minA = 0;
    int horaE = sm->config.hora_fecho,     minE = 0;
    int horaF = sm->st.tick_atual / 60 + sm->config.hora_abertura, minF = sm->st.tick_atual % 60 + sm->config.hora_abertura;


    printf("===  Estatísticas Gerais \n\n");

    printf("===| Horas");
    printf("   | Hora abertura : %02dh%02d\n", horaA, minA);
    printf("   | Hora fecho : %02dh%02d\n", horaF, minF);
    printf("   | Hora fecho (esperado): %02dh%02d\n\n", horaE, minE);


    printf("===| Clientes Atendidos \n");
    if (maisA)  printf(VERDE " + " RESET "|  Mais:  Caixa %02d | %7d clientes\n", maisA->id, maisA->total_clientes_atendidos);
    if (menosA) printf(VERMELHO " - " RESET "|  Menos: Caixa %02d | %7d clientes\n", menosA->id, menosA->total_clientes_atendidos);

    printf("\n===| Produtos Vendidos \n");
    if (maisP)  printf(VERDE " + " RESET "|  Mais:  Caixa %02d | %7d produtos\n", maisP->id, maisP->total_produtos_vendidos);
    if (menosP) printf(VERMELHO " - " RESET "|  Menos: Caixa %02d | %7d produtos\n", menosP->id, menosP->total_produtos_vendidos);

    printf("\n===| Produtos Oferecidos \n");
    if (maisP)  printf(VERDE " + " RESET "|  Mais:  Caixa %02d | %7d produtos oferecidos\n", maisO->id, maisO->total_produtos_vendidos);
    if (menosP) printf(VERMELHO " - " RESET "|  Menos: Caixa %02d | %7d produtos oferecidos\n", menosO->id, menosO->total_produtos_vendidos);

    printf("\n===|  Rendimento  \n");
    if(maisR)   printf(VERDE " + " RESET "|  Mais:  Caixa %02d | %9.2f euros\n", maisR->id, maisR->total_valor_vendido);
    if(menosR)  printf(VERMELHO " - " RESET "|  Menos: Caixa %02d | %9.2f euros\n", menosR->id, menosR->total_valor_vendido);

    printf("\n===| Tempo Aberta \n");
    if(maisT)
    {
        int horas = (maisT->ticks_aberta) / 60;
        int mins  = (maisT->ticks_aberta) % 60;
        printf(VERDE " + " RESET"|  Mais:  Caixa %02d | %02dh%02d\n", maisT->id, horas, mins);
    }
    if(menosT)
    {
        int horas = (menosT->ticks_aberta) / 60;
        int mins  = (menosT->ticks_aberta) %60;
        printf(VERMELHO " - " RESET "|  Menos: Caixa %02d | %02dh%02d\n", menosT->id, horas, mins);
    }
}



void estatisticas_clientes(Supermercado *sm)
{
    EstatisticasClientes *e = &sm->est_clientes;
    int total_entraram = sm->clientes.total_clientes;

    printf("\n=== Estatísticas de Clientes\n\n");

    if (e->total_atendidos > 0)
    {
        printf("  | Media de produtos por cliente  : %.1f\n", (float)e->total_produtos / e->total_atendidos);
        printf("  | Media de espera na fila        : %ldm\n", e->total_espera / e->total_atendidos);
        printf("  | Media de tempo na loja         : %ldm\n", e->total_tempo_loja / e->total_atendidos);
        printf("  | Media de dinheiro gasto        : %.2f euros\n", e->total_gasto / e->total_atendidos);
    }

    printf("  | Clientes sem produtos          : %d [%.1f%%]\n",
        e->total_sem_produtos,
        total_entraram > 0 ? (float)e->total_sem_produtos / total_entraram * 100.0f : 0.0f);

    printf("  | Clientes com oferta            : %d [%.1f%%]\n",
        e->total_com_oferta,
        e->total_atendidos > 0 ? (float)e->total_com_oferta / e->total_atendidos * 100.0f : 0.0f);
    printf("  |\n  | Total clientes                 : %d\n", sm->est_clientes.total_atendidos);
}
