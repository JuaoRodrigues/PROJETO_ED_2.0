#ifndef TEMPO_H_INCLUDED
#define TEMPO_H_INCLUDED

#include "supermercado.h"

typedef struct {
    int   hora_abertura;        /* da config, ex: 8  */
    int   hora_fecho;           /* da config, ex: 22 */
    int   ticks_totais;         /* total de ticks da simulação */
    int   tick_atual;           /* tick em que estamos agora */
    double segundos_por_tick;   /* tempo real de cada tick */
} SimulacaoTempo;


// CLIENTES NA LOJA
typedef struct {
    Cliente *cliente;
    int      tick_entrada;
    int      tick_saida;
} EntradaCliente;



SimulacaoTempo configurarTempo(Configuracao *config);
void tickParaHora(SimulacaoTempo *st, int *hora, int *minuto);
Cliente *clienteAleatorio(HashTable *ht);
EntradaCliente *prepararEntradas(HashTable *ht, SimulacaoTempo *st, int *total_entradas);

void correrSimulacao(SimulacaoTempo *st, HashTable *ht);









#endif // TEMPO_H_INCLUDED
