#ifndef TEMPO_H_INCLUDED
#define TEMPO_H_INCLUDED

#include "supermercado.h"
#include "funcoes.h"


SimulacaoTempo configurarTempo(Configuracao *config);
void tickParaHora(SimulacaoTempo *st, int *hora, int *minuto);
Cliente *clienteAleatorio(HashTable *ht);
EntradaCliente *prepararEntradas(HashTable *ht, SimulacaoTempo *st, int *total_entradas);

void correrSimulacao(Supermercado *sm);








#endif // TEMPO_H_INCLUDED
