// #include "../../include/mochila.h"
#include "../../include/utils.h"
#include <bits/stdc++.h>

using namespace std;

int numeroDeObjetos;
int pesoMochila;

int main1() {
  while (cin >> pesoMochila >> numeroDeObjetos) {

    int pesos[numeroDeObjetos];
    int precios[numeroDeObjetos];
    for (int i = 0; i < numeroDeObjetos; i++) {
      cin >> precios[i];
      cin >> pesos[i];
    }

    int dp[numeroDeObjetos + 1][pesoMochila + 1];
    memset(dp, 0, sizeof dp);

    for (int i = 1; i <= numeroDeObjetos; i++) {
      for (int j = 1; j <= pesoMochila; j++) {
        dp[i][j] = dp[i - 1][j];
        if (j - pesos[i - 1] >= 0) {
          dp[i][j] = max(dp[i][j], dp[i - 1][j - pesos[i - 1]] + precios[i - 1]);
        }
      }
    }
    vector<int> Objetos;

    int j = pesoMochila;
    for (int i = numeroDeObjetos; i >= 0; i--) {
      if (i != 0) {
        if (dp[i - 1][j] != dp[i][j]) {

          Objetos.push_back(i);

          j = j - pesos[i - 1];
        }
      }
    }
    cout << Objetos.size() << endl;

    for (int i = Objetos.size() - 1; i >= 0; i--) {
      cout << Objetos[i] - 1 << " ";
    }
    cout << endl;
  }
  return 0;
}

// // Estructura para los productos
// struct Item {
//     string nombre;
//     int costo;
//     int utilidad; // Alta para víveres (10), baja para snacks (2)
// };

// // Estructura para guardar el resultado de cada supermercado
// struct ResultadoMochila {
//     int utilidadTotal;
//     int costoTotal;
//     vector<Item> itemsSeleccionados;
// };

// // TU ALGORITMO ADAPTADO: Ahora es una función reutilizable
// ResultadoMochila resolverMochila(int presupuesto, const vector<Item>& inventario) {
//     int n = inventario.size();

//     // Matriz de DP (usamos vector para evitar problemas de memoria dinámica/VLA)
//     vector<vector<int>> dp(n + 1, vector<int>(presupuesto + 1, 0));

//     // Llenado de la tabla (Tu misma lógica)
//     for(int i = 1; i <= n; i++) {
//         for(int j = 1; j <= presupuesto; j++) {
//             dp[i][j] = dp[i-1][j];
//             if(j - inventario[i-1].costo >= 0) {
//                 dp[i][j] = max(dp[i][j], dp[i-1][j - inventario[i-1].costo] + inventario[i-1].utilidad);
//             }
//         }
//     }

//     // Reconstrucción del camino (Tu misma lógica)
//     vector<Item> elegidos;
//     int j = presupuesto;
//     int costoEfectivo = 0;

//     for(int i = n; i > 0; i--) {
//         if(dp[i-1][j] != dp[i][j]) {
//             elegidos.push_back(inventario[i-1]);
//             costoEfectivo += inventario[i-1].costo;
//             j = j - inventario[i-1].costo;
//         }
//     }

//     // Invertimos para que aparezcan en el orden original
//     reverse(elegidos.begin(), elegidos.end());

//     return { dp[n][presupuesto], costoEfectivo, elegidos };
// }
