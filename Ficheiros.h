#ifndef FICHEIROS_H_INCLUDED
#define FICHEIROS_H_INCLUDED

#include "supermercado.h"


/* ------------------------------ FUNCOES ------------------------------*/
int lerConfiguracao(const char *ficheiro, Configuracao *config);
void lerClientes(const char *ficheiro, HashTable *ht);
Produto *lerProdutos(const char *ficheiro, int *total, int tempo_max);
void lerDados(const char *ficheiro, Supermercado *sm);
void atribuirFuncionarios(const char *ficheiro, Supermercado *sm);


#endif // FICHEIROS_H_INCLUDED
