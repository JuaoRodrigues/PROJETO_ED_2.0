#ifndef ESTATISTICAS_H_INCLUDED
#define ESTATISTICAS_H_INCLUDED

#include "supermercado.h"

void registar_cliente_atendido (Caixa *cai, Cliente *cli);
void imprimir_historico (Caixa *cai);

void taxa_oferta (Supermercado *sm);
void estatisticas_gerais (Supermercado *sm);
void estatisticas_clientes(Supermercado *sm);



#endif // ESTATISTICAS_H_INCLUDED
