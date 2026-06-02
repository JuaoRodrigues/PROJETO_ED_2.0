#ifndef MENUS_H_INCLUDED
#define MENUS_H_INCLUDED

#include "supermercado.h"
#include "estatisticas.h"
#include "funcoes.h"
#include "Ficheiros.h"

 void limpar_ecra();
 void pausar();

 void ativarANSI();
 void simular (Supermercado *sm, int hora, int minuto, int previstos);

 void menu_principal(Supermercado *sm);
 void menu_caixas(Supermercado *sm);
 void menu_clientes(Supermercado *sm);
 void pausarSimulacao(Supermercado *sm);
 void menu_estatisticas(Supermercado *sm);
 void menu_estatisticas_anteriores(Supermercado *sm);
 void menu_configurar_tempo(Supermercado *sm);
 void menu_memoria(Supermercado *sm);

 void menu_inicial (Supermercado *sm);
 void menu_fim_simulacao (Supermercado *sm);

#endif // MENUS_H_INCLUDED
