#include "../../include/opcion1.h"
#include "../../include/utils.h"
#include "../../include/grafo_la_paz.h" // Cabecera sincronizada con el nuevo nombre
#include <bits/stdc++.h>

using namespace std;

// Transforma coordenadas geográficas a píxeles relativos al centro (0,0) de tus utils
// Transforma coordenadas geográficas a píxeles RELATIVOS al centro (0,0) que espera tu función Coord()
Vector2 MapGeoToRelativeScreen(double lat, double lon) {
  int w = GetScreenWidth();
  int h = GetScreenHeight();

  // 1. Mapear lat/lon a una posición lineal entre 0 y el ancho/alto de la pantalla
  float absX = (float) ((lon - WEST) / (EAST - WEST) * w);
  float absY = (float) ((NORTH - lat) / (NORTH - SOUTH) * h);

  // 2. Convertirlo a coordenadas relativas al centro (0,0)
  // Dejamos un margen del 10% (multiplicando por 0.8) para que el mapa no toque los bordes de la ventana
  float relX = (absX - (w / 2.0f)) * 0.8f;
  float relY = (absY - (h / 2.0f)) * 0.8f;

  return Vector2{ relX, relY };
}

// Convierte la posición absoluta del cursor del mouse a coordenadas reales (Lat, Lon) de forma invertida
void MapMouseToGeo(float mouseX, float mouseY, double &outLat, double &outLon) {
  int w = GetScreenWidth();
  int h = GetScreenHeight();

  // 1. Revertir el sistema relativo centrado a píxeles absolutos tradicionales (0 a ancho)
  float absX = ((mouseX - (w / 2.0f)) / 0.8f) + (w / 2.0f);
  float absY = ((mouseY - (h / 2.0f)) / 0.8f) + (h / 2.0f);

  // 2. Interpolar para recuperar los doubles geográficos exactos
  outLon = WEST + (absX / (double) w) * (EAST - WEST);
  outLat = NORTH - (absY / (double) h) * (NORTH - SOUTH);
}

// Variables persistentes entre fotogramas para guardar la información del pin seleccionado
static bool mapaClickeado = false;
static double lastLat = 0.0;
static double lastLon = 0.0;

void drawOpcion1() {
  // --- 1. RENDERIZADO DE CALLES Y AVENIDAS (GRAFO) ---
  for (int u = 0; u < NUM_NODOS; u++) {
    Vector2 p1 = MapGeoToRelativeScreen(METADATOS_NODOS[u].lat, METADATOS_NODOS[u].lon);
    pair<int, int> absP1 = Coord((int) p1.x, (int) p1.y);

    for (const auto &edge : MAPA_GRAFO[u]) {
      int v = edge.first;

      Vector2 p2 = MapGeoToRelativeScreen(METADATOS_NODOS[v].lat, METADATOS_NODOS[v].lon);
      pair<int, int> absP2 = Coord((int) p2.x, (int) p2.y);

      DrawLine(absP1.first, absP1.second, absP2.first, absP2.second, DARKGRAY);
    }
  }

  // Dibujar intersecciones urbanas (Nodos)
  for (int i = 0; i < NUM_NODOS; i++) {
    Vector2 p = MapGeoToRelativeScreen(METADATOS_NODOS[i].lat, METADATOS_NODOS[i].lon);
    pair<int, int> absP = Coord((int) p.x, (int) p.y);
    DrawCircle(absP.first, absP.second, 1.2f, MAROON);
  }

  // --- 2. INTERACCIÓN Y DETECCIÓN DE COORDENADAS ---
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    float mx = (float) GetMouseX();
    float my = (float) GetMouseY();

    // Comprobación de colisión para no capturar clics por debajo del botón "Hola we"
    pair<int, int> btnC = Coord(0, -100);
    bool clickEnBoton = (mx >= btnC.first - 100 && mx <= btnC.first + 100 &&
                         my >= btnC.second - 25 && my <= btnC.second + 25);

    if (!clickEnBoton) {
      mapaClickeado = true;
      MapMouseToGeo(mx, my, lastLat, lastLon);
    }
  }

  // Dibujar indicador visual del punto clickeado
  if (mapaClickeado) {
    Vector2 p = MapGeoToRelativeScreen(lastLat, lastLon);
    pair<int, int> absP = Coord((int) p.x, (int) p.y);

    DrawCircle(absP.first, absP.second, 5.0f, RED);
    DrawCircleLines(absP.first, absP.second, 9.0f, WHITE);

    string latStr = "Lat: " + to_string(lastLat);
    string lonStr = "Lon: " + to_string(lastLon);
    text(latStr.c_str(), 320, 280, 16, GREEN);
    text(lonStr.c_str(), 320, 305, 16, GREEN);
  }

  // --- 3. BOTONES Y MENÚS DEL SISTEMA ---
  vector<Button> buttons;

  addButton(0, 400, 200, 50, "Hola we", buttons);
  if (isButtonPressed(buttons[0])) {
    cout << buttons[0].text << "\n";
  }
  buttons.clear();
}