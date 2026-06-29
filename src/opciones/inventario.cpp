#include "../../include/inventario.h"

using namespace std;

// Retorna un vector con todos los supermercados reales de La Paz disponibles,
// cada uno con su propio catálogo de productos, precios en bolivianos y utilidades subjetivas.
// La utilidad representa el nivel de necesidad/prioridad del artículo:
// (Alimentos de primera necesidad > Lácteos y Carnes > Artículos de limpieza > Snacks)
vector<Mercado> obtenerTodosLosMercados() {

    // ══════════════════════════════════════════════════════════════════════════
    // HIPERMAXI — Cadena líder de supermercados en Bolivia
    // Precios orientativos basados en sus sucursales de La Paz (zona Sur / San Miguel)
    // ══════════════════════════════════════════════════════════════════════════
    Mercado hipermaxi;
    hipermaxi.nombre = "Hipermaxi";
    hipermaxi.catalogo = {
        // ── Básicos de despensa (Utilidades altas: 8 - 10) ──────────────────────────
        {"Arroz Supremo 1kg",       13,  10, "Despensa"},
        {"Fideo Don Vittorio 400g", 10,  10, "Despensa"},
        {"Aceite Fino 900ml",       18,   9, "Despensa"},
        {"Azúcar blanca 1kg",        8,   8, "Despensa"},
        {"Sal 1kg",                  8,   8, "Despensa"},
        {"Harina de trigo 1kg",     11,   8, "Despensa"},
        {"Lenteja 450g",            22,   8, "Despensa"},

        // ── Lácteos y proteína (Utilidades altas: 5 - 10) ───────────────────────────
        {"Leche PIL 800ml",         9,    9, "Lácteos"},
        {"Huevos (cartón 12u)",     14,  10, "Lácteos"},
        {"Yogur PIL 1L",            25,   6, "Lácteos"},
        {"Queso crema 200g",        30,   5, "Lácteos"},
        {"Queso criollo 200g",      25,   7, "Lácteos"},

        // ── Carnes y embutidos (Utilidades medias-altas: 6 - 9) ─────────────────────
        {"Pollo entero ~1.5kg",     25,   9, "Carnes"},
        {"Salchicha Sofía (10u)",   20,   6, "Carnes"},
        {"Atún Real lata 170g",     23,   7, "Carnes"},
        {"Sardina San Lucas 425g",  20,   7, "Carnes"},
        {"Lomo de res 1kg",         89,   8, "Carnes"},
        {"Carne molida 1kg",        49,   9, "Carnes"},
        {"Chuleta de cerdo 1kg",    44,   8, "Carnes"},

        // ── Higiene y limpieza (Utilidades medias: 6 - 8) ───────────────────────────
        {"Detergente Bolívar 700g", 21,   8, "Limpieza"},
        {"Jabón de tocador x3",     22,   7, "Limpieza"},
        {"Papel higiénico x6",      10,   8, "Limpieza"},
        {"Lavavajillas Ola 750g",   13,   6, "Limpieza"},

        // ── Bebidas y snacks (Utilidades más bajas: 3 - 10) ─────────────────────────
        {"Café Copacabana 250g",    27,   5, "Bebidas"},
        {"Té Windsor x50 sob.",     20,   7, "Bebidas"},
        {"Coca Cola 3L",            18,   3, "Bebidas"},
        {"Agua 5L",                 18,  10, "Bebidas"},
        {"Galletas Oreo 216g",      22,   3, "Snacks"},
        {"Papas Fritas 200g",       16,   3, "Snacks"},
        {"Avena La Suprema 800g",   23,   6, "Snacks"},
    };

    // ══════════════════════════════════════════════════════════════════════════
    // KETAL — Supermercado tradicional de La Paz, con sucursales en zona central y sur
    // Precios ligeramente más elevados que Hipermaxi en varios productos
    // ══════════════════════════════════════════════════════════════════════════
    Mercado ketal;
    ketal.nombre = "Ketal";
    ketal.catalogo = {
        // ── Despensa ────────────────────────────────────────────────────────────────
        {"Arroz Grano de Oro 1kg",   14, 10, "Despensa"},
        {"Fideo Carozzi 400g",       12, 10, "Despensa"},
        {"Aceite Fino 900ml",        19,  9, "Despensa"},
        {"Azúcar Bermejo 1kg",        9,  8, "Despensa"},
        {"Sal Lobos 1kg",             7,  8, "Despensa"},
        {"Harina Selecta 1kg",       12,  8, "Despensa"},
        {"Quinua Real 500g",         28,  9, "Despensa"},
        {"Avena Quaker 600g",        22,  7, "Despensa"},

        // ── Lácteos ─────────────────────────────────────────────────────────────────
        {"Leche PIL entera 1L",      11,  9, "Lácteos"},
        {"Huevos (cartón 12u)",      15, 10, "Lácteos"},
        {"Yogur Soalpro 1L",         23,  6, "Lácteos"},
        {"Queso holandés 250g",      35,  5, "Lácteos"},
        {"Mantequilla PIL 200g",     18,  6, "Lácteos"},

        // ── Carnes ──────────────────────────────────────────────────────────────────
        {"Pollo entero ~1.5kg",      27,  9, "Carnes"},
        {"Pechuga de pollo 1kg",     38,  9, "Carnes"},
        {"Atún Van Camps 170g",      25,  7, "Carnes"},
        {"Lomo de res 1kg",          95,  8, "Carnes"},
        {"Carne molida 1kg",         52,  9, "Carnes"},
        {"Chorizo parrillero 500g",  32,  5, "Carnes"},

        // ── Limpieza ────────────────────────────────────────────────────────────────
        {"Detergente Ace 800g",      24,  8, "Limpieza"},
        {"Jabón Palmolive x3",       25,  7, "Limpieza"},
        {"Papel higiénico Elite x4", 15,  8, "Limpieza"},
        {"Cloro Clorox 1L",          12,  6, "Limpieza"},
        {"Desodorante amb. Glade",   18,  4, "Limpieza"},

        // ── Bebidas ─────────────────────────────────────────────────────────────────
        {"Café Copacabana 250g",     28,  5, "Bebidas"},
        {"Té McColin's x50 sob.",    22,  7, "Bebidas"},
        {"Pepsi 3L",                 17,  3, "Bebidas"},
        {"Agua Vital 6L",            20, 10, "Bebidas"},
        {"Jugo Tampico 1L",          12,  4, "Bebidas"},
    };

    // ══════════════════════════════════════════════════════════════════════════
    // FIDALGA — Supermercado con fuerte presencia online y delivery en La Paz
    // Posicionamiento intermedio en precios; variedad en productos importados
    // ══════════════════════════════════════════════════════════════════════════
    Mercado fidalga;
    fidalga.nombre = "Fidalga";
    fidalga.catalogo = {
        // ── Despensa ────────────────────────────────────────────────────────────────
        {"Arroz Koky 1kg",           12, 10, "Despensa"},
        {"Fideo Lavazza 500g",       11, 10, "Despensa"},
        {"Aceite Girasol 900ml",     17,  9, "Despensa"},
        {"Azúcar blanca 1kg",         8,  8, "Despensa"},
        {"Sal yodada 1kg",            6,  8, "Despensa"},
        {"Maní pelado 250g",         15,  6, "Despensa"},
        {"Lenteja 500g",             20,  8, "Despensa"},
        {"Avena 3 Ositos 600g",      19,  7, "Despensa"},

        // ── Lácteos ─────────────────────────────────────────────────────────────────
        {"Leche PIL 1L",             10,  9, "Lácteos"},
        {"Huevos (cartón 12u)",      13, 10, "Lácteos"},
        {"Yogur Delizia 1L",         21,  6, "Lácteos"},
        {"Queso mozzarella 250g",    28,  5, "Lácteos"},
        {"Mantequilla Delicia 200g", 16,  6, "Lácteos"},
        {"Leche condensada 395g",    14,  4, "Lácteos"},

        // ── Carnes ──────────────────────────────────────────────────────────────────
        {"Pollo entero ~1.5kg",      24,  9, "Carnes"},
        {"Milanesa de pollo 500g",   22,  7, "Carnes"},
        {"Atún Real lata 170g",      22,  7, "Carnes"},
        {"Lomo de res 1kg",          85,  8, "Carnes"},
        {"Carne molida 1kg",         47,  9, "Carnes"},
        {"Salchicha Sofía (10u)",    18,  6, "Carnes"},

        // ── Limpieza ────────────────────────────────────────────────────────────────
        {"Detergente Omo 700g",      20,  8, "Limpieza"},
        {"Jabón Lux x3",            19,   7, "Limpieza"},
        {"Papel higiénico x6",       9,   8, "Limpieza"},
        {"Suavizante Aromatel 1L",  16,   5, "Limpieza"},

        // ── Bebidas ─────────────────────────────────────────────────────────────────
        {"Café Nescafé 100g",       32,   5, "Bebidas"},
        {"Té Windsor x50 sob.",     19,   7, "Bebidas"},
        {"Coca Cola 2.5L",          16,   3, "Bebidas"},
        {"Agua Viva 5L",            15,  10, "Bebidas"},
        {"Jugo Del Valle 1L",       14,   4, "Bebidas"},
        {"Chocolate Ceibo 90g",     25,   3, "Snacks"},
    };

    return { hipermaxi, ketal, fidalga };
}