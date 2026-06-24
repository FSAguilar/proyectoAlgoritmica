#ifndef DJIKSTRA_H
#define DJIKSTRA_H

#include "grafo_la_paz.h"
#include <vector>
#include <utility>
#include <queue>

// Llena 'distancia' con las distancias mínimas desde nodoOrigen a cada nodo.
// Llena 'prev' con el nodo anterior en el camino óptimo (-1 si no hay camino).
void djikstra(int nodoOrigen, std::vector<double> &distancia, std::vector<int> &prev);

// Reconstruye el camino desde origen hasta destino usando el vector prev.
std::vector<int> ReconstruirCamino(const std::vector<int> &prev, int destino);

#endif