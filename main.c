#include "supermercado.h"
#include "Ficheiros.h"
#include "menus.h"
#include "tempo.h"


#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    srand(time(NULL));  // tempo a 0
    setlocale(LC_ALL, "portuguese");

    Supermercado super = {0};

    lerConfiguracao("Configuracao.txt", &super.config);
    lerClientes("clientes.txt", &super.clientes);
    super.produtos = lerProdutos("produtos.txt", &super.total_produtos, super.config.tempo_atendimento_produto);

    // gestao de caixas
    for (int i = 0; i < super.config.n_caixas; i++)
    {
        super.caixas[i].id    = i;
        super.caixas[i].ativa = 0;
    }
    super.caixas[0].ativa = 1;     // a caixa 1 começa ativa todos os dias


    //menu_principal(&super);

    /* ---------- FUNCIONA
    printf("=== Configuracao carregada ===\n");
    printf("MAX_ESPERA:                 %d\n", super.config.max_espera);
    printf("N_CAIXAS:                   %d\n", super.config.n_caixas);
    printf("TEMPO_ATENDIMENTO_PRODUTO:  %d\n", super.config.tempo_atendimento_produto);
    printf("MAX_PRECO:                  %.2f\n", super.config.max_preco);
    printf("MAX_FILA:                   %d\n", super.config.max_fila);
    printf("MIN_FILA:                   %d\n", super.config.min_fila);
    printf("HORA_ABERTURA:              %d\n", super.config.hora_abertura);
    printf("HORA_FECHO:                 %d\n", super.config.hora_fecho);
    */

    /* --------- FUNCIONA
    printf("Total de clientes carregados: %d\n\n", ht.total_clientes);

    for (int i = 0; i < 10; i++) {
        NodoHash *nodo = ht.buckets[i];
        while (nodo) {
            printf("Bucket %3d | ID: %6d | Nome: %s\n",
                   i, nodo->cliente->id, nodo->cliente->nome);
            nodo = nodo->proximo;
        }
    }
    */

    /* FUNCIONA
    printf("Total de produtos carregados: %d\n\n", total);

        for (int i = 0; i < 20; i++) {
        printf("ID: %5d | Preco: %5.2f | Tempo: %2d | Nome: %s\n",
               super.produtos[i].id,
               super.produtos[i].preco,
               super.produtos[i].tempo_passagem,
               super.produtos[i].nome);
    }
    free(super.produtos);
    */




    super.st = configurarTempo(&super.config);
    correrSimulacao(&super);










    return 0;
}
