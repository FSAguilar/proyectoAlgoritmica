#include "../../include/mochila.h"
#include <algorithm>

using namespace std;

// Resuelve el problema clásico de la Mochila 0-1 utilizando Programación Dinámica.
// Dado un presupuesto máximo en bolivianos (Bs) y un catálogo de productos disponibles,
// determina el subconjunto óptimo de productos que maximiza la utilidad subjetiva acumulada
// sin exceder el presupuesto establecido.
ResultadoMochila resolverMochila(int presupuesto, const vector<Item>& inventario) {
    int n = inventario.size();
    // Caso base: si no hay presupuesto o el catálogo está vacío, no se puede comprar nada
    if (presupuesto <= 0 || n == 0) return { 0, 0, {} };

    // Matriz de programación dinámica.
    // dp[i][j] representa la utilidad máxima que se puede obtener considerando los
    // primeros 'i' elementos con un presupuesto máximo de 'j' bolivianos.
    vector<vector<int>> dp(n + 1, vector<int>(presupuesto + 1, 0));
    
    // Rellenamos la tabla DP iterativamente (Bottom-Up)
    for(int i = 1; i <= n; i++) {
        for(int j = 1; j <= presupuesto; j++) {
            // Opción A: No llevar el producto 'i-1'. Se conserva la utilidad óptima anterior.
            dp[i][j] = dp[i-1][j];
            
            // Opción B: Llevar el producto 'i-1' (solo si su costo no excede el presupuesto temporal 'j').
            if(j - inventario[i-1].costo >= 0) {
                // Comparamos el beneficio de no llevarlo vs llevarlo (sumando su utilidad al estado optimo restante)
                dp[i][j] = max(dp[i][j], dp[i-1][j - inventario[i-1].costo] + inventario[i-1].utilidad);
            }
        }
    }
    
    // ── Reconstrucción del carrito óptimo seleccionado ──────────────────
    // Recorremos la tabla DP hacia atrás desde el estado óptimo final (dp[n][presupuesto])
    // para identificar qué productos específicos causaron los incrementos de utilidad.
    vector<Item> elegidos;
    int j = presupuesto;
    int costoEfectivo = 0;
    
    for(int i = n; i > 0; i--) {
        // Si el valor cambia respecto a la fila anterior (dp[i-1][j] != dp[i][j]),
        // significa que el elemento 'i-1' fue incluido de forma obligatoria en la solución óptima.
        if(dp[i-1][j] != dp[i][j]) {
            elegidos.push_back(inventario[i-1]);
            costoEfectivo += inventario[i-1].costo; // Sumamos su costo al gasto real
            j = j - inventario[i-1].costo;          // Restamos el costo del presupuesto restante
        }
    }
    
    // Invertimos el orden para que los productos aparezcan en su secuencia original del catálogo
    reverse(elegidos.begin(), elegidos.end());
    
    // Devolvemos el paquete de resultados: utilidad acumulada, costo total efectivo y lista de items
    return { dp[n][presupuesto], costoEfectivo, elegidos };
}