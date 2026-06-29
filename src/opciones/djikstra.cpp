#include "../../include/djikstra.h"
#include "../../include/grafo_la_paz.h"
#include <bits/stdc++.h>

using namespace std;

// Ejecuta el algoritmo clásico de Dijkstra para encontrar los caminos más cortos
// desde 'nodoOrigen' a todos los demás nodos del grafo de La Paz.
// Si existe un bloqueo activo ('tieneBloqueo' es true), penaliza fuertemente el peso
// de las calles cuyos destinos caigan dentro del rectángulo delimitado por (lat1, lon1) y (lat2, lon2).
void djikstra(int nodoOrigen, vector<double> &distancia, vector<int> &prev,
              double lat1, double lon1, double lat2, double lon2, bool tieneBloqueo) {
  // Inicializamos todas las distancias en infinito (1e18) y el nodo previo en -1
  distancia.assign(NUM_NODOS, 1e18);
  prev.assign(NUM_NODOS, -1);
  
  // Cola de prioridad (min-heap) para extraer siempre el nodo con menor distancia tentativa acumulada.
  // Almacena pares {distancia_acumulada, ID_nodo}
  priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;
  
  distancia[nodoOrigen] = 0;
  pq.push({ 0.0, nodoOrigen });

  // Si hay bloqueo, normalizamos las coordenadas para definir el rectángulo
  double minLat = 0.0, maxLat = 0.0, minLon = 0.0, maxLon = 0.0;
  if (tieneBloqueo) {
    minLat = min(lat1, lat2);
    maxLat = max(lat1, lat2);
    minLon = min(lon1, lon2);
    maxLon = max(lon1, lon2);
  }

  while (pq.size()) {
    int nodo = pq.top().second;
    double distActual = pq.top().first;
    pq.pop();
    
    // Si ya encontramos un camino más corto a este nodo antes de sacarlo de la cola, lo ignoramos
    if (distActual > distancia[nodo]) continue;

    // Explorar los vecinos del nodo actual
    for (auto &edge : MAPA_GRAFO[nodo]) {
      int nodoDestino = edge.first;
      double peso = edge.second;

      // Si el nodo de destino está en la zona de bloqueo, se le aplica una penalización alta
      // para disuadir al algoritmo de elegir este camino, a menos que sea la única ruta viable
      if (tieneBloqueo) {
        double latV = METADATOS_NODOS[nodoDestino].lat;
        double lonV = METADATOS_NODOS[nodoDestino].lon;
        if (latV >= minLat && latV <= maxLat && lonV >= minLon && lonV <= maxLon) {
          peso += 100000.0; // Penalización alta pero no infinita (100 kilómetros extras de costo)
        }
      }

      // Relajación de la arista
      if (distancia[nodo] + peso < distancia[nodoDestino]) {
        distancia[nodoDestino] = distancia[nodo] + peso;
        prev[nodoDestino] = nodo;
        pq.push({ distancia[nodoDestino], nodoDestino });
      }
    }
  }
}

// Reconstruye hacia atrás el camino desde el nodo de destino hasta el origen
// recorriendo la lista de punteros del vector 'prev', e invierte el resultado final.
vector<int> ReconstruirCamino(const vector<int> &prev, int destino) {
  vector<int> camino;
  for (int nodo = destino; nodo != -1; nodo = prev[nodo]) {
    camino.push_back(nodo);
  }
  // Revertimos el camino para que quede ordenado desde Origen -> Destino
  reverse(camino.begin(), camino.end());
  return camino;
}