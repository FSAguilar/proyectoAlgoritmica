#ifndef MOCHILA_H
#define MOCHILA_H

#include <bits/stdc++.h>

// Estructura para los productos o combos
struct Item {
    std::string nombre;
    int costo;
    int utilidad; 
};

// Estructura para almacenar el resultado del algoritmo por supermercado
struct ResultadoMochila {
    int utilidadTotal;
    int costoTotal;
    std::vector<Item> itemsSeleccionados;
};

// Función principal para renderizar y controlar la lógica de la pantalla de la mochila
void drawMochila();

#endif