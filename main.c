#include "supermercado.h"
#include "Ficheiros.h"
#include "menus.h"
#include "tempo.h"
#include "funcoes.h"
#include "estatisticas.h"


#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    ativarANSI();
    limpar_ecra();

    srand(time(NULL));  // tempo a 0
    setlocale(LC_ALL, "portuguese");

    Supermercado super = {0};
    inicializarLoja(&super);

    menu_inicial(&super);

    return 0;
}
