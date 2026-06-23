#ifndef GRAFO_LA_PAZ_H
#define GRAFO_LA_PAZ_H

#include <vector>
#include <utility>
#include <fstream>
#include <iostream>

extern int NUM_NODOS;
extern double NORTH, SOUTH, EAST, WEST;

struct NodeMetadata {
    double lat;
    double lon;
};

extern std::vector<NodeMetadata> METADATOS_NODOS;
extern std::vector<std::vector<std::pair<int, double>>> MAPA_GRAFO;

bool CargarMapaBinario(const char* rutaArchivo);

#endif // GRAFO_LA_PAZ_H
