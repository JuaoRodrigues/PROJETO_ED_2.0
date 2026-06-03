#ifndef SUPERMERCADO_H_INCLUDED
#define SUPERMERCADO_H_INCLUDED


/* ------------------------------ CONSTANTES ------------------------------*/
#define MAX_NOME            100
#define MAX_CAIXAS          20          // ACHO QUE NAO É USADO
#define HASH_SIZE           100

// PARA A SIMULACAO
#define MIN_CLIENTES_DIA    1000
#define MAX_CLIENTES_DIA    1001

#define MIN_TEMPO_LOJA      10   /* minutos simulados */
#define MAX_TEMPO_LOJA      60   /* minutos simulados */

#define MAX_PRODUTOS_CARRINHO 20


/* ------------------------------ CONFIGURAÇÃO ------------------------------*/
typedef struct {
    int   max_espera;      // em ticks simulados
    int   n_caixas;
    int   tempo_atendimento_produto;    // em segundos simulados
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
    int            tempo_passagem;  // gerado aleatoriamente entre x/2 e x | em segundos simulados
    struct Produto *proximo;
} Produto;


/* ------------------------------ CLIENTE ------------------------------*/
typedef struct Cliente {
    int            id;
    char           nome[MAX_NOME];
    int            n_produtos;
    Produto        *carrinho;           /* lista ligada de produtos */
    int            tick_entrada_fila;   /* sim_time quando entrou na fila */
    int            produto_oferecido;  /* 1 se já recebeu oferta neste atendimento */
    struct Cliente *proximo;           /* próximo cliente na fila */
    int            tick_entrada;
    int            tick_saida;
} Cliente;


/* ------------------------------ FILA ------------------------------*/
typedef struct {
    Cliente *frente;
    Cliente *fim;
    int      tamanho;
} Fila;


/* ------------------------------ LISTA PROD OFERECIDOS ------------------------------*/
typedef struct no_prod_oferecido
{
    Produto                     prod;
    struct no_prod_oferecido    *proximo;
}NODO_prod_oferecido;

typedef struct
{
    NODO_prod_oferecido         *inicio;
    int                         total;
}Lista_ofertas;


/* ------------------------------ LISTA CLIENTES ATENDIDOS POR CAIXA ------------------------------*/
typedef struct NodoClienteAtendido {
    Cliente                     atendido;
    struct NodoClienteAtendido  *proximo;
} NodoClienteAtendido;

typedef struct {
    NodoClienteAtendido         *inicio;
    int                         total;
} ListaClientesAtendidos;

/* ------------------------------ CAIXA ------------------------------*/
typedef struct {
    int    id;
    char   nome[MAX_NOME];          // acho que nao é usado
    char   operador_nome[MAX_NOME];
    int    operador_id;
    // 0 = fechada, 1 = aberta, 2 = a fechar (atende o restante de clientes mas nao permite mais ninguem na fila)
    // 3 = recentemente aberta (nao pode ser fechada nesse tick)
    // 4 = fechada definitivamente (todos os clientes incluindo o da frente sao redistribuidos)
    // 5 = aberta definitivamente (nunca fecha ate ordem contrária)
    int    ativa;
    Fila   fila;
    long   seg_fim_atendimento;     // segundos simulados em que o cliente termina a compra
    int    tick_fim_atendimento;
    int    total_clientes_atendidos;
    int    total_produtos_vendidos;
    float  total_valor_vendido;
    int    produtos_oferecidos;
    float  valor_oferecido;
    Lista_ofertas oferta;
    ListaClientesAtendidos historico;
    int ticks_aberta;       // total de ticks aberta
    int tick_abertura;      // ultimo tick em que abriu
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
    int     hora_abertura;        // da config, ex: 8
    int     hora_fecho;           // da config, ex: 22
    int     ticks_totais;         // total de ticks da simulação
    int     tick_atual;           // tick em que estamos agora
    double  segundos_por_tick;    // tempo real de cada tick
    long    seg_simulados;         // tick atual * 60
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

/* ------------------------------ ESTATISTICAS CLIENTES ------------------------------*/
typedef struct {
    int    total_atendidos;
    int    total_sem_produtos;
    int    total_com_oferta;
    int    total_produtos;
    float  total_gasto;
    long   total_espera;       // soma dos ticks de espera na fila
    long   total_tempo_loja;   // soma dos ticks na loja
} EstatisticasClientes;


/* ---------------------- CLIENTES BANIDOS ---------------------- */
typedef struct NodoBanido {
    int                 id;
    char                nome[MAX_NOME];
    struct NodoBanido   *proximo;
} NodoBanido;

typedef struct {
    NodoBanido  *inicio;
    int         total;
} ListaBanidos;

/* ------------------------------ SUPERMERCADO ------------------------------ */
typedef struct {
    Caixa           caixas[MAX_CAIXAS];
    Configuracao    config;
    HashTable       clientes;
    ListaLoja       clientes_na_loja;
    Produto         *produtos;
    int             total_produtos;
    SimulacaoTempo  st;
    int             produtos_oferecidos_total;
    float           valor_total_ganho;
    float           valor_oferecido_total;
    int             clientesDia;
    int             clientesEntrados;
    int             dia;
    ListaBanidos    banidos;
    EstatisticasClientes est_clientes;
} Supermercado;





#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
    #define SLEEP_MS(ms) Sleep(ms)
    #define KBHIT()      _kbhit()
    #define GETCH()      _getch()
#else
    #include <unistd.h>
    #include <termios.h>
    #include <fcntl.h>
    #define SLEEP_MS(ms) usleep((ms) * 1000)

    static inline void set_terminal_raw(int raw)
    {
        static struct termios oldt;
        if (raw)
        {
            struct termios newt;
            tcgetattr(STDIN_FILENO, &oldt);
            newt = oldt;
            newt.c_lflag &= ~(ICANON | ECHO);
            tcsetattr(STDIN_FILENO, TCSANOW, &newt);
            fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
        }
        else
        {
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            fcntl(STDIN_FILENO, F_SETFL, 0);
        }
    }

    static inline int KBHIT(void)
    {
        char ch;
        int n = read(STDIN_FILENO, &ch, 1);
        if (n > 0) { ungetc(ch, stdin); return 1; }
        return 0;
    }

    static inline int GETCH(void)
    {
        char ch;
        read(STDIN_FILENO, &ch, 1);
        return (int)ch;
    }
#endif


#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ----------------- CORES -----------------
#define VERMELHO    "\033[31m"
#define VERDE       "\033[32m"
#define AMARELO     "\033[33m"
#define RESET       "\033[0m"


#define LIMPAR_BUFFER() do { int c; while ((c = getchar()) != '\n' && c != EOF); } while(0)

#endif // SUPERMERCADO_H_INCLUDED
