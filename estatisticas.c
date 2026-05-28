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


void imprimir_historico (Supermercado *sm)
{
    for(int i = 0; i < sm->config.n_caixas; i++)
    {
        Caixa *cai = &sm->caixas[i];
        if(cai->historico.total == 0)  continue;

        printf("\n=== Caixa %d | Total atendidos: %d ===\n", cai->id, cai->historico.total);

        NodoClienteAtendido *n = cai->historico.inicio;
        while(n)
        {
            printf("  Cliente %06d (%s) | Produtos: %d\n", n->atendido.id, n->atendido.nome, n->atendido.n_produtos);
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

        if (maxDiv == 0 || oferecidos == 0)     printf("  Caixa %d | nenhum produto oferecido\n", cai->id);
        else printf("  Caixa %d | %d em cada %d clientes recebeu uma oferta\n", cai->id, oferecidos / maxDiv, total / maxDiv);
    }
}




void estatisticas_gerais (Supermercado *sm)
{
    // A -> atendidos
    // P -> produtos vendidos
    // R -> rendimento (total vendas em euros)
    // T -> tempo aberta
    Caixa *maisA = NULL, *menosA = NULL;
    Caixa *maisP = NULL, *menosP = NULL;
    Caixa *maisR = NULL, *menosR = NULL;
    Caixa *maisT = NULL, *menosT = NULL;

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
    }

    printf("=== Clientes Atendidos ===\n");
    if (maisA)  printf("  Mais:  Caixa %d | %d clientes\n", maisA->id, maisA->total_clientes_atendidos);   // <---
    if (menosA) printf("  Menos: Caixa %d | %d clientes\n", menosA->id, menosA->total_clientes_atendidos); // <---

    printf("\n=== Produtos Vendidos ===\n");
    if (maisP)  printf("  Mais:  Caixa %d | %d produtos\n", maisP->id, maisP->total_produtos_vendidos);
    if (menosP) printf("  Menos: Caixa %d | %d produtos\n", menosP->id, menosP->total_produtos_vendidos);

    printf("\n===  Rendimento  ===\n");
    if(maisR)   printf("  Mais:  Caixa %d | %.2f euros\n", maisR->id, maisR->total_valor_vendido);
    if(menosR)  printf("  Menos: Caixa %d | %.2f euros\n", menosR->id, menosR->total_valor_vendido);

    printf("\n=== Tempo Aberta ===\n");
    if(maisT)
    {
        int horas = (maisT->ticks_aberta) / 60;
        int mins  = (maisT->ticks_aberta) % 60;
        printf("  Mais:  Caixa %d | %02dh%02d\n", maisT->id, horas, mins);
    }
    if(menosT)
    {
        int horas = (menosT->ticks_aberta) / 60;
        int mins  = (menosT->ticks_aberta) %60;
        printf("  Menos: Caixa %d | %02dh%02d\n", menosT->id, horas, mins);
    }
}

