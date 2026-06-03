#ifndef FICHEIROS_H_INCLUDED
#define FICHEIROS_H_INCLUDED

#include "supermercado.h"


// LER FICHEIROS INICIAIS
int lerConfiguracao(const char *ficheiro, Configuracao *config);
void lerClientes(const char *ficheiro, HashTable *ht);
Produto *lerProdutos(const char *ficheiro, int *total, int tempo_max);
void atribuirFuncionarios(const char *ficheiro, Supermercado *sm);


// GUARDAR E CARREGAR DADOS EM/DE FICHEIROS
void guardarDia(Supermercado *sm);
int carregarDia(int dia_pretendido, Supermercado *sm_temp);
void carregarBanidos(Supermercado *sm);                             //funciona
void registarAcao(Supermercado *sm, const char *acao, const char *detalhes);        //funciona
int obterProximoDia();



float atof_virgula(const char *str);    //trocar . por , em ficheiros

#endif // FICHEIROS_H_INCLUDED
