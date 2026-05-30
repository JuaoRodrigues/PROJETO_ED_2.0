#ifndef TEMPO_H_INCLUDED
#define TEMPO_H_INCLUDED

#include "supermercado.h"
#include "funcoes.h"

#include <conio.h>      // serve para pausar a simulacao

SimulacaoTempo configurarTempo(Configuracao *config, int temp);
void tickParaHora(SimulacaoTempo *st, int *hora, int *minuto);
Cliente *clienteAleatorio(HashTable *ht);
EntradaCliente *prepararEntradas(Supermercado *sm, HashTable *ht, SimulacaoTempo *st, int *total_entradas);

void correrSimulacao(Supermercado *sm);






#endif // TEMPO_H_INCLUDED
