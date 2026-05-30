#ifndef FUNCOES_H_INCLUDED
#define FUNCOES_H_INCLUDED

#include "supermercado.h"



void inicializarLoja (Supermercado *sm);
void iniciarDia(Supermercado *sm);

void inserirLoja (ListaLoja *L, EntradaCliente entradaCliente);
void removerLoja (ListaLoja *L, int tick_atual);

void preencherCarrinho (Cliente *Cli, Supermercado *sm);

Caixa *escolherCaixa (Supermercado *sm);
void moverMetadeFila(Caixa *antiga, Caixa *nova);
void gerirCaixas(Supermercado *sm, int loja_fechada);
void entrarFila(Caixa *cai, Cliente *cli);
Cliente *sairFila(Caixa *cai);
void clienteEntrarCaixa(Supermercado *sm, Cliente *cliente);
void processarAtendimento (Supermercado *sm);                   //funciona
void oferecerProduto(Supermercado *sm);                         //funciona
Cliente *sairFilaPonteiro(Caixa *cai, Cliente *alvo);           //testar
void moverClientePorId(Supermercado *sm, int origem);           //testar


void abrirCaixaDefinitiva(Supermercado *sm, int id_caixa);      // funciona
void fecharCaixaDefinitiva(Supermercado *sm, int id_caixa);     // funciona
void caixasAutomaticas (Supermercado *sm, int op);              // funciona

int filasTotais(Supermercado *sm);

#endif // FUNCOES_H_INCLUDED
