#include "funcoes.h"
#include "Ficheiros.h"
#include "estatisticas.h"


// ------------------------- INICIALIZAR CAIXAS POR DIA -------------------------
void inicializarLoja (Supermercado *sm)
{
    lerConfiguracao("Configuracao.txt", &sm->config);
    lerClientes("clientes.txt", &sm->clientes);
    sm->produtos = lerProdutos("produtos.txt", &sm->total_produtos, sm->config.tempo_atendimento_produto);

    // gestao de caixas
    for (int i = 0; i < sm->config.n_caixas; i++)
    {
        sm->caixas[i].id    = i + 1;
        sm->caixas[i].ativa = 0;
        sm->caixas[i].seg_fim_atendimento = -1;
    }
    carregarBanidos(sm);
    iniciarDia(sm);
}

void iniciarDia (Supermercado *sm)
{
    // os funcionarios trocam de caixa em cada dia
    atribuirFuncionarios("funcionarios.txt", sm);

    // limpa a lista de clientes na loja
    NodoLoja *nodoLoja = sm->clientes_na_loja.inicio;
    while (nodoLoja != NULL)
    {
        NodoLoja *temp = nodoLoja;
        nodoLoja = nodoLoja->proximo;
        free(temp);
    }
    sm->clientes_na_loja.inicio       = NULL;
    sm->clientes_na_loja.total_na_loja = 0;

    // reset das caixas
    for (int i = 0; i < sm->config.n_caixas; i++)
    {
        Caixa *cai = &sm->caixas[i];

        // esvazia a fila
        while (cai->fila.tamanho > 0)
            sairFila(cai);

        // limpa lista de ofertas
        NODO_prod_oferecido *nodoOferta = cai->oferta.inicio;
        while (nodoOferta != NULL)
        {
            NODO_prod_oferecido *temp = nodoOferta;
            nodoOferta = nodoOferta->proximo;
            free(temp);
        }
        cai->oferta.inicio = NULL;
        cai->oferta.total  = 0;

        // limpa historico de clientes atendidos
        NodoClienteAtendido *nodoHist = cai->historico.inicio;
        while (nodoHist != NULL)
        {
            NodoClienteAtendido *temp = nodoHist;
            nodoHist = nodoHist->proximo;
            free(temp);
        }
        cai->historico.inicio = NULL;
        cai->historico.total  = 0;

        // reset dos contadores
        cai->total_clientes_atendidos = 0;
        cai->total_produtos_vendidos  = 0;
        cai->total_valor_vendido      = 0.0;
        cai->produtos_oferecidos      = 0;
        cai->valor_oferecido          = 0.0;
        cai->seg_fim_atendimento      = -1;
        cai->tick_fim_atendimento     = 0;
        cai->ticks_aberta             = 0;
        cai->tick_abertura            = 0;
        sm->clientesEntrados = 0;

        // fecha todas as caixas
        if (cai->ativa != 0) cai->ativa = 0;
    }

    // reinicia os contadores gerais
    sm->produtos_oferecidos_total = 0;
    sm->valor_total_ganho         = 0.0f;
    sm->valor_oferecido_total     = 0.0f;
    sm->clientesDia               = 0;
    sm->est_clientes              = (EstatisticasClientes){0};

    // reinicia o tempo e atualiza o dia
    sm->st.tick_atual    = 0;
    sm->st.seg_simulados = 0;
    sm->dia = obterProximoDia();

    // abre a caixa 1 todos os dias
    sm->caixas[0].ativa = 1;     // a caixa 1 começa ativa todos os dias
    sm->caixas[0].tick_abertura = 0;
}


// ------------------------- INSERIR CLIENTE NA LOJA -------------------------
void inserirLoja (ListaLoja *L, EntradaCliente entradaCliente)
{
    NodoLoja *novo = malloc(sizeof(NodoLoja));
    if (!novo)  return;

    novo->entrada = entradaCliente;
    novo->proximo = L->inicio;
    L->inicio = novo;
    L->total_na_loja ++;
}

// ------------------------- REMOVER CLIENTE DA LOJA -------------------------
void removerLoja (ListaLoja *L, int tick_atual)
{
    NodoLoja *atual = L->inicio;
    NodoLoja *anterior = NULL;

    while(atual)
    {
        if(atual->entrada.tick_saida == tick_atual){
            NodoLoja *remover = atual;

            if(anterior)
                anterior->proximo = atual->proximo;
            else
                L->inicio = atual->proximo;

            atual = atual->proximo;
            free(remover);
            L->total_na_loja --;
        }else{
            anterior = atual;
            atual = atual->proximo;
        }
    }
}

// ------------------------- ENCHER CARRINHOS -------------------------

void preencherCarrinho (Cliente *Cli, Supermercado *sm)
{
    //limpar carrinho anterior se existir
    Produto *p = Cli->carrinho;
    while(p)
    {
        Produto *tmp = p->proximo;
        free(p);
        p = tmp;
    }


    int n = rand() % (MAX_PRODUTOS_CARRINHO + 1);   // escolha da quantidade de produtos [0, max_produtos]
    Cli->n_produtos = 0;
    Cli->carrinho = NULL;

    for(int i = 0; i < n; i++)
    {
        int escolha = rand() % sm->total_produtos;  // escolha aleatoria do produto
        Produto *copia = malloc(sizeof(Produto));
        if(!copia)  continue;
        *copia = sm->produtos[escolha];
        copia->proximo = NULL;

        copia->proximo = Cli->carrinho;
        Cli->carrinho = copia;
        Cli->n_produtos ++;
    }
}




// ------------------------- CONTAR PRODUTOS NA FILA X -------------------------


// ------------------------- ESCOLHER CAIXA -------------------------
// baseado na quantidade de produtos em cada caixa
Caixa *escolherCaixa(Supermercado *sm)
{
    Caixa *melhor = NULL;
    int menos_produtos = -1;

    for(int i = 0; i < sm->config.n_caixas; i++)
    {
        if (sm->caixas[i].ativa != 1 && sm->caixas[i].ativa != 3 && sm->caixas[i].ativa != 5) continue;
        if (sm->caixas[i].fila.tamanho >= sm->config.max_fila) continue;

        int total_produtos = 0;
        Cliente *Cli = sm->caixas[i].fila.frente;
        while(Cli){
            total_produtos += Cli->n_produtos;
            Cli = Cli->proximo;
        }

        if(melhor == NULL || total_produtos < menos_produtos){
            menos_produtos = total_produtos;
            melhor = &sm->caixas[i];
        }
    }

    // se todas as caixas estao cheias, escolhe a com menos produtos ignorando o limite
    if (melhor == NULL)
    {
        for(int i = 0; i < sm->config.n_caixas; i++)
        {
            if (sm->caixas[i].ativa != 1 && sm->caixas[i].ativa != 3) continue;

            int total_produtos = 0;
            Cliente *Cli = sm->caixas[i].fila.frente;
            while(Cli){
                total_produtos += Cli->n_produtos;
                Cli = Cli->proximo;
            }

            if(melhor == NULL || total_produtos < menos_produtos){
                menos_produtos = total_produtos;
                melhor = &sm->caixas[i];
            }
        }
    }

    return melhor;
}


/*
Caixa *escolherCaixa (Supermercado *sm)
{
    Caixa *melhor = NULL;
    int menos_produtos = -1;

    for(int i = 0; i < sm->config.n_caixas; i++)
    {
        if (sm->caixas[i].ativa != 1 && sm->caixas[i].ativa != 3) continue;
        if (sm->caixas[i].fila.tamanho >= sm->config.max_fila) continue;  // ignora filas cheias

        // faz a contagem de produtos em cada caixa ativa
        int total_produtos = 0;
        Cliente *Cli = sm->caixas[i].fila.frente;
        while(Cli){
            total_produtos += Cli->n_produtos;
            Cli = Cli->proximo;
        }

        // decidir a melhor caixa para ir
        if(melhor == NULL || total_produtos < menos_produtos){
            menos_produtos = total_produtos;
            melhor = &sm->caixas[i];
        }
    }
    return melhor;
}
*/
// ------------------------- MOVER METADE DA FILA -------------------------

void moverMetadeFila(Caixa *antiga, Caixa *nova)
{
    if (antiga->fila.tamanho == 0) return;
    int mover = antiga->fila.tamanho / 2;
    if(mover == 0)  mover = 1;

    // encontrar o ultimo NODO que fica na fila
    int manter = antiga->fila.tamanho - mover;
    if (manter == 0) {
        nova->fila.frente    = antiga->fila.frente;
        nova->fila.fim       = antiga->fila.fim;
        nova->fila.tamanho   = mover;
        antiga->fila.frente  = NULL;
        antiga->fila.fim     = NULL;
        antiga->fila.tamanho = 0;
        return;
    }


    Cliente *cli = antiga->fila.frente;
    for(int i = 0; i < manter - 1; i++)
    {
        cli = cli->proximo;
    }

    // cli é agora o ultimo cliente da fila antiga
    // passa pessoas para a outra fila
    nova->fila.frente = cli->proximo;
    nova->fila.fim = antiga->fila.fim;
    nova->fila.tamanho = mover;

    // corta a fila antiga
    cli->proximo = NULL;
    antiga->fila.fim = cli;
    antiga->fila.tamanho = manter;
}

/*      ANTIGO
void moverMetadeFila(Caixa *antiga, Caixa *nova)
{
    int mover = antiga->fila.tamanho / 2;

    // encontrar o ultimo NODO que fica na fila
    int manter = antiga->fila.tamanho - mover;
    Cliente *cli = antiga->fila.frente;
    for(int i = 0; i < manter - 1; i++)
    {
        cli = cli->proximo;
    }

    // cli é agora o ultimo cliente da fila antiga
    // passa pessoas para a outra fila
    nova->fila.frente = cli->proximo;
    nova->fila.fim = antiga->fila.fim;
    nova->fila.tamanho = mover;

    // corta a fila antiga
    cli->proximo = NULL;
    antiga->fila.fim = cli;
    antiga->fila.tamanho = manter;
}



*/


// ------------------------- GERIR CAIXA -------------------------
void gerirCaixas(Supermercado *sm, int loja_fechada)
{
    // converte todas as caixas recentemente abertas para abertas normais
    // ativa = 3 -> ativa = 1
    // serve para a caixa nao ser aberta e fechada no mesmo tick
    for(int i = 0; i < sm->config.n_caixas; i++)
    {
        if (sm->caixas[i].ativa == 3)   sm->caixas[i].ativa = 1;
    }


    //          1 PARTE
    // verifica se alguma caixa ativa atingiu o maximo -> abre nova caixa
    // verifica se existe alguma caixa a fechar -> esta permanece aberta
    for(int i = 0; i < sm->config.n_caixas; i++)
    {
        if (sm->caixas[i].ativa != 1 && sm->caixas[i].ativa != 5) continue;

        if(sm->caixas[i].fila.tamanho >= sm->config.max_fila)
        {
            // verifica se ha alguma caixa a fechar para reativar
            int reativou = 0;
            for(int j = 0; j < sm->config.n_caixas; j++)
            {
                if(sm->caixas[j].ativa == 2){
                    // reativa caixa que estava a fechar
                    sm->caixas[j].ativa = 1;
                    sm->caixas[j].tick_abertura = sm->st.tick_atual;
                    moverMetadeFila(&sm->caixas[i], &sm->caixas[j]);
                    /*printf("Caixa %d reativada. %d clientes transferidos da caixa %d\n",
                        sm->caixas[j].id,
                        sm->caixas[j].fila.tamanho,
                        sm->caixas[i].id);*/
                    reativou = 1;
                    break;
                }
            }

            if(!reativou)
            {
                for(int j = 0; j < sm->config.n_caixas; j++)
                {
                    if(sm->caixas[j].ativa == 0)
                    {
                        sm->caixas[j].ativa = 1;
                        sm->caixas[j].tick_abertura = sm->st.tick_atual;
                        moverMetadeFila(&sm->caixas[i], &sm->caixas[j]);
                        /*printf("Caixa %d aberta. %d clientes transferidos da caixa %d\n",
                                sm->caixas[j].id,
                                sm->caixas[j].fila.tamanho,
                                sm->caixas[i].id);*/
                        break;
                    }
                }
            }
            break;      // restringe a abertura de caixas ao maximo de 1 por tick
        }
    }


    //          2 PARTE
    // conta quantas caixas estao ativas
    int ativas = 0;
    for(int i = 0; i < sm->config.n_caixas; i++)
    {
        if(sm->caixas[i].ativa == 1 || sm->caixas[i].ativa == 5)    ativas ++;
    }


    //          3 PARTE
    // se ha mais que 1 caixa aberta entao verifica se tem que fechar alguma
    // uma caixa com o minimo -> fecha caixa
    if(ativas > 1)
    {
        int caixa_cheia = 0;
        for(int i = 0; i < sm->config.n_caixas; i++)
        {
            if((sm->caixas[i].ativa == 1 || sm->caixas[i].ativa == 5) && sm->caixas[i].fila.tamanho >= sm->config.max_fila)
            {
                caixa_cheia = 1;
                break;
            }
        }

        // so fecha caixas se nao houver nenhuma caixa cheia
        if(!caixa_cheia)
        {
            for(int i = 0; i < sm->config.n_caixas; i++)
            {
                if (sm->caixas[i].ativa != 1) continue;
                if (sm->caixas[i].fila.tamanho < sm->config.min_fila)
                    {
                    sm->caixas[i].ativa = 2;
                    //printf("Caixa %d a fechar (poucos clientes)\n", sm->caixas[i].id);
                    ativas--;
                if (ativas == 1) break;
                }
            }
        }
    }


    //          4 PARTE
    // fecha as caixas que estao a fechar e que ja nao têm fila
    for(int i = 0; i < sm->config.n_caixas; i++)
    {
        if (sm->caixas[i].ativa == 2 && sm->caixas[i].fila.tamanho == 0)
        {
            sm->caixas[i].ticks_aberta += sm->st.tick_atual - sm->caixas[i].tick_abertura;  // acumula o tempo aberta da caixa
            sm->caixas[i].ativa = 0;
            //printf("Caixa %d fechada.\n", sm->caixas[i].id);
        }
    }

    //          5 PARTE
    // caso a loja esteja fechada ja nao precisa de manter 1 caixa aberta
    // fecha todas
    if(loja_fechada)
    {
        int total_fila = 0;
        for(int i = 0; i < sm->config.n_caixas; i++)
        {
            total_fila += sm->caixas[i].fila.tamanho;
        }
        if(total_fila == 0)
        {
            for(int i = 0; i < sm->config.n_caixas; i++)
            {
                sm->caixas[i].ativa = 0;
            }
        }

    }
}


// ------------------------- ENTRAR FILA -------------------------
void entrarFila(Caixa *cai, Cliente *cli)
{
    cli->proximo = NULL;

    // se a fila estiver vazia -> adicionar ao inicio
    if(cai->fila.fim == NULL)
    {
        cai->fila.frente = cli;
        cai->fila.fim = cli;
    }else{
        cai->fila.fim->proximo = cli;
        cai->fila.fim = cli;
    }
    cai->fila.tamanho ++;
}


// ------------------------- SAIR FILA -------------------------
Cliente *sairFila(Caixa *cai)
{
    if(cai->fila.frente == NULL)    return NULL;

    Cliente *atendido = cai->fila.frente;
    cai->fila.frente = atendido->proximo;

    // se a caixa ficou vazia
    if(cai->fila.frente == NULL)    cai->fila.fim = NULL;

    atendido->proximo = NULL;
    cai->fila.tamanho --;
    return atendido;
}


// ------------------------- CLIENTE ENTRA NA FILA CAIXA -------------------------
void clienteEntrarCaixa(Supermercado *sm, Cliente *cli)
{
    if (cli->n_produtos == 0) {
        //printf("Cliente %06d (%s) saiu da loja sem produtos.\n", cli->id, cli->nome);
        sm->est_clientes.total_sem_produtos++;
        return;
    }


    Caixa *cai = escolherCaixa(sm);

    if(cai == NULL)     return;

    cli->tick_entrada_fila = sm->st.tick_atual;
    entrarFila(cai, cli);

    /*printf("Cliente %06d (%s) entrou na fila da caixa %d. Fila: %d clientes.\n",
           cli->id, cli->nome, cai->id, cai->fila.tamanho);*/
}


// ------------------------- ATENDER CLIENTE NA CAIXA -------------------------
void processarAtendimento (Supermercado *sm)
{
    long seg_atual = (long)sm->st.tick_atual * 60;

    for(int i = 0; i< sm->config.n_caixas; i++)
    {
        Caixa *cai = &sm->caixas[i];

        if(cai->ativa == 0)             continue;
        if(cai->fila.frente == NULL)    continue;

        // 1 PARTE
        //calcula o tempo de atendimento do cliente atual
        if(cai->seg_fim_atendimento == -1)
        {
            long tempo_total = 0;

            Produto *p1 = cai->fila.frente->carrinho;

            while(p1)
            {
                tempo_total += p1->tempo_passagem;
                p1 = p1->proximo;
            }
        cai->seg_fim_atendimento = seg_atual + tempo_total;
        }

        // 2 PARTE
        // verifica se o atendimento acabou
        if(seg_atual >= cai->seg_fim_atendimento)
        {
            Cliente *atendido = sairFila(cai);
            if(!atendido)   continue;

            atendido->proximo = NULL;
            cai->total_clientes_atendidos ++;
            cai->total_produtos_vendidos += atendido->n_produtos;

            // 3 PARTE
            // soma o valor dos produtos
            Produto *p2 = atendido->carrinho;
            while(p2)
            {
                cai->total_valor_vendido += p2->preco;
                sm->valor_total_ganho    += p2->preco;
                p2 = p2->proximo;
            }
            //printf("Cliente %06d (%s) foi atendido na caixa %d.\n",
            //       atendido->id, atendido->nome, cai->id);

            registar_cliente_atendido(cai, atendido);

            // atualiza as estatisticas dos clientes
            sm->est_clientes.total_atendidos++;
            sm->est_clientes.total_produtos   += atendido->n_produtos;
            sm->est_clientes.total_com_oferta += atendido->produto_oferecido;
            sm->est_clientes.total_espera     += sm->st.tick_atual - atendido->tick_entrada_fila;
            sm->est_clientes.total_tempo_loja += sm->st.tick_atual - atendido->tick_entrada;
            Produto *p3 = atendido->carrinho;
            while (p3) { sm->est_clientes.total_gasto += p3->preco; p3 = p3->proximo; }


            cai->seg_fim_atendimento = -1;   // reset para o proximo cliente
        }
    }
}


void oferecerProduto(Supermercado *sm)
{
    for(int i = 0; i < sm->config.n_caixas; i++)
    {
        Caixa *cai = &sm->caixas[i];
        if(cai->ativa == 0)  continue;

        // comecamos a ver no 2 elemento da fila pq o 1 ja está a ser atendido
        // e nao faz sentido oferecer-lhe nada

        Cliente *cli = cai->fila.frente;
        if(cli) cli = cli->proximo;

        while(cli)
        {
            // verifica se o cliente espera ha mais de max_espera ticks
            int espera = sm->st.tick_atual - cli->tick_entrada_fila;
            if(espera >= sm->config.max_espera && cli->produto_oferecido == 0 && cli->carrinho != NULL)
            {
                // conta os produtos do carrinho
                int total = 0;
                Produto *p = cli->carrinho;
                while(p)
                {
                    total ++;
                    p = p->proximo;
                }

                // escolhe um produto aleatorio e reinicia p
                int escolha = rand() % total;
                p = cli->carrinho;
                for(int i = 0; i < escolha; i++)
                {
                    p = p->proximo;
                }

                cli->produto_oferecido = 1;

                // adiciona o produto à lista de produtos oferecidos
                NODO_prod_oferecido *novo = malloc(sizeof(NODO_prod_oferecido));
                if(!novo) {cli = cli->proximo; continue;}
                novo->prod              = *p;    //copia o produto
                novo->prod.proximo      = NULL;
                novo->proximo           = cai->oferta.inicio;
                cai->oferta.inicio      = novo;
                cai->oferta.total++;

//printf("\nPRODUTO OFERECIDO A: [%s] - %s : %.2f", cli->nome, p->nome, p->preco);

                // atualiza estatísticas da caixa e do supermercado
                cai->produtos_oferecidos++;
                cai->valor_oferecido += p->preco;
                sm->produtos_oferecidos_total++;
                sm->valor_oferecido_total += p->preco;
            }
            cli = cli->proximo;
        }
    }
}


int filasTotais(Supermercado *sm)
{
    int total = 0;
    for (int i = 0; i < sm->config.n_caixas; i++)
        total += sm->caixas[i].fila.tamanho;
    return total;
}


// -------------------------- REVER NAO FUNCIONA

// NECESSARIO VALIDACAO ANTES SOBRE O ID DA CAIXA
void abrirCaixaDefinitiva(Supermercado *sm, int id_caixa)
{
    Caixa *cai = &sm->caixas[id_caixa - 1];

    if(cai->ativa == 5)
    {
        printf("\n  A caixa %d já estava aberta definitivamente!");
        return;
    }

    cai->ativa = 5;
    cai->seg_fim_atendimento = -1;
    char det[64]; sprintf(det, "Caixa %d", id_caixa); registarAcao(sm, "ABRIR CAIXA DEF", det);
    printf("\n  Caixa %d aberta definitivamente!", id_caixa);
    printf("\n  Caso futuramente pretenda fechar esta caixa tem que o fazer manualmente");
}


void fecharCaixaDefinitiva(Supermercado *sm, int id_caixa)
{
    Caixa *cai = &sm->caixas[id_caixa - 1];

    if(cai->ativa == 4)
    {
        printf("\n  A caixa %d já estava fechada definitivamente!", id_caixa);
        return;
    }

    cai->ativa = 4;
    cai->seg_fim_atendimento = -1;
    char det[64]; sprintf(det, "Caixa %d", id_caixa); registarAcao(sm, "FECHAR CAIXA DEF", det);

    // move os clientes um a um, cada um escolhe a melhor caixa disponivel
    if(cai->fila.tamanho > 0)
    {
        printf("\n  Na seguinte tabela pode verificar a distribuição dos clientes da caixa %d", id_caixa);
        printf("\n\n  | Caixa | Cliente\n");
    }

    while (cai->fila.tamanho > 0)
    {
        Caixa *destino = escolherCaixa(sm);
        if(destino == NULL)
        {
            printf("\n  Aviso: nao ha caixas abertas para receber a fila.\n");
            break;
        }
        Cliente *cli = sairFila(cai);
        entrarFila(destino, cli);
        printf("  | %3d   | %s\n", destino->id, cli->nome);
    }
    printf("\n  Caixa %d fechada definitivamente.\n", id_caixa);
    printf("\n  Caso futuramente pretenda fechar esta caixa tem que o fazer manualmente");
}


void caixasAutomaticas (Supermercado *sm, int op)
{
    if (op == 0)
    {
        // mete todas as caixas em automatico
        for(int i = 0; i < sm->config.n_caixas; i++)
        {
            if (sm->caixas[i].ativa == 5) sm->caixas[i].ativa = 1;
            if (sm->caixas[i].ativa == 4) sm->caixas[i].ativa = 0;
        }
        registarAcao(sm, "GESTAO AUTOMATICA", "Todas as caixas em automatico");
        printf("\n  Gestao automatica reativada para todas as caixas.\n");
    }else{
        if (sm->caixas[op - 1].ativa == 5) sm->caixas[op - 1].ativa = 1;
        if (sm->caixas[op - 1].ativa == 4) sm->caixas[op - 1].ativa = 0;
        printf("\n  Gestao automatica reativada para a caixa %d.\n", op);
        char det[64];
        sprintf(det, "Caixa %d em automatico", op);
        registarAcao(sm, "GESTAO AUTOMATICA", det);
    }
}


// encontra o cliente que vamos mover
// aponta para ele
Cliente *sairFilaPonteiro(Caixa *cai, Cliente *alvo)
{
    if (cai->fila.frente == NULL || alvo == NULL) return NULL;

    // caso especial: remover o primeiro
    if (cai->fila.frente == alvo) return sairFila(cai);

    // percorre ate encontrar o anterior ao alvo
    Cliente *anterior = cai->fila.frente;
    while (anterior->proximo != alvo)
    {
        if (anterior->proximo == NULL) return NULL; // nao encontrou
        anterior = anterior->proximo;
    }

    anterior->proximo = alvo->proximo;
    if (alvo == cai->fila.fim) cai->fila.fim = anterior;

    alvo->proximo = NULL;
    cai->fila.tamanho--;
    return alvo;
}


void moverClientePorId(Supermercado *sm, int origem)
{
    Caixa *cai_origem = &sm->caixas[origem - 1];
    printf("\n  DEBUG: caixa %d | ativa=%d | tamanho=%d\n",
           cai_origem->id, cai_origem->ativa, cai_origem->fila.tamanho);

    if(cai_origem->fila.tamanho == 0)
    {
        printf("  A caixa %d não tem clientes na fila", origem);
        return;
    }

    // listar fila
    imprimirFila(sm, origem);

    // escolher pelo ID do cleinte
    int id_escolhido;
    Cliente *cli = NULL;
    do{
        printf("\n  Indique o ID do cliente que pretende mover!");
        printf("\n  ID: ");
        scanf("%d", &id_escolhido);
        LIMPAR_BUFFER();

        cli = cai_origem->fila.frente;
        while(cli)
        {
            if (cli->id == id_escolhido) break;
            cli = cli->proximo;
        }
        if (!cli) printf("\n  ID nao encontrado na fila desta caixa!\n");
    }while(!cli);

    // escolher caixa de destino
    int destino;
    do{
        printf("\n\n  Escolha o número da caixa de destino!");
        printf("\n  Caixa de destino: ");
        scanf("%d", &destino);
        LIMPAR_BUFFER();
        if (destino <= 0 || destino > sm->config.n_caixas)
        {
            printf("\n  Número de caixa inválido!\n  Tente um valor entre 1 e %d", sm->config.n_caixas, origem);
            continue;
        }
        if(destino == origem)
        {
            printf("\n  A caixa de destino não pode ser igual à de origem!\n  Tente novamente");
            continue;
        }
        if(sm->caixas[destino - 1].ativa == 0 || sm->caixas[destino - 1].ativa == 4)
        {
            printf("\n  A caixa %d está fechada!\n  Tente novamente");
            destino = 0;
        }
    }while(destino <= 0 || destino > sm->config.n_caixas || destino == origem);

    // mover usando o ponteiro ja encontrado
    Cliente *movido = sairFilaPonteiro(cai_origem, cli);
    if (movido != NULL)
    {
        entrarFila(&sm->caixas[destino - 1], movido);
        char det[128];
        sprintf(det, "Cliente %06d (%s) da caixa %d para caixa %d", movido->id, movido->nome, origem, destino);
        registarAcao(sm, "MOVER CLIENTE", det);
        printf("\n  Cliente %06d (%s) movido da caixa %d para a caixa %d.\n", movido->id, movido->nome, origem, destino);
    }
}

// verifica se o cliente pertence a clientes.txt
// se sim devolve o cliente
NodoHash *procurarCliente(HashTable *ht, int id)
{
    int bucket = id % HASH_SIZE;
    NodoHash *nodo = ht->buckets[bucket];

    while(nodo)
    {
        if(nodo->id_cliente == id)  return nodo;
        nodo = nodo->proximo;
    }
    return NULL;
}


void pesquisarCliente(Supermercado *sm, int id)
{
    int naLoja = 0;
    int naFila = 0;
    // verifica se o cliente existe na hashtable
    NodoHash *nodo = procurarCliente(&sm->clientes, id);
    if(!nodo)   {printf("\n  Cliente %06d não existe no ficheiro 'clientes.txt'.\n", id); return;}

    char det[128];
    sprintf(det, "Cliente %06d - %s", id, nodo->cliente->nome);
    int tick = nodo->cliente->tick_entrada;
    int hora = tick / 60;
    int min = tick % 60;

    printf("\n  Dados do cliente %06d", id);
    printf("\n  Nome: %s", nodo->cliente->nome);

    // verifica se está na loja
    NodoLoja *nodoL = sm->clientes_na_loja.inicio;
    while(nodoL)
    {
        if(nodoL->entrada.cliente->id == id)
        {
            printf("\n  Estado: dentro da loja (não está em nenhuma fila)");
            naLoja = 1;
            break;
        }
        nodoL = nodoL->proximo;
    }

    registarAcao(sm, "PESQUISAR CLIENTE", det);
    // verifica se está numa fila
    for(int i = 0; i < sm->config.n_caixas; i++)
    {
        Cliente *cli = sm->caixas[i].fila.frente;
        int posicao = 1;

        while(cli)
        {
            if(cli->id == id)
            {
                printf("\n  Estado: na fila da caixa %d | posição %d de %d", i, posicao, sm->caixas[i].fila.tamanho);
                naFila = 1;
                break;
            }
            cli = cli->proximo;
            posicao++;
        }
    }

    if(naLoja == 0 || naFila == 0)
    {
        printf("\n  Estado: não está na loja nem em nenhuma fila\n");
        return;
    }


    // imprime o resto dos dados
    printf("  Entrada: %02dh%02d", hora, min);
    printf("\n  Carrinho: %d produtos", nodo->cliente->n_produtos);
    Produto *p = nodo->cliente->carrinho;
    int contador = 1;
    while(p)
    {
        printf("\n%5d. %3.2f | %s", contador, p->preco, p->nome);
        contador++;
    }
}


void banirCliente(Supermercado *sm, int id)
{
    NodoHash *nodo = procurarCliente(&sm->clientes, id);
    if(!nodo) {printf("\n  Cliente %06d não existe no ficheiro 'clientes.txt'.\n", id); return;}

    // verifica se ja esta banido
    NodoBanido *b = sm->banidos.inicio;
    while(b)
    {
        if(b->id == id)
        {
            printf("\n  O cliente %06d - %s já estava banido!", b->id, b->nome);
            return;
        }
        b = b->proximo;
    }

    // adicionar à lista
    NodoBanido *novo = malloc(sizeof(NodoBanido));
    if (!novo) return;
    novo->id = id;
    strncpy(novo->nome, nodo->cliente->nome, MAX_NOME - 1);
    novo->proximo       = sm->banidos.inicio;
    sm->banidos.inicio  = novo;
    sm->banidos.total++;

    // guarda no ficheiro
    FILE *f = fopen("banidos.txt", "a");
    if(f)
    {
        fprintf(f, "%d\t%s\n", id, nodo->cliente->nome);
        fclose(f);
    }
    printf("\n  O cliente %06d - %s foi banido com sucesso.\n", id, nodo->cliente->nome);
    char det[128];
    sprintf(det, "Cliente %06d - %s", id, nodo->cliente->nome);
    registarAcao(sm, "BANIR CLIENTE", det);
}


void desbanirCliente(Supermercado *sm, int id)
{
    NodoBanido *atual    = sm->banidos.inicio;
    NodoBanido *anterior = NULL;

    while(atual)
    {
        if(atual->id == id)
        {
            if (anterior)   anterior->proximo  = atual->proximo;
            else            sm->banidos.inicio = atual->proximo;
            sm->banidos.total--;

            // guarda o nome antes de libertar o nodo
            char nome_temp[MAX_NOME];
            strncpy(nome_temp, atual->nome, MAX_NOME - 1);
            nome_temp[MAX_NOME - 1] = '\0';

            printf("\n  Cliente %06d - %s desbanido com sucesso.\n", id, atual->nome);
            free(atual);
            char det[128];
            sprintf(det, "Cliente %06d - %s", id, atual->nome);

            // reescreve o ficheiro sem este cliente
            FILE *f = fopen("banidos.txt", "w");
            if (f)
            {
                NodoBanido *b = sm->banidos.inicio;
                while (b)
                {
                    fprintf(f, "%d\t%s\n", b->id, b->nome);
                    b = b->proximo;
                }
                fclose(f);
                char det[128];
                sprintf(det, "Cliente %06d - %s", id, nome_temp);
                registarAcao(sm, "DESBANIR CLIENTE", det);
            }
            return;
        }
        anterior = atual;
        atual    = atual->proximo;
    }
    printf("\n  Cliente %06d não está na lista de banidos.\n", id);
}


// verifica se o cliente foi banido
int clienteBanido(Supermercado *sm, int id)
{
    NodoBanido *b = sm->banidos.inicio;
    while (b)
    {
        if (b->id == id) return 1;
        b = b->proximo;
    }
    return 0;
}

void listarBanidos(Supermercado *sm)
{
    printf("\n=== Clientes Banidos | Total: %d\n\n", sm->banidos.total);

    if (sm->banidos.total == 0)
    {
        printf("  Nenhum cliente banido.\n");
        return;
    }

    NodoBanido *b = sm->banidos.inicio;
    int i = 1;
    while (b)
    {
        printf(" %3d. | %06d | %s\n", i, b->id, b->nome);
        b = b->proximo;
        i++;
    }
}

// ------------------------------------------------------
void libertarMemoria(Supermercado *sm)
{
    // 1 - HASHTABLE
    for(int i = 0; i < HASH_SIZE; i++)
    {
        NodoHash *nodo = sm->clientes.buckets[i];
        while(nodo)
        {
            NodoHash *temp = nodo;
            nodo = nodo->proximo;

            // liberta o carrinho do cliente
            Produto *p = temp->cliente->carrinho;
            while(p)
            {
                Produto *tmp = p->proximo;
                free(p);
                p = tmp;
            }
            free(temp->cliente);  // liberta o cliente
            free(temp);           // liberta o nodo da hashtable
        }
        sm->clientes.buckets[i] = NULL;
    }
    sm->clientes.total_clientes = 0;


    // 2 - ARRAY DE PRODUTOS
    free(sm->produtos);
    sm->produtos      = NULL;
    sm->total_produtos = 0;


    // 3 - LISTA DE CLIENTES NA LOJA
    NodoLoja *nodoL = sm->clientes_na_loja.inicio;
    while(nodoL)
    {
        NodoLoja *temp = nodoL;
        nodoL = nodoL->proximo;
        free(temp);
    }
    sm->clientes_na_loja.inicio        = NULL;
    sm->clientes_na_loja.total_na_loja = 0;


    // 4 - CAIXAS
    for(int i = 0; i < sm->config.n_caixas; i++)
    {
        Caixa *cai = &sm->caixas[i];

        // 4.1 - fila (os clientes pertencem à hashtable, nao se libertam aqui)
        cai->fila.frente  = NULL;
        cai->fila.fim     = NULL;
        cai->fila.tamanho = 0;

        // 4.2 - lista de ofertas
        NODO_prod_oferecido *nodoOferta = cai->oferta.inicio;
        while (nodoOferta)
        {
            NODO_prod_oferecido *temp = nodoOferta;
            nodoOferta = nodoOferta->proximo;
            free(temp);
        }
        cai->oferta.inicio = NULL;
        cai->oferta.total  = 0;

        // 4.3 - historico de clientes atendidos
        NodoClienteAtendido *nodoHist = cai->historico.inicio;
        while (nodoHist)
        {
            NodoClienteAtendido *temp = nodoHist;
            nodoHist = nodoHist->proximo;

            // 4.4 - liberta o carrinho da copia do cliente no historico
            Produto *p = temp->atendido.carrinho;
            while (p)
            {
                Produto *tmp = p->proximo;
                free(p);
                p = tmp;
            }
            free(temp);
        }
        cai->historico.inicio = NULL;
        cai->historico.total  = 0;
    }


    // 5 - LISTA DE BANIDOS
    NodoBanido *nodoBan = sm->banidos.inicio;
    while (nodoBan)
    {
        NodoBanido *temp = nodoBan;
        nodoBan = nodoBan->proximo;
        free(temp);
    }
    sm->banidos.inicio = NULL;
    sm->banidos.total  = 0;
}


void reiniciarPrograma(Supermercado *sm)
{
    // limpa a memória
    libertarMemoria(sm);

    // apaga os ficheiros
    remove("historico.txt");
    remove("banidos.txt");

    // reinicializa tudo do zero
    memset(sm, 0, sizeof(Supermercado));
    inicializarLoja(sm);

    printf("\n  Programa reiniciado com sucesso.\n");
    printf("\n  Historico e lista de banidos apagados.\n");
}


void adicionarCliente(Supermercado *sm)
{
    // 1 - pedir o ID ao utilizador e verificar se ja existe
    int novo_id;
    do
    {
        printf("\n  ID do novo cliente (1 a 999999): ");
        scanf("%d", &novo_id);
        LIMPAR_BUFFER();

        if (novo_id <= 0 || novo_id > 999999)
        {
            printf("\n  ID inválido! Deve ser um valor entre 1 e 999999.\n");
            continue;
        }

        // verificar se já existe na hashtable
        if (procurarCliente(&sm->clientes, novo_id) != NULL)
        {
            printf("\n  Já existe um cliente com o ID %06d! Tente outro.\n", novo_id);
            novo_id = -1;   // força repetição
        }

    } while (novo_id <= 0 || novo_id > 999999);

    // 2 - pedir o nome e validar
    char nome[MAX_NOME];
    do
    {
        printf("\n  Nome do novo cliente: ");
        fgets(nome, MAX_NOME, stdin);
        nome[strcspn(nome, "\n")] = '\0';

        if (strlen(nome) == 0 || strspn(nome, " ") == strlen(nome))
            printf("\n  Nome inválido! Tente novamente.\n");

    } while (strlen(nome) == 0 || strspn(nome, " ") == strlen(nome));

    char det[128]; sprintf(det, "Cliente %06d - %s", novo_id, nome); registarAcao(sm, "ADICIONAR CLIENTE", det);
    // 3 - adicionar à hashtable
    Cliente *c = malloc(sizeof(Cliente));
    if (!c) return;
    c->id                = novo_id;
    strncpy(c->nome, nome, MAX_NOME - 1);
    c->nome[MAX_NOME - 1] = '\0';
    c->n_produtos         = 0;
    c->carrinho           = NULL;
    c->tick_entrada_fila  = -1;
    c->produto_oferecido  = 0;
    c->proximo            = NULL;

    int bucket = novo_id % HASH_SIZE;
    NodoHash *nodo = malloc(sizeof(NodoHash));
    if (!nodo) { free(c); return; }
    nodo->id_cliente   = novo_id;
    nodo->cliente      = c;
    nodo->indice_caixa = -1;
    nodo->proximo      = sm->clientes.buckets[bucket];
    sm->clientes.buckets[bucket] = nodo;
    sm->clientes.total_clientes++;

    // 4 - guardar no ficheiro garantindo que começa numa linha nova
    FILE *f = fopen("clientes.txt", "a");
    if (!f) { printf("\n  Erro ao abrir clientes.txt.\n"); return; }

    // verifica se o ficheiro termina com '\n'
    FILE *f_check = fopen("clientes.txt", "r");
    if (f_check)
    {
        fseek(f_check, -1, SEEK_END);
        char ultimo;
        fread(&ultimo, 1, 1, f_check);
        fclose(f_check);

        FILE *f = fopen("clientes.txt", "a");
        if (!f) { printf("\n  Erro ao abrir clientes.txt.\n"); return; }
        if (ultimo != '\n')
            fprintf(f, "\n");
        fprintf(f, "%06d\t%s\n", novo_id, nome);
        fclose(f);
    }
}



void removerCliente(Supermercado *sm)
{
    // 1 - pedir o ID
    int id;
    do
    {
        printf("\n  ID do cliente a remover (1 a 999999): ");
        scanf("%d", &id);
        LIMPAR_BUFFER();

        if (id <= 0 || id > 999999)
        {
            printf("\n  ID inválido! Deve ser um valor entre 1 e 999999.\n");
            continue;
        }

        if (procurarCliente(&sm->clientes, id) == NULL)
        {
            printf("\n  Cliente %06d não existe!\n", id);
            id = -1;
        }
    } while (id <= 0 || id > 999999);

    // 2 - remover da hashtable
    int bucket = id % HASH_SIZE;
    NodoHash *nodo    = sm->clientes.buckets[bucket];
    NodoHash *anterior = NULL;

    while (nodo)
    {
        if (nodo->id_cliente == id)
        {
            if (anterior)   anterior->proximo            = nodo->proximo;
            else            sm->clientes.buckets[bucket] = nodo->proximo;

            // liberta o carrinho
            Produto *p = nodo->cliente->carrinho;
            while (p)
            {
                Produto *tmp = p->proximo;
                free(p);
                p = tmp;
            }

            char nome_temp[MAX_NOME];
            strncpy(nome_temp, nodo->cliente->nome, MAX_NOME - 1);

            free(nodo->cliente);
            free(nodo);
            sm->clientes.total_clientes--;

            printf("\n  Cliente %06d - %s removido com sucesso.\n", id, nome_temp);

            // 3 - reescrever o ficheiro sem este cliente
            FILE *f_leitura = fopen("clientes.txt", "r");
            FILE *f_escrita = fopen("clientes_temp.txt", "w");
            if (f_leitura && f_escrita)
            {
                char linha[256];
                while (fgets(linha, sizeof(linha), f_leitura))
                {
                    int id_linha = atoi(linha);
                    if (id_linha != id)
                        fprintf(f_escrita, "%s", linha);
                }
                fclose(f_leitura);
                fclose(f_escrita);
                remove("clientes.txt");
                rename("clientes_temp.txt", "clientes.txt");
            }

            char det[128];
            sprintf(det, "Cliente %06d - %s", id, nome_temp);
            registarAcao(sm, "REMOVER CLIENTE", det);
            return;
        }
        anterior = nodo;
        nodo     = nodo->proximo;
    }
}


void memoriaUtilizada(Supermercado *sm)
{
    size_t total = 0;
    size_t total_carrinhos = 0;

    // hashtable - clientes
    for (int i = 0; i < HASH_SIZE; i++)
    {
        NodoHash *nodo = sm->clientes.buckets[i];
        while (nodo)
        {
            total += sizeof(NodoHash);
            total += sizeof(Cliente);

            // carrinho do cliente
            Produto *p = nodo->cliente->carrinho;
            while (p)
            {
                total += sizeof(Produto);
                total_carrinhos += sizeof(Produto);
                p = p->proximo;
            }

            nodo = nodo->proximo;
        }
    }

    // array de produtos
    total += sm->total_produtos * sizeof(Produto);

    // caixas
    for (int i = 0; i < sm->config.n_caixas; i++)
    {
        Caixa *cai = &sm->caixas[i];

        NODO_prod_oferecido *o = cai->oferta.inicio;
        while (o) { total += sizeof(NODO_prod_oferecido); o = o->proximo; }

        NodoClienteAtendido *h = cai->historico.inicio;
        while (h) { total += sizeof(NodoClienteAtendido); h = h->proximo; }
    }

    // clientes na loja
    NodoLoja *nl = sm->clientes_na_loja.inicio;
    while (nl) { total += sizeof(NodoLoja); nl = nl->proximo; }

    // banidos
    NodoBanido *nb = sm->banidos.inicio;
    while (nb) { total += sizeof(NodoBanido); nb = nb->proximo; }

    // estrutura principal
    total += sizeof(Supermercado);

    printf("\n=== Memória Utilizada\n");
    printf("  | Clientes (HashTable)   : %6zu bytes\n", (size_t)(sm->clientes.total_clientes * (sizeof(NodoHash) + sizeof(Cliente))));
    printf("  | Carrinhos (clientes)   : %6zu bytes\n", total_carrinhos);
    printf("  | Produtos (array)       : %6zu bytes\n", (size_t)(sm->total_produtos * sizeof(Produto)));
    printf("  | Estrutura Supermercado : %6zu bytes\n", sizeof(Supermercado));
    printf("  |-------------------------------\n");
    printf("  | TOTAL                  : %6zu bytes (%.2f KB)\n", total, total / 1024.0);
}


void memoriaDesperdicada(Supermercado *sm)
{
    size_t desperdicada = 0;

    // hashtable: buckets vazios
    int buckets_vazios = 0;
    for (int i = 0; i < HASH_SIZE; i++)
        if (sm->clientes.buckets[i] == NULL)
            buckets_vazios++;
    desperdicada += buckets_vazios * sizeof(NodoHash *);

    // array de produtos: capacidade alocada vs usada
    // lerProdutos aloca exatamente 'count' entradas, por isso nao ha desperdicio
    // mas se houver linhas vazias no ficheiro pode haver
    // aqui mostramos o padding por struct (alinhamento de memória)
    size_t padding_produto   = sizeof(Produto)   - (sizeof(int) + MAX_NOME + sizeof(float) + sizeof(int));
    size_t padding_cliente   = sizeof(Cliente)   - (sizeof(int) + MAX_NOME + sizeof(int) + sizeof(long) + sizeof(int));
    size_t padding_caixa     = sizeof(Caixa)     - (sizeof(int) + MAX_NOME * 2 + sizeof(int) * 6 + sizeof(float) * 3 + sizeof(long));

    desperdicada += sm->total_produtos          * padding_produto;
    desperdicada += sm->clientes.total_clientes * padding_cliente;
    desperdicada += sm->config.n_caixas         * padding_caixa;

    // caixas fechadas com fila vazia mas estrutura alocada
    int caixas_inativas = 0;
    for (int i = 0; i < sm->config.n_caixas; i++)
        if (sm->caixas[i].ativa == 0)
            caixas_inativas++;
    desperdicada += caixas_inativas * sizeof(Caixa);

    printf("\n=== Memória Desperdiçada\n");
    printf("  | Buckets vazios (HashTable) : %4d  -> %6zu bytes\n", buckets_vazios, (size_t)(buckets_vazios * sizeof(NodoHash *)));
    printf("  | Padding structs (Produto)  : %4zu bytes/struct x %d = %zu bytes\n", padding_produto, sm->total_produtos, sm->total_produtos * padding_produto);
    printf("  | Padding structs (Cliente)  : %4zu bytes/struct x %d = %zu bytes\n", padding_cliente, sm->clientes.total_clientes, sm->clientes.total_clientes * padding_cliente);
    printf("  | Caixas inativas alocadas   : %4d  -> %6zu bytes\n", caixas_inativas, (size_t)(caixas_inativas * sizeof(Caixa)));
    printf("  |-------------------------------\n");
    printf("  | TOTAL                      : %6zu bytes (%.2f KB)\n", desperdicada, desperdicada / 1024.0);
}
