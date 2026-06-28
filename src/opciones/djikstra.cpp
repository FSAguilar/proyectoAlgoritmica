#include "../../include/djikstra.h"
#include "../../include/grafo_la_paz.h"
#include <bits/stdc++.h>

using namespace std;

void djikstra(int nodoOrigen, vector<double> &distancia, vector<int> &prev,
              double lat1, double lon1, double lat2, double lon2, bool tieneBloqueo) {
  distancia.assign(NUM_NODOS, 1e18);
  prev.assign(NUM_NODOS, -1);
  priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;
  distancia[nodoOrigen] = 0;
  pq.push({ 0.0, nodoOrigen });

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
    if (distActual > distancia[nodo]) continue;

    for (auto &edge : MAPA_GRAFO[nodo]) {
      int nodoDestino = edge.first;
      double peso = edge.second;

      if (tieneBloqueo) {
        double latV = METADATOS_NODOS[nodoDestino].lat;
        double lonV = METADATOS_NODOS[nodoDestino].lon;
        if (latV >= minLat && latV <= maxLat && lonV >= minLon && lonV <= maxLon) {
          peso += 100000.0; // Penalización alta pero no infinita
        }
      }

      if (distancia[nodo] + peso < distancia[nodoDestino]) {
        distancia[nodoDestino] = distancia[nodo] + peso;
        prev[nodoDestino] = nodo;
        pq.push({ distancia[nodoDestino], nodoDestino });
      }
    }
  }
}

vector<int> ReconstruirCamino(const vector<int> &prev, int destino) {
  vector<int> camino;
  for (int nodo = destino; nodo != -1; nodo = prev[nodo]) {
    camino.push_back(nodo);
  }
  reverse(camino.begin(), camino.end());
  return camino;
}