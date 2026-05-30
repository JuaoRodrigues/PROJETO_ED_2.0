#include "tempo.h"
#include "menus.h"
#include "Ficheiros.h"




// CONFIGURAR TEMPO DE SIMULACAO
SimulacaoTempo configurarTempo(Configuracao *config, int temp) {

    SimulacaoTempo st;
    // VERIFICAR AS HORAS DE ABERTURA E DE FECHO DA LOJA
    st.hora_abertura = config->hora_abertura;
    st.hora_fecho    = config->hora_fecho;
    st.tick_atual    = 0;

    /* total de ticks = total de minutos simulados */
    st.ticks_totais = (config->hora_fecho - config->hora_abertura) * 60;

    /* pede ao utilizador a duração real em minutos */
    int duracao_minutos;

    if(temp != 1 && temp != 2)
    {
        printf("  |\n");
        printf("  |Quanto tempo (em minutos) deve demorar a simulacao de 1 dia?\n");
        printf("  |Minutos: ");
        scanf("%d", &duracao_minutos);
        LIMPAR_BUFFER();
    }
    if(temp == 1)   duracao_minutos = 1;
    if(temp == 2)   duracao_minutos = 2;


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
EntradaCliente *prepararEntradas(Supermercado *sm, HashTable *ht, SimulacaoTempo *st, int *total_entradas) {
    int n = (rand() % (MAX_CLIENTES_DIA - MIN_CLIENTES_DIA + 1)) + MIN_CLIENTES_DIA;

    EntradaCliente *entradas = malloc(n * sizeof(EntradaCliente));
    if (!entradas) { *total_entradas = 0; return NULL; }

    int count = 0;
    int tentativas = 0;

    while (count < n && tentativas < n * 10) {
        tentativas++;
        Cliente *c = clienteAleatorio(ht);
        if (!c) continue;

        // verifica se este cliente já foi escolhido
        int repetido = 0;
        for (int j = 0; j < count; j++) {
            if (entradas[j].cliente == c) { repetido = 1; break; }
        }
        if (repetido) continue;
        if (clienteBanido(sm, c->id)) continue;

        int tick_max_entrada = st->ticks_totais - MAX_TEMPO_LOJA;
        entradas[count].cliente      = c;
        entradas[count].tick_entrada = rand() % tick_max_entrada;

        int tempo_loja = (rand() % (MAX_TEMPO_LOJA - MIN_TEMPO_LOJA + 1)) + MIN_TEMPO_LOJA;
        entradas[count].tick_saida   = entradas[count].tick_entrada + tempo_loja;

        if (entradas[count].tick_saida > st->ticks_totais)
            entradas[count].tick_saida = st->ticks_totais;

        count++;
    }

    *total_entradas = count;
    return entradas;
}


// ----------------------------------------------------------------------------------------------------

// LOOP PRINCIPAL
void correrSimulacao(Supermercado *sm) {
    int total_entradas = 0;
    EntradaCliente *entradas = prepararEntradas(sm, &sm->clientes, &sm->st, &total_entradas);
    sm->clientesDia = total_entradas;
    //printf("Clientes previstos para hoje: %d\n\n", total_entradas);
    limpar_ecra();
    printf("\033[?25l");
    // so para quando:
    // 1) passar da hora de fecho
    // 2) não haver clientes na loja
    // 3) não haver clientes na fila
    while (sm->st.tick_atual <= sm->st.ticks_totais || sm->clientes_na_loja.total_na_loja > 0 || filasTotais(sm) > 0)
    {
        int hora, minuto;
        tickParaHora(&sm->st, &hora, &minuto);

        for (int i = 0; i < total_entradas; i++) {
            // so entra na loja se ainda nao passou a hora de fecho  // <-- controlo de entradas
            if (entradas[i].tick_entrada == sm->st.tick_atual && entradas[i].cliente != NULL && sm->st.tick_atual <= sm->st.ticks_totais) {
                entradas[i].cliente->tick_entrada = entradas[i].tick_entrada;
                preencherCarrinho(entradas[i].cliente, sm);
                inserirLoja(&sm->clientes_na_loja, entradas[i]);
                /*
                printf("[%02d:%02d] Cliente %06d (%s) entrou na loja com %d produtos.\n",
                   hora, minuto,
                   entradas[i].cliente->id,
                   entradas[i].cliente->nome,
                   entradas[i].cliente->n_produtos);
                */
                // ESCREVER OS PRODUTOS DE CADA PESSOA
                /*
                Produto *prod = entradas[i].cliente->carrinho;
                while(prod)
                {
                    printf("%s\n", prod->nome);
                    prod = prod->proximo;
                }
                printf("\n");
                */
            }
            // saida da loja acontece sempre, independentemente da hora  // <-- sem restricao de hora
            if (entradas[i].tick_saida == sm->st.tick_atual && entradas[i].cliente != NULL) {
                entradas[i].cliente->tick_saida = entradas[i].tick_saida;
                removerLoja(&sm->clientes_na_loja, sm->st.tick_atual);
                clienteEntrarCaixa(sm, entradas[i].cliente);
                entradas[i].cliente = NULL;  // <-- marca como processado
            }
        }



        processarAtendimento(sm);
        oferecerProduto(sm);
        gerirCaixas(sm, sm->st.tick_atual >= sm->st.ticks_totais && sm->clientes_na_loja.total_na_loja == 0);

        // calcula o tempo de cada caixa aberta
        for (int i = 0; i < sm->config.n_caixas; i++)
        {
            if (sm->caixas[i].ativa != 0)
                sm->caixas[i].ticks_aberta ++;
        }

        // verifica se o utilizador pretende pausar a simulacao
        if (_kbhit()) {
            int tecla = _getch();
            if (tecla == 'p' || tecla == 'P') {
                printf("\033[?25h");        //mostra o cursor
                printf("\n");
                pausarSimulacao(sm);
                while (_kbhit()) _getch();
                printf("\033[?25l");        //esconde o cursor
                limpar_ecra();
            }
        }
        simular(sm, hora, minuto, total_entradas);
        sm->st.tick_atual++;
        Sleep((DWORD)(sm->st.segundos_por_tick * 1000));
    }
    printf("\033[?25h");                // mostra o cursor novamente
    guardarDia(sm);
    printf("\n  Loja fechada. Simulacao terminada.");
    printf("\n  Dia %d guardado em 'historico.txt'.\n\n", sm->dia);
    //pausar();

    menu_fim_simulacao(sm);
    free(entradas);
}



