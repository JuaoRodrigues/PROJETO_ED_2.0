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






