#include "../../include/grafo_la_paz.h"

int NUM_NODOS = 0;
double NORTH = 0.0, SOUTH = 0.0, EAST = 0.0, WEST = 0.0;
std::vector<NodeMetadata> METADATOS_NODOS;
std::vector<std::vector<std::pair<int, double>>> MAPA_GRAFO;

bool CargarMapaBinario(const char *rutaArchivo) {
  std::ifstream f(rutaArchivo, std::ios::binary);
  if (!f.is_open()) return false;

  int total_aristas = 0;
  f.read((char *) &NUM_NODOS, sizeof(int));
  f.read((char *) &total_aristas, sizeof(int));
  f.read((char *) &NORTH, sizeof(double));
  f.read((char *) &SOUTH, sizeof(double));
  f.read((char *) &EAST, sizeof(double));
  f.read((char *) &WEST, sizeof(double));

  // Cargar bloque continuo de coordenadas de nodos
  METADATOS_NODOS.resize(NUM_NODOS);
  f.read((char *) METADATOS_NODOS.data(), NUM_NODOS * sizeof(NodeMetadata));

  // Inicializar la lista de adyacencia vacía para rellenar bajo demanda
  MAPA_GRAFO.assign(NUM_NODOS, std::vector<std::pair<int, double>>());

  // Cargar bloque continuo de aristas planas
  for (int i = 0; i < total_aristas; i++) {
    int u = 0, v = 0;
    double w = 0.0;
    f.read((char *) &u, sizeof(int));
    f.read((char *) &v, sizeof(int));
    f.read((char *) &w, sizeof(double));

    // Validación de seguridad para prevenir corrupción de memoria
    if (u >= 0 && u < NUM_NODOS) {
      MAPA_GRAFO[u].push_back({ v, w });
    }
  }
  f.close();
  return true;
}
