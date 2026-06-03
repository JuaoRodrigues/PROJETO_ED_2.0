#ifndef TEMPO_H_INCLUDED
#define TEMPO_H_INCLUDED

#include "supermercado.h"
#include "funcoes.h"


// ESCOLHA E ENTRADA DE CLIENTES
Cliente *clienteAleatorio(HashTable *ht);
EntradaCliente *prepararEntradas(Supermercado *sm, HashTable *ht, SimulacaoTempo *st, int *total_entradas);


// TEMPO
void tickParaHora(SimulacaoTempo *st, int *hora, int *minuto);
SimulacaoTempo configurarTempo(Configuracao *config, int temp);


// LOOP
void correrSimulacao(Supermercado *sm);






#endif // TEMPO_H_INCLUDED
