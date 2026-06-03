#ifndef FUNCOES_H_INCLUDED
#define FUNCOES_H_INCLUDED
#include "supermercado.h"


// INICIALIZAR
void inicializarLoja (Supermercado *sm);
void iniciarDia(Supermercado *sm);


// CLIENTES
void inserirLoja (ListaLoja *L, EntradaCliente entradaCliente);
void removerLoja (ListaLoja *L, int tick_atual);
void entrarFila(Caixa *cai, Cliente *cli);
Cliente *sairFila(Caixa *cai);
Cliente *sairFilaPonteiro(Caixa *cai, Cliente *alvo);
void moverClientePorId(Supermercado *sm, int origem);
NodoHash *procurarCliente(HashTable *ht, int id);
void pesquisarCliente(Supermercado *sm, int id);
void banirCliente(Supermercado *sm, int id);
void desbanirCliente(Supermercado *sm, int id);
int clienteBanido(Supermercado *sm, int id);
void listarBanidos(Supermercado *sm);
void adicionarCliente(Supermercado *sm);
void removerCliente(Supermercado *sm);


// PRODUTOS
void preencherCarrinho (Cliente *Cli, Supermercado *sm);
void oferecerProduto(Supermercado *sm);


// CAIXAS
Caixa *escolherCaixa (Supermercado *sm);
void moverMetadeFila(Caixa *antiga, Caixa *nova);
void gerirCaixas(Supermercado *sm, int loja_fechada);
void clienteEntrarCaixa(Supermercado *sm, Cliente *cliente);
void processarAtendimento (Supermercado *sm);
void abrirCaixaDefinitiva(Supermercado *sm, int id_caixa);
void fecharCaixaDefinitiva(Supermercado *sm, int id_caixa);
void caixasAutomaticas (Supermercado *sm, int op);
int filasTotais(Supermercado *sm);


// MEMORIA
void memoriaUtilizada(Supermercado *sm);
void memoriaDesperdicada(Supermercado *sm);
void libertarMemoria(Supermercado *sm);
void reiniciarPrograma(Supermercado *sm);


#endif // FUNCOES_H_INCLUDED
