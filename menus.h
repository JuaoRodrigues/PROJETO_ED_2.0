#ifndef MENUS_H_INCLUDED
#define MENUS_H_INCLUDED

#include "supermercado.h"
#include "estatisticas.h"
#include "funcoes.h"

 void limpar_ecra();
 void pausar();

 void ativarANSI();
 void simular (Supermercado *sm, int hora, int minuto, int previstos);

 void menu_principal(Supermercado *sm);
 void menu_caixas(Supermercado *sm);
 void menu_clientes(Supermercado *sm);
 void pausarSimulacao(Supermercado *sm);
 void menu_estatisticas(Supermercado *sm);

#endif // MENUS_H_INCLUDED
