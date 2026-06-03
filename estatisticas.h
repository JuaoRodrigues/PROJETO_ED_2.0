#ifndef ESTATISTICAS_H_INCLUDED
#define ESTATISTICAS_H_INCLUDED

#include "supermercado.h"


// LISTAGENS
void registar_cliente_atendido (Supermercado *sm, Caixa *cai, Cliente *cli);
void imprimir_historico (Caixa *cai);
void imprimirFila (Supermercado *sm, int id_caixa);


// ESTATISTICAS
void taxa_oferta (Supermercado *sm);
void estatisticas_gerais (Supermercado *sm);
void estatisticas_clientes(Supermercado *sm);



#endif // ESTATISTICAS_H_INCLUDED
