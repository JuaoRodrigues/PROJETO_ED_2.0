#include "supermercado.h"
#include "Ficheiros.h"
#include "menus.h"
#include "tempo.h"
#include "simulacao.h"


#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    srand(time(NULL));  // tempo a 0
    setlocale(LC_ALL, "portuguese");

    Configuracao config = {0};  // inicializa tudo a zero
    lerConfiguracao("Configuracao.txt", &config);

    /* ---------- FUNCIONA
    printf("=== Configuracao carregada ===\n");
    printf("MAX_ESPERA:                 %d\n", config.max_espera);
    printf("N_CAIXAS:                   %d\n", config.n_caixas);
    printf("TEMPO_ATENDIMENTO_PRODUTO:  %d\n", config.tempo_atendimento_produto);
    printf("MAX_PRECO:                  %.2f\n", config.max_preco);
    printf("MAX_FILA:                   %d\n", config.max_fila);
    printf("MIN_FILA:                   %d\n", config.min_fila);
    printf("HORA_ABERTURA:              %d\n", config.hora_abertura);
    printf("HORA_FECHO:                 %d\n", config.hora_fecho);
    */

    HashTable ht = {0};     /* inicializa todos os buckets a NULL */
    lerClientes("clientes.txt", &ht);

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


    int total = 0;
    Produto *produtos = lerProdutos("produtos.txt", &total, 10);

    /* FUNCIONA
    printf("Total de produtos carregados: %d\n\n", total);

        for (int i = 0; i < 20; i++) {
        printf("ID: %5d | Preco: %5.2f | Tempo: %2d | Nome: %s\n",
               produtos[i].id,
               produtos[i].preco,
               produtos[i].tempo_passagem,
               produtos[i].nome);
    }
    free(produtos);
    */

    Supermercado super;
    //menu_principal(&super);

    SimulacaoTempo st = configurarTempo(&config);
    correrSimulacao(&st, &ht);







    return 0;
}
