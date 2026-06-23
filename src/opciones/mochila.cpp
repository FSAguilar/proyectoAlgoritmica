#include "../../include/mochila.h"
#include "../../include/utils.h"
#include "raylib.h"
#include <bits/stdc++.h>

using namespace std;

// Enum interno para manejar las sub-pantallas
enum SubEstado { SUB_INPUT,
                 SUB_RESULTADOS };

// Función interna del algoritmo de la mochila (Tu lógica DP óptima)
ResultadoMochila resolverMochila(int presupuesto, const vector<Item> &inventario) {
  int n = inventario.size();
  if (presupuesto <= 0) return { 0, 0, {} };

  vector<vector<int>> dp(n + 1, vector<int>(presupuesto + 1, 0));

  for (int i = 1; i <= n; i++) {
    for (int j = 1; j <= presupuesto; j++) {
      dp[i][j] = dp[i - 1][j];
      if (j - inventario[i - 1].costo >= 0) {
        dp[i][j] = max(dp[i][j], dp[i - 1][j - inventario[i - 1].costo] + inventario[i - 1].utilidad);
      }
    }
  }

  vector<Item> elegidos;
  int j = presupuesto;
  int costoEfectivo = 0;

  for (int i = n; i > 0; i--) {
    if (dp[i - 1][j] != dp[i][j]) {
      elegidos.push_back(inventario[i - 1]);
      costoEfectivo += inventario[i - 1].costo;
      j = j - inventario[i - 1].costo;
    }
  }

  reverse(elegidos.begin(), elegidos.end());
  return { dp[n][presupuesto], costoEfectivo, elegidos };
}

void drawMochila() {
  // Variables persistentes entre frames gracias a 'static'
  static SubEstado estadoActual = SUB_INPUT;
  static string presupuestoStr = "";
  static int presupuestoFinal = 0;
  static bool cajaActiva = false;

  // Resultados de los supermercados
  static ResultadoMochila resHiper, resKetal, resAndys;

  // Inventarios fijos de prueba (Se pueden ajustar los datos)
  static const vector<Item> hipermaxi = { { "Arroz 1kg", 15, 10 }, { "Fideo 1kg", 12, 10 }, { "Combo Sodas", 25, 3 }, { "Aceite", 18, 9 }, { "Galletas Pack", 10, 4 } };
  static const vector<Item> ketal = { { "Arroz Prem.", 18, 10 }, { "Fideo Ital.", 16, 10 }, { "Pack Leche", 30, 9 }, { "Snacks Finos", 15, 2 }, { "Yogurt Fam.", 22, 7 } };
  static const vector<Item> andys = { { "Combo Fideo", 22, 10 }, { "Arroz Granel", 10, 8 }, { "Refresco Ya", 5, 2 }, { "Atun familiar", 14, 9 }, { "Pan de Molde", 11, 7 } };

  vector<Button> buttons;

  if (estadoActual == SUB_INPUT) {
    text("OPTIMIZADOR DE COMPRAS (ALGORITMO DE LA MOCHILA)", 0, -180, 24, DARKGRAY);
    text("Haz clic en la caja morada e ingresa tu presupuesto:", 0, -100, 18, GRAY);

    // Dibujar la caja de texto usando tus coordenadas relativas (Centro 0,0)
    // La caja estará en (0, -30) con ancho 200 y alto 40
    Color colorCaja = cajaActiva ? PURPLE : LIGHTGRAY;
    Rect(0, -30, 200, 40, colorCaja);

    if (!presupuestoStr.empty()) {
      text((presupuestoStr + " Bs").c_str(), 0, -30, 20, WHITE);
    } else {
      text("Ej. 150", 0, -30, 20, DARKGRAY);
    }

    // Manejo de clicks para activar la caja de texto manual
    pair<int, int> posCaja = Coord(0, -30);
    int mx = GetMouseX(), my = GetMouseY();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      if (mx >= posCaja.first - 100 && mx <= posCaja.first + 100 && my >= posCaja.second - 20 && my <= posCaja.second + 20) {
        cajaActiva = true;
      } else {
        cajaActiva = false;
      }
    }

    // Captura de Teclado numérico
    if (cajaActiva) {
      int key = GetCharPressed();
      while (key > 0) {
        if (key >= '0' && key <= '9' && presupuestoStr.length() < 4) { // Máximo 9999 Bs
          presupuestoStr += (char) key;
        }
        key = GetCharPressed();
      }
      if (IsKeyPressed(KEY_BACKSPACE) && !presupuestoStr.empty()) {
        presupuestoStr.pop_back();
      }
    }

    // Botón Calcular usando tu lógica estructurada de botones
    addButton(0, 50, 160, 40, "Calcular", buttons);

    if ((isButtonPressed(buttons[0]) || (cajaActiva && IsKeyPressed(KEY_ENTER))) && !presupuestoStr.empty()) {
      presupuestoFinal = stoi(presupuestoStr);

      // Procesamiento en cascada para las 3 opciones
      resHiper = resolverMochila(presupuestoFinal, hipermaxi);
      resKetal = resolverMochila(presupuestoFinal, ketal);
      resAndys = resolverMochila(presupuestoFinal, andys);

      estadoActual = SUB_RESULTADOS;
    }
  } else if (estadoActual == SUB_RESULTADOS) {
    text(("Resultados para un presupuesto de: " + to_string(presupuestoFinal) + " Bs").c_str(), 0, -220, 22, MAROON);

    // Encontrar la mejor utilidad para destacar el supermercado ganador
    int maxUtilidad = max({ resHiper.utilidadTotal, resKetal.utilidadTotal, resAndys.utilidadTotal });

    // Configuración de las 3 columnas usando el sistema relativo de utils
    int anioCol = 260, altoCol = 320;
    int yCol = 0; // Centrado en la pantalla horizontal

    // --- COLUMNA 1: HIPERMAXI (Izquierda: x = -300) ---
    Color colHiper = (resHiper.utilidadTotal == maxUtilidad && resHiper.utilidadTotal > 0) ? LIME : LIGHTGRAY;
    Rect(-300, yCol, anioCol, altoCol, colHiper);
    // Texto manual calculado con posiciones fijas relativas a su columna
    DrawText("HIPERMAXI", Coord(-410, -140).first, Coord(-410, -140).second, 20, BLACK);
    DrawText(TextFormat("Utilidad: %d", resHiper.utilidadTotal), Coord(-410, -100).first, Coord(-410, -100).second, 16, DARKGRAY);
    DrawText(TextFormat("Gasto: %d Bs", resHiper.costoTotal), Coord(-410, -80).first, Coord(-410, -80).second, 16, DARKGRAY);
    int offset = -40;
    for (const auto &item : resHiper.itemsSeleccionados) {
      DrawText(("- " + item.nombre).c_str(), Coord(-410, offset).first, Coord(-410, offset).second, 14, BLACK);
      offset += 20;
    }

    // --- COLUMNA 2: KETAL (Centro: x = 0) ---
    Color colKetal = (resKetal.utilidadTotal == maxUtilidad && resKetal.utilidadTotal > 0) ? LIME : LIGHTGRAY;
    Rect(0, yCol, anioCol, altoCol, colKetal);
    DrawText("KETAL", Coord(-110, -140).first, Coord(-110, -140).second, 20, BLACK);
    DrawText(TextFormat("Utilidad: %d", resKetal.utilidadTotal), Coord(-110, -100).first, Coord(-110, -100).second, 16, DARKGRAY);
    DrawText(TextFormat("Gasto: %d Bs", resKetal.costoTotal), Coord(-110, -80).first, Coord(-110, -80).second, 16, DARKGRAY);
    offset = -40;
    for (const auto &item : resKetal.itemsSeleccionados) {
      DrawText(("- " + item.nombre).c_str(), Coord(-110, offset).first, Coord(-110, offset).second, 14, BLACK);
      offset += 20;
    }

    // --- COLUMNA 3: ANDY'S (Derecha: x = 300) ---
    Color colAndys = (resAndys.utilidadTotal == maxUtilidad && resAndys.utilidadTotal > 0) ? LIME : LIGHTGRAY;
    Rect(300, yCol, anioCol, altoCol, colAndys);
    DrawText("ANDY'S", Coord(190, -140).first, Coord(190, -140).second, 20, BLACK);
    DrawText(TextFormat("Utilidad: %d", resAndys.utilidadTotal), Coord(190, -100).first, Coord(190, -100).second, 16, DARKGRAY);
    DrawText(TextFormat("Gasto: %d Bs", resAndys.costoTotal), Coord(190, -80).first, Coord(190, -80).second, 16, DARKGRAY);
    offset = -40;
    for (const auto &item : resAndys.itemsSeleccionados) {
      DrawText(("- " + item.nombre).c_str(), Coord(190, offset).first, Coord(190, offset).second, 14, BLACK);
      offset += 20;
    }

    // Botón para reiniciar
    addButton(0, 210, 180, 40, "Volver", buttons);
    if (isButtonPressed(buttons[0])) {
      presupuestoStr = "";
      estadoActual = SUB_INPUT;
    }
  }
}
