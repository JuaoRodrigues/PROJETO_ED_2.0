#include "tempo.h"


// CONFIGURAR TEMPO DE SIMULACAO
SimulacaoTempo configurarTempo(Configuracao *config) {

    SimulacaoTempo st;
    // VERIFICAR AS HORAS DE ABERTURA E DE FECHO DA LOJA
    st.hora_abertura = config->hora_abertura;
    st.hora_fecho    = config->hora_fecho;
    st.tick_atual    = 0;

    /* total de ticks = total de minutos simulados */
    st.ticks_totais = (config->hora_fecho - config->hora_abertura) * 60;

    /* pede ao utilizador a duração real em minutos */
    int duracao_minutos;
    printf("Quanto tempo (em minutos) deve demorar a simulacao de 1 dia?\n");
    printf("Minutos: ");
    scanf("%d", &duracao_minutos);
    LIMPAR_BUFFER();

    /* converte para segundos e divide pelos ticks */
    double duracao_segundos = duracao_minutos * 60.0;
    st.segundos_por_tick = duracao_segundos / st.ticks_totais;

    printf("\nSimulacao configurada:\n");
    printf("  Loja aberta: %dh - %dh\n", st.hora_abertura, st.hora_fecho);
    printf("  Ticks totais: %d (1 tick = 1 minuto simulado)\n", st.ticks_totais);
    printf("  Tempo real por tick: %.4f segundos\n\n", st.segundos_por_tick);

    return st;
}


// CONVERTER TICKS EM HORAS SIMULADAS
void tickParaHora(SimulacaoTempo *st, int *hora, int *minuto) {
    int minutos_totais = st->hora_abertura * 60 + st->tick_atual;
    *hora   = minutos_totais / 60;
    *minuto = minutos_totais % 60;
}


// ESCOLHER UM CLIENTE ALEATORIAMENTE NO HASHTABLE PARA ENTRAR NA LOJA
Cliente *clienteAleatorio(HashTable *ht) {
        int bucket = rand() % HASH_SIZE;

        if (ht->buckets[bucket] != NULL) {
            /* percorre a lista do bucket e escolhe um nodo aleatorio */
            int count = 0;
            NodoHash *nodo = ht->buckets[bucket];
            while (nodo) { count++; nodo = nodo->proximo; }     // faz a contagem de elementos do bucket

            int escolha = rand() % count;   // escolhe um aleatorio do total do bucket
            nodo = ht->buckets[bucket];
            for (int i = 0; i < escolha; i++) nodo = nodo->proximo;

            return nodo->cliente;
        }
    return NULL;
}


// PREPARAR AS ENTRADAS
EntradaCliente *prepararEntradas(HashTable *ht, SimulacaoTempo *st, int *total_entradas) {
    int n = (rand() % (MAX_CLIENTES_DIA - MIN_CLIENTES_DIA + 1)) + MIN_CLIENTES_DIA;

    EntradaCliente *entradas = malloc(n * sizeof(EntradaCliente));
    if (!entradas) { *total_entradas = 0; return NULL; }

    for (int i = 0; i < n; i++) {
        entradas[i].cliente      = clienteAleatorio(ht);
        entradas[i].tick_entrada = rand() % st->ticks_totais;

        int tempo_loja = (rand() % (MAX_TEMPO_LOJA - MIN_TEMPO_LOJA + 1)) + MIN_TEMPO_LOJA;
        entradas[i].tick_saida   = entradas[i].tick_entrada + tempo_loja;

        // garante que nao sai depois do fecho
        if (entradas[i].tick_saida > st->ticks_totais)
            entradas[i].tick_saida = st->ticks_totais;
    }

    *total_entradas = n;
    return entradas;
}


// ----------------------------------------------------------------------------------------------------

// LOOP PRINCIPAL

void correrSimulacao(Supermercado *sm) {
    int total_entradas = 0;
    EntradaCliente *entradas = prepararEntradas(&sm->clientes, &sm->st, &total_entradas);

    printf("Clientes previstos para hoje: %d\n\n", total_entradas);

    /*
    struct timespec ts;
    ts.tv_sec  = (time_t)sm->st.segundos_por_tick;
    ts.tv_nsec = (long)((sm->st.segundos_por_tick - ts.tv_sec) * 1e9);
    */

    while (sm->st.tick_atual < sm->st.ticks_totais) {
        int hora, minuto;
        tickParaHora(&sm->st, &hora, &minuto);

        // verifica entradas neste tick
        for (int i = 0; i < total_entradas; i++) {
            if (entradas[i].tick_entrada == sm->st.tick_atual && entradas[i].cliente != NULL) {
                preencherCarrinho(entradas[i].cliente, sm);
                inserirLoja(&sm->clientes_na_loja, entradas[i]);
                printf("[%02d:%02d] Cliente %06d (%s) entrou na loja com %d produtos.\n",
                       hora, minuto,
                       entradas[i].cliente->id,
                       entradas[i].cliente->nome,
                       entradas[i].cliente->n_produtos);

                // ESCREVER OS PRODUTOS DE CADA PESSOA
                Produto *prod = entradas[i].cliente->carrinho;
                while(prod)
                {
                    printf("%s\n", prod->nome);
                    prod = prod->proximo;
                }
                printf("\n");

            }

            if (entradas[i].tick_saida == sm->st.tick_atual && entradas[i].cliente != NULL) {
                removerLoja(&sm->clientes_na_loja, sm->st.tick_atual);
                clienteEntrarCaixa(sm, entradas[i].cliente);
            }
        }

        gerirCaixas(sm);

        sm->st.tick_atual++;

        Sleep((DWORD)(sm->st.segundos_por_tick * 1000));
    }

    printf("\nLoja fechada. Simulacao terminada.\n");
    free(entradas);
}


// ----------------------------------------------------------------------------------------------------------------





