#ifndef FUNCOES_H_INCLUDED
#define FUNCOES_H_INCLUDED

#include "supermercado.h"


void inserirLoja (ListaLoja *L, EntradaCliente entradaCliente);
void removerLoja (ListaLoja *L, int tick_atual);

void preencherCarrinho (Cliente *Cli, Supermercado *sm);



#endif // FUNCOES_H_INCLUDED
