#include "Ficheiros.h"

/* ------------------------------ LEITURA DE FICHEIROS ------------------------------*/


int lerConfiguracao(const char *ficheiro, Configuracao *config)
{
    FILE *f = fopen(ficheiro, "r");
    if (!f) {
        printf("Erro: nao foi possivel abrir %s\n", ficheiro);
        return 0;
    }

    char chave[50];
    while (fscanf(f, "%s", chave) == 1) {
        if      (strcmp(chave, "MAX_ESPERA")                == 0) fscanf(f, "%d", &config->max_espera);
        else if (strcmp(chave, "N_CAIXAS")                  == 0) fscanf(f, "%d", &config->n_caixas);
        else if (strcmp(chave, "TEMPO_ATENDIMENTO_PRODUTO") == 0) fscanf(f, "%d", &config->tempo_atendimento_produto);
        else if (strcmp(chave, "MAX_PRECO")                 == 0) fscanf(f, "%f", &config->max_preco);
        else if (strcmp(chave, "MAX_FILA")                  == 0) fscanf(f, "%d", &config->max_fila);
        else if (strcmp(chave, "MIN_FILA")                  == 0) fscanf(f, "%d", &config->min_fila);
        else if (strcmp(chave, "HORA_ABERTURA")             == 0) fscanf(f, "%d", &config->hora_abertura);
        else if (strcmp(chave, "HORA_FECHO")                == 0) fscanf(f, "%d", &config->hora_fecho);
    }
    fclose(f);
    return 1;
}


void lerClientes(const char *ficheiro, HashTable *ht) {
    FILE *f = fopen(ficheiro, "r");
    if (!f) {
        printf("Erro: nao foi possivel abrir %s\n", ficheiro);
        return;
    }

    char linha[256];
    while (fgets(linha, sizeof(linha), f)) {
        if (strlen(linha) <= 1) continue;
        linha[strcspn(linha, "\n")] = '\0';

        char *tab = strchr(linha, '\t');
        if (!tab) continue;
        *tab = '\0';

        int id = atoi(linha);
        char *nome = tab + 1;

        Cliente *c = malloc(sizeof(Cliente));
        if (!c) continue;
        c->id                = id;
        strncpy(c->nome, nome, MAX_NOME - 1);
        c->nome[MAX_NOME - 1] = '\0';
        c->n_produtos         = 0;
        c->carrinho           = NULL;
        c->tick_entrada_fila   = -1;
        c->produto_oferecido  = 0;
        c->proximo            = NULL;

        int bucket = id % HASH_SIZE;
        NodoHash *nodo = malloc(sizeof(NodoHash));
        if (!nodo) { free(c); continue; }
        nodo->id_cliente    = id;
        nodo->cliente       = c;
        nodo->indice_caixa  = -1;
        nodo->proximo       = ht->buckets[bucket];
        ht->buckets[bucket] = nodo;
        ht->total_clientes++;
    }

    fclose(f);
}




Produto *lerProdutos(const char *ficheiro, int *total, int tempo_max) {
    FILE *f = fopen(ficheiro, "r");
    if (!f) {
        printf("Erro: nao foi possivel abrir %s\n", ficheiro);
        *total = 0;
        return NULL;
    }

    int count = 0;
    char linha[512];
    while (fgets(linha, sizeof(linha), f)) {
        if (strlen(linha) > 1) count++;
    }

    Produto *produtos = malloc(count * sizeof(Produto));
    if (!produtos) {
        fclose(f);
        *total = 0;
        return NULL;
    }

    rewind(f);
    int i = 0;
    while (i < count && fgets(linha, sizeof(linha), f)) {
        if (strlen(linha) <= 1) continue;
        linha[strcspn(linha, "\n")] = '\0';

        char *token = strtok(linha, "\t");
        if (!token) continue;
        produtos[i].id = atoi(token);

        token = strtok(NULL, "\t");
        if (!token) continue;
        strncpy(produtos[i].nome, token, MAX_NOME - 1);
        produtos[i].nome[MAX_NOME - 1] = '\0';

        token = strtok(NULL, "\t");
        if (!token) continue;
        produtos[i].preco = atof(token);

        produtos[i].tempo_passagem = (rand() % (tempo_max - 2 + 1)) + 2;
        produtos[i].proximo = NULL;

        i++;
    }

    fclose(f);
    *total = i;
    return produtos;
}



/*
void lerDados(const char *ficheiro, Supermercado *sm) {
    FILE *f = fopen(ficheiro, "r");
    if (!f) {
        printf("Erro: nao foi possivel abrir %s\n", ficheiro);
        return;
    }

    char linha[512];

    // Ignorar linhas de comentário e ler número de caixas
    int n_caixas = 0;
    while (fgets(linha, sizeof(linha), f)) {
        if (linha[0] == '/' || strlen(linha) <= 1) continue;
        sscanf(linha, "%d", &n_caixas);
        break;
    }

    for (int i = 0; i < n_caixas; i++) {
        // Ler linha da caixa: "CaixaN : ESTADO"
        while (fgets(linha, sizeof(linha), f)) {
            if (linha[0] != '/' && strlen(linha) > 1) break;
        }
        char nome_caixa[MAX_NOME];
        int ativa;
        sscanf(linha, "%s : %d", nome_caixa, &ativa);

        sm->caixas[i].id    = i + 1;
        strncpy(sm->caixas[i].nome, nome_caixa, MAX_NOME - 1);
        sm->caixas[i].ativa = ativa;
        sm->caixas[i].fila.frente  = NULL;
        sm->caixas[i].fila.fim     = NULL;
        sm->caixas[i].fila.tamanho = 0;
        sm->caixas[i].total_clientes_atendidos = 0;
        sm->caixas[i].total_produtos_vendidos  = 0;
        sm->caixas[i].total_valor_vendido      = 0.0f;
        sm->caixas[i].produtos_oferecidos      = 0;
        sm->caixas[i].valor_oferecido          = 0.0f;
        sm->caixas[i].tick_fim_atendimento = sm->config.hora_abertura * 3600;

        // Ler operador: "OPERADOR : NomeOperador"
        while (fgets(linha, sizeof(linha), f)) {
            if (linha[0] != '/' && strlen(linha) > 1) break;
        }
        char operador_nome[MAX_NOME];
        sscanf(linha, "OPERADOR : %s", operador_nome);
        strncpy(sm->caixas[i].operador_nome, operador_nome, MAX_NOME - 1);

        // Ler número de clientes
        while (fgets(linha, sizeof(linha), f)) {
            if (linha[0] != '/' && strlen(linha) > 1) break;
        }
        int n_clientes;
        sscanf(linha, "%d", &n_clientes);

        // Ler cada cliente
        for (int j = 0; j < n_clientes; j++) {
            while (fgets(linha, sizeof(linha), f)) {
                if (linha[0] != '/' && strlen(linha) > 1) break;
            }
            linha[strcspn(linha, "\n")] = '\0';

            Cliente *c = malloc(sizeof(Cliente));
            if (!c) continue;
            c->n_produtos        = 0;
            c->carrinho          = NULL;
            c->tick_entrada_fila = sm->config.hora_abertura * 3600;
            c->produto_oferecido = 0;
            c->proximo           = NULL;
            c->id                = 0;

            // Ler "NomeCliente : N_PRODUTOS  PROD1 PROD2 ..."
            char *token = strtok(linha, " ");
            strncpy(c->nome, token, MAX_NOME - 1);

            strtok(NULL, " ");  /* salta o ":"

            token = strtok(NULL, " ");
            int n_prod = atoi(token);
            c->n_produtos = n_prod;

            // Ler produtos do carrinho
            Produto *ultimo = NULL;
            for (int k = 0; k < n_prod; k++) {
                token = strtok(NULL, " ");
                if (!token) break;

                Produto *p = malloc(sizeof(Produto));
                if (!p) continue;
                strncpy(p->nome, token, MAX_NOME - 1);
                p->nome[MAX_NOME - 1] = '\0';
                p->id             = 0;
                p->preco          = 0.0f;  // desconhecido nos dados iniciais
                p->tempo_passagem = (rand() % (sm->config.tempo_atendimento_produto - 2 + 1)) + 2;
                p->proximo        = NULL;

                if (c->carrinho == NULL) {
                    c->carrinho = p;
                } else {
                    ultimo->proximo = p;
                }
                ultimo = p;
            }

            // Enqueue na caixa
            if (sm->caixas[i].fila.fim == NULL) {
                sm->caixas[i].fila.frente = c;
                sm->caixas[i].fila.fim    = c;
            } else {
                sm->caixas[i].fila.fim->proximo = c;
                sm->caixas[i].fila.fim          = c;
            }
            sm->caixas[i].fila.tamanho++;
        }
    }

    fclose(f);
}
*/

void atribuirFuncionarios(const char *ficheiro, Supermercado *sm)
{
    FILE *f = fopen(ficheiro, "r");
    if (!f) {
        printf("Erro: nao foi possivel abrir %s\n", ficheiro);
        return;
    }

    // contar quantos funcionarios existem
    int total = 0;
    char linha[256];
    while (fgets(linha, sizeof(linha), f))
        if (strlen(linha) > 1) total++;

    if (total == 0) { fclose(f); return; }

    // guardar todos os funcionarios em arrays temporarios
    int *ids          = malloc(total * sizeof(int));
    char (*nomes)[MAX_NOME] = malloc(total * sizeof(*nomes));
    if (!ids || !nomes) { fclose(f); free(ids); free(nomes); return; }

    rewind(f);
    int i = 0;
    while (i < total && fgets(linha, sizeof(linha), f))
    {
        if (strlen(linha) <= 1) continue;
        linha[strcspn(linha, "\n")] = '\0';

        char *espaco = strchr(linha, '\t');
        if (!espaco) continue;
        *espaco = '\0';

        ids[i] = atoi(linha);
        strncpy(nomes[i], espaco + 1, MAX_NOME - 1);
        nomes[i][MAX_NOME - 1] = '\0';
        i++;
    }
    fclose(f);

    // atribuir um funcionario aleatorio a cada caixa
    // sem repeticoes se houver funcionarios suficientes
    int *usados = calloc(i, sizeof(int));
    if (!usados) { free(ids); free(nomes); return; }

    for (int c = 0; c < sm->config.n_caixas; c++)
    {
        int escolha, tentativas = 0;
        do {
            escolha = rand() % i;
            tentativas++;
        } while (usados[escolha] && tentativas < i);

        usados[escolha] = 1;
        sm->caixas[c].operador_id = ids[escolha];
        strncpy(sm->caixas[c].operador_nome, nomes[escolha], MAX_NOME - 1);
    }

    free(ids);
    free(nomes);
    free(usados);
}



int obterProximoDia()
{
    FILE *f = fopen("historico.txt", "r");
    if (!f) return 1;

    char linha[512];
    int ultimo_dia = 0;

    while (fgets(linha, sizeof(linha), f))
    {
        int dia;
        if (sscanf(linha, "DIA %d", &dia) == 1)
            ultimo_dia = dia;
    }

    fclose(f);
    return (ultimo_dia == 0) ? 1 : ultimo_dia + 1;
}


// --------------------------------------------------------------------


void guardarDia(Supermercado *sm)
{
    FILE *f = fopen("historico.txt", "a");
    if (!f)
    {
        printf("Erro ao abrir historico.txt\n");
        return;
    }
    fprintf(f, "DIA %d\n", sm->dia);
    fprintf(f, "n_caixas|%d\n", sm->config.n_caixas);
    fprintf(f, "produtos_oferecidos_total|%d\n", sm->produtos_oferecidos_total);
    fprintf(f, "valor_total_ganho|%.2f\n", sm->valor_total_ganho);
    fprintf(f, "valor_oferecido_total|%.2f\n", sm->valor_oferecido_total);
    fprintf(f, "clientes_dia|%d\n", sm->clientesDia);
    fprintf(f, "est_total_atendidos|%d\n", sm->est_clientes.total_atendidos);       // <-
    fprintf(f, "est_total_sem_produtos|%d\n", sm->est_clientes.total_sem_produtos); // <-
    fprintf(f, "est_total_com_oferta|%d\n", sm->est_clientes.total_com_oferta);     // <-
    fprintf(f, "est_total_produtos|%d\n", sm->est_clientes.total_produtos);         // <-
    fprintf(f, "est_total_espera|%ld\n", sm->est_clientes.total_espera);            // <-
    fprintf(f, "est_total_tempo_loja|%ld\n", sm->est_clientes.total_tempo_loja);    // <-
    fprintf(f, "est_total_gasto|%.2f\n", sm->est_clientes.total_gasto);             // <-
    for (int i = 0; i < sm->config.n_caixas; i++)
    {
        Caixa *cai = &sm->caixas[i];
        fprintf(f, "CAIXA|%d|%s|%d|%d|%.2f|%d|%.2f|%d\n",
            cai->id,
            cai->operador_nome,
            cai->total_clientes_atendidos,
            cai->total_produtos_vendidos,
            cai->total_valor_vendido,
            cai->produtos_oferecidos,
            cai->valor_oferecido,
            cai->ticks_aberta);
    }
    fprintf(f, "FIM_DIA\n\n");
    fclose(f);
}


float atof_virgula(const char *str)
{
    char copia[64];
    strncpy(copia, str, sizeof(copia) - 1);
    copia[sizeof(copia) - 1] = '\0';
    for (int i = 0; copia[i]; i++)
        if (copia[i] == ',') copia[i] = '.';
    return (float)atof(copia);
}

int carregarDia(int dia_pretendido, Supermercado *sm_temp)
{
    FILE *f = fopen("historico.txt", "r");
    if (!f) return 0;
    char linha[512];
    int dia_atual  = -1;
    int encontrado = 0;
    int idx        = 0;
    // limpa o sm_temp
    memset(sm_temp, 0, sizeof(Supermercado));
    while (fgets(linha, sizeof(linha), f))
    {
        linha[strcspn(linha, "\n")] = '\0';
        if (sscanf(linha, "DIA %d", &dia_atual) == 1)
        {
            encontrado = (dia_atual == dia_pretendido);
            if (encontrado) sm_temp->dia = dia_atual;
            continue;
        }
        if (!encontrado) continue;
        if (strcmp(linha, "FIM_DIA") == 0) break;
        char copia[512];
        strcpy(copia, linha);
        char *tok = strtok(copia, "|");
        if (strcmp(tok, "n_caixas") == 0)
            sm_temp->config.n_caixas = atoi(strtok(NULL, "|"));
        else if (strcmp(tok, "produtos_oferecidos_total") == 0)
            sm_temp->produtos_oferecidos_total = atoi(strtok(NULL, "|"));
        else if (strcmp(tok, "valor_total_ganho") == 0)
            sm_temp->valor_total_ganho = atof_virgula(strtok(NULL, "|"));
        else if (strcmp(tok, "valor_oferecido_total") == 0)
            sm_temp->valor_oferecido_total = atof_virgula(strtok(NULL, "|"));
        else if (strcmp(tok, "clientes_dia") == 0)
            sm_temp->clientesDia = atoi(strtok(NULL, "|"));
        else if (strcmp(tok, "est_total_atendidos") == 0)                                           // <-
            sm_temp->est_clientes.total_atendidos = atoi(strtok(NULL, "|"));                        // <-
        else if (strcmp(tok, "est_total_sem_produtos") == 0)                                        // <-
            sm_temp->est_clientes.total_sem_produtos = atoi(strtok(NULL, "|"));                     // <-
        else if (strcmp(tok, "est_total_com_oferta") == 0)                                          // <-
            sm_temp->est_clientes.total_com_oferta = atoi(strtok(NULL, "|"));                       // <-
        else if (strcmp(tok, "est_total_produtos") == 0)                                            // <-
            sm_temp->est_clientes.total_produtos = atoi(strtok(NULL, "|"));                         // <-
        else if (strcmp(tok, "est_total_espera") == 0)                                              // <-
            sm_temp->est_clientes.total_espera = atol(strtok(NULL, "|"));                           // <-
        else if (strcmp(tok, "est_total_tempo_loja") == 0)                                          // <-
            sm_temp->est_clientes.total_tempo_loja = atol(strtok(NULL, "|"));                       // <-
        else if (strcmp(tok, "est_total_gasto") == 0)                                               // <-
            sm_temp->est_clientes.total_gasto = atof_virgula(strtok(NULL, "|"));                    // <-
        else if (strcmp(tok, "CAIXA") == 0 && idx < MAX_CAIXAS)
        {
            Caixa *cai = &sm_temp->caixas[idx];
            cai->id                       = atoi(strtok(NULL, "|"));
            strncpy(cai->operador_nome,        strtok(NULL, "|"), MAX_NOME - 1);
            cai->total_clientes_atendidos = atoi(strtok(NULL, "|"));
            cai->total_produtos_vendidos  = atoi(strtok(NULL, "|"));
            cai->total_valor_vendido      = atof_virgula(strtok(NULL, "|"));
            cai->produtos_oferecidos      = atoi(strtok(NULL, "|"));
            cai->valor_oferecido          = atof_virgula(strtok(NULL, "|"));
            cai->ticks_aberta             = atoi(strtok(NULL, "|"));
            idx++;
        }
    }
    fclose(f);
    return encontrado;
}


void carregarBanidos(Supermercado *sm)
{
    FILE *f = fopen("banidos.txt", "r");
    if (!f) return; // ficheiro nao existe ainda

    char linha[256];
    while (fgets(linha, sizeof(linha), f))
    {
        if (strlen(linha) <= 1) continue;
        linha[strcspn(linha, "\n")] = '\0';

        char *tab = strchr(linha, '\t');
        if (!tab) continue;
        *tab = '\0';

        NodoBanido *novo = malloc(sizeof(NodoBanido));
        if (!novo) continue;
        novo->id = atoi(linha);
        strncpy(novo->nome, tab + 1, MAX_NOME - 1);
        novo->nome[MAX_NOME - 1] = '\0';
        novo->proximo       = sm->banidos.inicio;
        sm->banidos.inicio  = novo;
        sm->banidos.total++;
    }
    fclose(f);
}


void registarAcao(Supermercado *sm, const char *acao, const char *detalhes)
{
    FILE *f = fopen("utilizador.csv", "a");
    if (!f) return;

    // obter data e hora real
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char data_hora[32];
    strftime(data_hora, sizeof(data_hora), "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(f, "%s ; Dia %d ; %-20s ; %s\n",
            data_hora,
            sm->dia,
            acao,
            detalhes);

    fclose(f);
}
