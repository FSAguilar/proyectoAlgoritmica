#include "../../include/grafo_la_paz.h"

// Número total de nodos presentes en el grafo de La Paz
int NUM_NODOS = 0;

// Límites geográficos dinámicos de la vista actual del mapa (se alteran por Zoom y Pan)
double NORTH = 0.0, SOUTH = 0.0, EAST = 0.0, WEST = 0.0;

// Almacena las coordenadas geográficas (latitud, longitud) de cada nodo usando su ID continuo
std::vector<NodeMetadata> METADATOS_NODOS;

// Lista de adyacencia del grafo del mapa de La Paz.
// Para cada nodo u, almacena pares de {nodo_destino, distancia_en_metros}
std::vector<std::vector<std::pair<int, double>>> MAPA_GRAFO;

// Lee la estructura del mapa serializado en formato binario plano (.bin)
// para cargar eficientemente en memoria los nodos, aristas y límites del mapa
bool CargarMapaBinario(const char *rutaArchivo) {
  std::ifstream f(rutaArchivo, std::ios::binary);
  if (!f.is_open()) return false;

  int total_aristas = 0;

  // 1. Leer cabecera de control: número de nodos y total de aristas
  f.read((char *) &NUM_NODOS, sizeof(int));
  f.read((char *) &total_aristas, sizeof(int));

  // 2. Leer límites geográficos originales del bounding box del mapa
  f.read((char *) &NORTH, sizeof(double));
  f.read((char *) &SOUTH, sizeof(double));
  f.read((char *) &EAST, sizeof(double));
  f.read((char *) &WEST, sizeof(double));

  // 3. Reservar memoria y cargar en un solo bloque los metadatos de coordenadas de todos los nodos
  METADATOS_NODOS.resize(NUM_NODOS);
  f.read((char *) METADATOS_NODOS.data(), NUM_NODOS * sizeof(NodeMetadata));

  // 4. Inicializar la lista de adyacencia del grafo vacía
  MAPA_GRAFO.assign(NUM_NODOS, std::vector<std::pair<int, double>>());

  // 5. Cargar secuencialmente cada una de las aristas (origen, destino, distancia en metros)
  for (int i = 0; i < total_aristas; i++) {
    int u = 0, v = 0;
    double w = 0.0;
    f.read((char *) &u, sizeof(int));
    f.read((char *) &v, sizeof(int));
    f.read((char *) &w, sizeof(double));

    // Validación de seguridad para evitar accesos fuera de rango o corrupción de memoria
    if (u >= 0 && u < NUM_NODOS) {
      MAPA_GRAFO[u].push_back({ v, w });
    }
  }
  
  f.close();
  return true;
}
