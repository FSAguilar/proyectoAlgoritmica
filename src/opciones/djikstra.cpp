#include "../../include/djikstra.h"
#include "../../include/grafo_la_paz.h"
#include <bits/stdc++.h>

using namespace std;

void djikstra(int nodoOrigen, vector<double> &distancia, vector<int> &prev) {
  distancia.assign(NUM_NODOS, 1e18);
  prev.assign(NUM_NODOS, -1);
  priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;
  distancia[nodoOrigen] = 0;
  pq.push({ 0.0, nodoOrigen });
  while (pq.size()) {
    int nodo = pq.top().second;
    double distActual = pq.top().first;
    pq.pop();
    if (distActual > distancia[nodo]) continue;

    for (auto &edge : MAPA_GRAFO[nodo]) {
      int nodoDestino = edge.first;
      double peso = edge.second;
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