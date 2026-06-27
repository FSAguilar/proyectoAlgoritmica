#include "../../include/mochila.h"
#include <algorithm>

using namespace std;

ResultadoMochila resolverMochila(int presupuesto, const vector<Item>& inventario) {
    int n = inventario.size();
    if (presupuesto <= 0 || n == 0) return { 0, 0, {} };

    vector<vector<int>> dp(n + 1, vector<int>(presupuesto + 1, 0));
    
    for(int i = 1; i <= n; i++) {
        for(int j = 1; j <= presupuesto; j++) {
            dp[i][j] = dp[i-1][j];
            if(j - inventario[i-1].costo >= 0) {
                dp[i][j] = max(dp[i][j], dp[i-1][j - inventario[i-1].costo] + inventario[i-1].utilidad);
            }
        }
    }
    
    // Reconstrucción de los elementos óptimos seleccionados
    vector<Item> elegidos;
    int j = presupuesto;
    int costoEfectivo = 0;
    
    for(int i = n; i > 0; i--) {
        if(dp[i-1][j] != dp[i][j]) {
            elegidos.push_back(inventario[i-1]);
            costoEfectivo += inventario[i-1].costo;
            j = j - inventario[i-1].costo;
        }
    }
    
    reverse(elegidos.begin(), elegidos.end());
    return { dp[n][presupuesto], costoEfectivo, elegidos };
}