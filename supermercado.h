#ifndef SUPERMERCADO_H_INCLUDED
#define SUPERMERCADO_H_INCLUDED


/* ------------------------------ CONSTANTES ------------------------------*/
#define MAX_NOME            100
#define MAX_CAIXAS          20
#define SIM_SPEED           600

#define HASH_SIZE           100

// PARA A SIMULACAO
#define MIN_CLIENTES_DIA    10
#define MAX_CLIENTES_DIA    50

#define MIN_TEMPO_LOJA      10   /* minutos simulados */
#define MAX_TEMPO_LOJA      60   /* minutos simulados */


/* ------------------------------ CONFIGURAÇÃO ------------------------------*/
typedef struct {
    int   max_espera;
    int   n_caixas;
    int   tempo_atendimento_produto;
    float max_preco;
    int   max_fila;
    int   min_fila;
    int   hora_abertura;   /* em horas, ex: 6 */
    int   hora_fecho;      /* em horas, ex: 22 */
} Configuracao;


/* ------------------------------ PRODUTO ------------------------------*/
typedef struct Produto {
    int            id;
    char           nome[MAX_NOME];
    float          preco;
    int            tempo_passagem;  /* gerado: rand() % (tempo_atend_prod - 2 + 1) + 2 */
    struct Produto *proximo;
} Produto;


/* ------------------------------ CLIENTE ------------------------------*/
typedef struct Cliente {
    int            id;
    char           nome[MAX_NOME];
    int            n_produtos;
    Produto       *carrinho;           /* lista ligada de produtos */
    double         sim_time_entrada;   /* sim_time quando entrou na fila */
    int            produto_oferecido;  /* 1 se já recebeu oferta neste atendimento */
    struct Cliente *proximo;           /* próximo cliente na fila */
    int      tick_entrada;
    int      tick_saida;
} Cliente;


/* ------------------------------ FILA ------------------------------*/
typedef struct {
    Cliente *frente;
    Cliente *fim;
    int      tamanho;
} Fila;


/* ------------------------------ CAIXA ------------------------------*/
typedef struct {
    int    id;
    char   nome[MAX_NOME];
    char   operador_nome[MAX_NOME];
    int    operador_id;
    int    ativa;
    Fila   fila;
    double sim_time_fim_atendimento;
    int    total_clientes_atendidos;
    int    total_produtos_vendidos;
    float  total_valor_vendido;
    int    produtos_oferecidos;
    float  valor_oferecido;
} Caixa;



/* ------------------------------ HASTABLE ------------------------------*/
typedef struct NodoHash {
    int             id_cliente;
    Cliente         *cliente;       /* ponteiro para o cliente */
    int             indice_caixa;   /* -1 se não está em nenhuma caixa */
    struct NodoHash *proximo;       /* encadeamento para colisões */
} NodoHash;

typedef struct {
    NodoHash *buckets[HASH_SIZE];
    int       total_clientes;      /* útil para estatísticas */
} HashTable;


/* ------------------------------ usado em tempo.c ------------------------------*/
// CLIENTES NA LOJA
typedef struct {
    Cliente *cliente;
    int      tick_entrada;
    int      tick_saida;
} EntradaCliente;

typedef struct {
    int   hora_abertura;        /* da config, ex: 8  */
    int   hora_fecho;           /* da config, ex: 22 */
    int   ticks_totais;         /* total de ticks da simulação */
    int   tick_atual;           /* tick em que estamos agora */
    double segundos_por_tick;   /* tempo real de cada tick */
} SimulacaoTempo;


/* ------------------------------ CLIENTES NA LOJA ------------------------------*/
typedef struct NodoLoja{
    EntradaCliente      entrada;
    struct NodoLoja     *proximo;
} NodoLoja;

typedef struct {
    NodoLoja            *inicio;
    int                 total_na_loja;
} ListaLoja;


/* ------------------------------ SUPERMERCADO ------------------------------*/
typedef struct {
    Caixa           caixas[MAX_CAIXAS];
    Configuracao    config;
    HashTable       clientes;
    ListaLoja       clientes_na_loja;
    Produto         *produtos;
    int             total_produtos;
    SimulacaoTempo  st;
    int             produtos_oferecidos_total;
    float           valor_oferecido_total;
} Supermercado;









#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#define LIMPAR_BUFFER() do { int c; while ((c = getchar()) != '\n' && c != EOF); } while(0)

#endif // SUPERMERCADO_H_INCLUDED
