#ifndef INVENTARIO_H
#define INVENTARIO_H

#include <bits/stdc++.h>

struct Item {
    std::string nombre;
    int costo;
    int utilidad;       // Valor interno del algoritmo (no se muestra al usuario)
    std::string categoria; // Categoría legible para la UI
};

// Función que devuelve el catálogo exclusivo de Hipermaxi
std::vector<Item> obtenerInventarioHipermaxi();

#endif