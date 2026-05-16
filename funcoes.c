#include "funcoes.h"

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

// ------------------------- MOVER METADE DA FILA -------------------------

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



// ------------------------- GERIR CAIXA -------------------------
void gerirCaixas(Supermercado *sm)
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
        if (sm->caixas[i].ativa != 1) continue;

        if(sm->caixas[i].fila.tamanho >= sm->config.max_fila)
        {
            // verifica se ha alguma caixa a fechar para reativar
            int reativou = 0;
            for(int j = 0; j < sm->config.n_caixas; j++)
            {
                if(sm->caixas[j].ativa == 2){
                    sm->caixas[j].ativa = 1;
                    moverMetadeFila(&sm->caixas[i], &sm->caixas[j]);
                    printf("Caixa %d reativada. %d clientes transferidos da caixa %d\n",
                        sm->caixas[j].id,
                        sm->caixas[j].fila.tamanho,
                        sm->caixas[i].id);
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
                        moverMetadeFila(&sm->caixas[i], &sm->caixas[j]);
                        printf("Caixa %d aberta. %d clientes transferidos da caixa %d\n",
                                sm->caixas[j].id,
                                sm->caixas[j].fila.tamanho,
                                sm->caixas[i].id);
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
        if(sm->caixas[i].ativa == 1)    ativas ++;
    }


    //          3 PARTE
    // se ha mais que 1 caixa aberta entao verifica se tem que fechar alguma
    // uma caixa com o minimo -> fecha caixa
    if(ativas > 1)
    {
        int caixa_cheia = 0;
        for(int i = 0; i < sm->config.n_caixas; i++)
        {
            if((sm->caixas[i].ativa == 1) && sm->caixas[i].fila.tamanho >= sm->config.max_fila)
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
                    printf("Caixa %d a fechar (poucos clientes)\n", sm->caixas[i].id);
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
            sm->caixas[i].ativa = 0;
            printf("Caixa %d fechada.\n", sm->caixas[i].id);
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
    Caixa *cai = escolherCaixa(sm);

    if(cai == NULL)     return;

    cli->tick_entrada_fila = sm->st.tick_atual;
    entrarFila(cai, cli);

    printf("Cliente %06d (%s) entrou na fila da caixa %d. Fila: %d clientes.\n",
           cli->id, cli->nome, cai->id, cai->fila.tamanho);
}





