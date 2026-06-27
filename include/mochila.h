#ifndef MOCHILA_H
#define MOCHILA_H

#include "inventario.h"
#include <vector>

struct ResultadoMochila {
    int utilidadTotal;
    int costoTotal;
    std::vector<Item> itemsSeleccionados;
};

// Algoritmo puro: recibe el presupuesto y un vector de items, devuelve el resultado óptimo
ResultadoMochila resolverMochila(int presupuesto, const std::vector<Item>& inventario);

#endif