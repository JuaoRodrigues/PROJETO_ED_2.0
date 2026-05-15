#include "funcoes.h"

//
void inserirLoja (ListaLoja *L, EntradaCliente entradaCliente)
{
    NodoLoja *novo = malloc(sizeof(NodoLoja));
    if (!novo)  return;

    novo->entrada = entradaCliente;
    novo->proximo = L->inicio;
    L->inicio = novo;
    L->total_na_loja ++;
}

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
