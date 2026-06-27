#include "../../include/inventario.h"

using namespace std;

vector<Item> obtenerInventarioHipermaxi() {
    // utilidad = prioridad interna del algoritmo (básicos > lácteos > limpieza > snacks)
    vector<Item> catalogo = {
        // ── Básicos de despensa ──────────────────────────────────────
        {"Arroz Supremo 1kg",       13,  10, "Despensa"},
        {"Fideo Don Vittorio 400g", 10,  10, "Despensa"},
        {"Aceite Fino 900ml",       18,   9, "Despensa"},
        {"Azúcar blanca 1kg",        8,   8, "Despensa"},
        {"Sal 1kg",                  8,   8, "Despensa"},
        {"Harina de trigo 1kg",     11,   8, "Despensa"},
        {"Lenteja 450g",            22,   8, "Despensa"},

        // ── Lácteos y proteína ───────────────────────────────────────
        {"Leche PIL 800ml",         9,    9, "Lácteos"},
        {"Huevos (cartón 12u)",     14,  10, "Lácteos"},
        {"Yogur PIL 1L",            25,   6, "Lácteos"},
        {"Queso crema 200g",        30,   5, "Lácteos"},
        {"Queso criollo 200g",      25,   7, "Lácteos"},

        // ── Carnes y embutidos ───────────────────────────────────────
        {"Pollo entero ~1.5kg",     25,   9, "Carnes"},
        {"Salchicha Sofía (10u)",   20,   6, "Carnes"},
        {"Atún Real lata 170g",     23,   7, "Carnes"},
        {"Sardina San Lucas 425g",  20,   7, "Carnes"},
        {"Lomo de res 1kg",         89,   8, "Carnes"},
        {"Carne molida 1kg",        49,   9, "Carnes"},
        {"Chuleta de cerdo 1kg",    44,   8, "Carnes"},

        // ── Higiene y limpieza ───────────────────────────────────────
        {"Detergente Bolívar 700g", 21,   8, "Limpieza"},
        {"Jabón de tocador x3",     22,   7, "Limpieza"},
        {"Papel higiénico x6",      10,   8, "Limpieza"},
        {"Lavavajillas Ola 750g",   13,   6, "Limpieza"},

        // ── Bebidas y snacks ─────────────────────────────────────────
        {"Café Copacabana 250g",    27,   5, "Bebidas"},
        {"Té Windsor x50 sob.",     20,   7, "Bebidas"},
        {"Coca Cola 3L",            18,   3, "Bebidas"},
        {"Agua 5L",                 18,   10, "Bebidas"},
        {"Galletas Oreo 216g",      22,   3, "Snacks"},
        {"Papas Fritas 200g",       16,   3, "Snacks"},
        {"Avena La Suprema 800g",   23,   6, "Snacks"},
    };
    return catalogo;
}