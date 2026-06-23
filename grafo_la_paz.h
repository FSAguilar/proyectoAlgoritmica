#ifndef GRAFO_LA_PAZ_H
#define GRAFO_LA_PAZ_H

#include <vector>
#include <utility>

extern const int NUM_NODOS;
extern const double NORTH;
extern const double SOUTH;
extern const double EAST;
extern const double WEST;

struct NodeMetadata {
  double lat;
  double lon;
};

extern const std::vector<NodeMetadata> METADATOS_NODOS;
extern const std::vector<std::vector<std::pair<int, double>>> MAPA_GRAFO;

#endif
