#ifndef INVENTARIO_H
#define INVENTARIO_H

#include <bits/stdc++.h>

struct Item {
    std::string nombre;
    int costo;
    int utilidad;       // Valor interno del algoritmo (no se muestra al usuario)
    std::string categoria; // Categoría legible para la UI
};

// Agrupa el nombre del supermercado con su catálogo de productos disponibles
struct Mercado {
    std::string nombre;
    std::vector<Item> catalogo;
};

// Devuelve todos los supermercados disponibles, cada uno con su catálogo propio
std::vector<Mercado> obtenerTodosLosMercados();

#endif