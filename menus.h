#ifndef MENUS_H_INCLUDED
#define MENUS_H_INCLUDED

#include "supermercado.h"
#include "estatisticas.h"
#include "funcoes.h"
#include "Ficheiros.h"

// GERAIS
 void limpar_ecra();
 void pausar();
 void ativarANSI();     // mover o cursor para o inicio e escrever por cima


 // SIMULAÇÃO
 void simular (Supermercado *sm, int hora, int minuto, int previstos);
 void menu_inicial (Supermercado *sm);
 void menu_fim_simulacao (Supermercado *sm);
 void pausarSimulacao(Supermercado *sm);
 void menu_configurar_tempo(Supermercado *sm);
 void menu_memoria(Supermercado *sm);


 // ESTATISTICAS
 void menu_estatisticas(Supermercado *sm);
 void menu_estatisticas_anteriores(Supermercado *sm);


// GESTÃO
 void menu_caixas(Supermercado *sm);
 void menu_clientes(Supermercado *sm);


#endif // MENUS_H_INCLUDED
