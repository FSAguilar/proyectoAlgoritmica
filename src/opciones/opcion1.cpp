#include "../../include/opcion1.h"
#include "../../include/utils.h"
#include "../../include/grafo_la_paz.h" // Cabecera sincronizada con el nuevo nombre
#include "../../include/djikstra.h"
#include <bits/stdc++.h>
#include <raylib.h>

using namespace std;
int ORIGEN = -1;
int DESTINO = -1;
bool CAMINO_CALCULADO = false;
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

// Retorna el índice del nodo más cercano al punto geográfico dado (scan lineal O(n))
int NodoMasCercano(double clickLat, double clickLon) {
  int mejorIdx = -1;
  double mejorDist = 1e18;

  for (int i = 0; i < NUM_NODOS; i++) {
    double dLat = METADATOS_NODOS[i].lat - clickLat;
    double dLon = METADATOS_NODOS[i].lon - clickLon;
    double dist = dLat * dLat + dLon * dLon;

    if (dist < mejorDist) {
      mejorDist = dist;
      mejorIdx = i;
    }
  }
  return mejorIdx;
}

// Variables persistentes entre fotogramas
static bool mapaClickeado = false;
static double lastLat = 0.0;
static double lastLon = 0.0;
static int nodoSeleccionado = -1;
static vector<int> caminoCalculado; // Resultado del último Dijkstra

static bool ELIGIENDO_BLOQUEOS = false;
static bool TIENE_BLOQUEO = false;
static double BLOQUEO_LAT1 = 0.0;
static double BLOQUEO_LON1 = 0.0;
static double BLOQUEO_LAT2 = 0.0;
static double BLOQUEO_LON2 = 0.0;
static int CLICKS_BLOQUEO = 0;

// Textura de fondo del mapa (se carga una sola vez)
static Texture2D texturaMapa = { 0 };
static bool texturaMapaCargada = false;
// Límites geográficos originales con los que se generó la imagen
// Deben coincidir con los límites reales del grafo completo
static const double IMG_NORTH = -16.410549788;
static const double IMG_SOUTH = -16.794315612;
static const double IMG_EAST = -67.60832563599999;
static const double IMG_WEST = -68.169195764;

// Estado del pan (arrastrar con clic derecho)
static bool panActivo = false;
static float panMousePrevX = 0.0f;
static float panMousePrevY = 0.0f;

static void procesarZoomYPan() {
  // Zoom con rueda del mouse, centrado en la posición del cursor
  float rueda = GetMouseWheelMove();
  if (rueda != 0.0f) {
    float mx = (float) GetMouseX();
    float my = (float) GetMouseY();

    // Convertir posición del cursor a coordenadas geográficas antes del zoom
    double geoLat, geoLon;
    MapMouseToGeo(mx, my, geoLat, geoLon);

    // Factor de escala: >1 achica el bounding box (zoom in), <1 lo agranda (zoom out)
    double factor = (rueda > 0) ? 0.85 : 1.0 / 0.85;

    double rangoLat = (NORTH - SOUTH) * factor;
    double rangoLon = (EAST - WEST) * factor;

    // Proporciones del cursor dentro del bounding box actual
    double propLat = (NORTH - geoLat) / (NORTH - SOUTH);
    double propLon = (geoLon - WEST) / (EAST - WEST);

    // Recalcular límites manteniendo el punto bajo el cursor fijo
    NORTH = geoLat + propLat * rangoLat;
    SOUTH = NORTH - rangoLat;
    WEST = geoLon - propLon * rangoLon;
    EAST = WEST + rangoLon;
  }

  // Pan con clic derecho (arrastrar) — solo dentro del área del mapa
  if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
    float mx = (float) GetMouseX();
    float my = (float) GetMouseY();
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    bool enElMapa = (mx >= w * 0.1f && mx <= w * 0.9f && my >= h * 0.1f && my <= h * 0.9f);
    if (enElMapa) {
      panActivo = true;
      panMousePrevX = mx;
      panMousePrevY = my;
    }
  }
  if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
    panActivo = false;
  }
  if (panActivo) {
    float mx = (float) GetMouseX();
    float my = (float) GetMouseY();
    float dx = mx - panMousePrevX;
    float dy = my - panMousePrevY;
    panMousePrevX = mx;
    panMousePrevY = my;

    int w = GetScreenWidth();
    int h = GetScreenHeight();

    // Convertir desplazamiento en píxeles a grados geográficos
    double deltaLon = -(dx / (w * 0.8)) * (EAST - WEST);
    double deltaLat = (dy / (h * 0.8)) * (NORTH - SOUTH);

    NORTH += deltaLat;
    SOUTH += deltaLat;
    EAST += deltaLon;
    WEST += deltaLon;
  }
}

static void dibujarFondoMapa() {
  // Cargar textura la primera vez
  if (!texturaMapaCargada) {
    texturaMapa = LoadTexture("mapa_fondo.png");
    texturaMapaCargada = true;
  }

  // Dibujar textura ajustada al zoom/pan actual
  if (texturaMapa.id != 0) {
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    // Qué fracción de la imagen original corresponde a la vista actual
    float srcX = (float) ((WEST - IMG_WEST) / (IMG_EAST - IMG_WEST) * texturaMapa.width);
    float srcY = (float) ((IMG_NORTH - NORTH) / (IMG_NORTH - IMG_SOUTH) * texturaMapa.height);
    float srcW = (float) ((EAST - WEST) / (IMG_EAST - IMG_WEST) * texturaMapa.width);
    float srcH = (float) ((NORTH - SOUTH) / (IMG_NORTH - IMG_SOUTH) * texturaMapa.height);

    // El grafo usa un margen del 10% en cada lado (factor 0.8 en MapGeoToRelativeScreen)
    // La imagen debe ocupar exactamente el mismo area para alinearse
    float margenX = w * 0.1f;
    float margenY = h * 0.1f;
    Rectangle fuente = { srcX, srcY, srcW, srcH };
    Rectangle destino = { margenX, margenY, w * 0.8f, h * 0.8f };
    DrawTexturePro(texturaMapa, fuente, destino, { 0, 0 }, 0.0f, WHITE);
  }
}

static void dibujarCalles() {
  for (int u = 0; u < NUM_NODOS; u++) {
    if (METADATOS_NODOS[u].lon == -67.6191116) {
      continue;
    }

    Vector2 p1 = MapGeoToRelativeScreen(METADATOS_NODOS[u].lat, METADATOS_NODOS[u].lon);
    pair<int, int> absP1 = Coord((int) p1.x, (int) p1.y);

    for (const auto &edge : MAPA_GRAFO[u]) {
      int v = edge.first;
      if (METADATOS_NODOS[v].lon == -67.6191116) {
        continue;
      }

      Vector2 p2 = MapGeoToRelativeScreen(METADATOS_NODOS[v].lat, METADATOS_NODOS[v].lon);
      pair<int, int> absP2 = Coord((int) p2.x, (int) p2.y);

      DrawLine(absP1.first, absP1.second, absP2.first, absP2.second, DARKGRAY);
    }
  }
}

static void dibujarCaminoCalculado() {
  if (CAMINO_CALCULADO && ORIGEN != -1 && DESTINO != -1) {
    for (int i = 0; i < (int) caminoCalculado.size() - 1; i++) {
      int nodo = caminoCalculado[i];
      int nodoDestino = caminoCalculado[i + 1];
      Vector2 p1 = MapGeoToRelativeScreen(METADATOS_NODOS[nodo].lat, METADATOS_NODOS[nodo].lon);
      Vector2 p2 = MapGeoToRelativeScreen(METADATOS_NODOS[nodoDestino].lat, METADATOS_NODOS[nodoDestino].lon);
      pair<int, int> absP1 = Coord((int) p1.x, (int) p1.y);
      pair<int, int> absP2 = Coord((int) p2.x, (int) p2.y);
      DrawLineEx({ (float) absP1.first, (float) absP1.second }, { (float) absP2.first, (float) absP2.second },
                 5.0f, GREEN);
    }
  }
}

static void dibujarNodos() {
  for (int i = 0; i < NUM_NODOS; i++) {
    if (METADATOS_NODOS[i].lon == -67.6191116) {
      continue;
    }
    Vector2 p = MapGeoToRelativeScreen(METADATOS_NODOS[i].lat, METADATOS_NODOS[i].lon);
    pair<int, int> absP = Coord((int) p.x, (int) p.y);
    DrawCircle(absP.first, absP.second, 1.2f, MAROON);
  }
}

static void procesarInteraccion() {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (ELIGIENDO_BLOQUEOS || ORIGEN == -1 || DESTINO == -1)) {
    float mx = (float) GetMouseX();
    float my = (float) GetMouseY();
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    // Solo procesar clics dentro del área visible del mapa (80% central)
    bool enElMapa = (mx >= w * 0.1f && mx <= w * 0.9f && my >= h * 0.1f && my <= h * 0.9f);

    // Comprobación de colisión para no capturar clics en el botón
    pair<int, int> btnC = Coord(0, -100);
    bool clickEnBoton = (mx >= btnC.first - 100 && mx <= btnC.first + 100 &&
                         my >= btnC.second - 25 && my <= btnC.second + 25);

    // Comprobación de colisión para no capturar clics en los selectores de zonas
    bool clickEnZona = false;
    for (int yTarget : { -450, -400, -350, -300, -250 }) {
      pair<int, int> zc = Coord(400, yTarget);
      if (mx >= zc.first - 80 && mx <= zc.first + 80 &&
          my >= zc.second - 18 && my <= zc.second + 18) {
        clickEnZona = true;
        break;
      }
    }

    // Comprobación de colisión para no capturar clics en los botones del panel inferior
    bool clickEnBotonInferior = (mx >= w / 2 - 330 && mx <= w / 2 + 330 &&
                                 my >= h / 2 + 370 && my <= h / 2 + 430);

    if (enElMapa && !clickEnBoton && !clickEnZona && !clickEnBotonInferior) {
      if (ELIGIENDO_BLOQUEOS) {
        double clickedLat, clickedLon;
        MapMouseToGeo(mx, my, clickedLat, clickedLon);
        if (CLICKS_BLOQUEO == 0) {
          BLOQUEO_LAT1 = clickedLat;
          BLOQUEO_LON1 = clickedLon;
          CLICKS_BLOQUEO = 1;
          TIENE_BLOQUEO = false;
        } else if (CLICKS_BLOQUEO == 1) {
          BLOQUEO_LAT2 = clickedLat;
          BLOQUEO_LON2 = clickedLon;
          CLICKS_BLOQUEO = 2;
          TIENE_BLOQUEO = true;
          ELIGIENDO_BLOQUEOS = false;
        } else {
          BLOQUEO_LAT1 = clickedLat;
          BLOQUEO_LON1 = clickedLon;
          CLICKS_BLOQUEO = 1;
          TIENE_BLOQUEO = false;
        }
      } else {
        mapaClickeado = true;
        MapMouseToGeo(mx, my, lastLat, lastLon);
        nodoSeleccionado = NodoMasCercano(lastLat, lastLon);
        if (ORIGEN == -1) {
          ORIGEN = nodoSeleccionado;
        } else {
          DESTINO = nodoSeleccionado;
        }
      }
    }
  }

  if (IsKeyPressed(KEY_BACKSPACE)) {
    ORIGEN = -1;
    DESTINO = -1;
    CAMINO_CALCULADO = false;
    caminoCalculado.clear();
    TIENE_BLOQUEO = false;
    CLICKS_BLOQUEO = 0;
    ELIGIENDO_BLOQUEOS = false;
  }
}

static void dibujarIndicadores() {
  // Dibujar indicador del nodo más cercano seleccionado
  if (nodoSeleccionado != -1) {
    double nodLat = METADATOS_NODOS[nodoSeleccionado].lat;
    double nodLon = METADATOS_NODOS[nodoSeleccionado].lon;

    Vector2 pNodo = MapGeoToRelativeScreen(nodLat, nodLon);
    pair<int, int> absNodo = Coord((int) pNodo.x, (int) pNodo.y);

    DrawCircle(absNodo.first, absNodo.second, 6.0f, YELLOW);
    DrawCircleLines(absNodo.first, absNodo.second, 10.0f, ORANGE);

    string idxStr = "Nodo: " + to_string(nodoSeleccionado);
    string latStr = "Lat: " + to_string(nodLat);
    string lonStr = "Lon: " + to_string(nodLon);
    text(idxStr.c_str(), 320, 255, 16, YELLOW);
    text(latStr.c_str(), 320, 280, 16, YELLOW);
    text(lonStr.c_str(), 320, 305, 16, YELLOW);
  }

  if (ORIGEN != -1) {
    double nodLat = METADATOS_NODOS[ORIGEN].lat;
    double nodLon = METADATOS_NODOS[ORIGEN].lon;

    Vector2 pNodo = MapGeoToRelativeScreen(nodLat, nodLon);
    pair<int, int> absNodo = Coord((int) pNodo.x, (int) pNodo.y);

    DrawCircle(absNodo.first, absNodo.second, 6.0f, RED);
    DrawCircleLines(absNodo.first, absNodo.second, 10.0f, RED);

    string idxStr = "Nodo: " + to_string(ORIGEN);
    string latStr = "Lat: " + to_string(nodLat);
    string lonStr = "Lon: " + to_string(nodLon);
    text(idxStr.c_str(), 320, 330, 16, RED);
    text(latStr.c_str(), 320, 355, 16, RED);
    text(lonStr.c_str(), 320, 380, 16, RED);
  }

  if (DESTINO != -1) {
    double nodLat = METADATOS_NODOS[DESTINO].lat;
    double nodLon = METADATOS_NODOS[DESTINO].lon;

    Vector2 pNodo = MapGeoToRelativeScreen(nodLat, nodLon);
    pair<int, int> absNodo = Coord((int) pNodo.x, (int) pNodo.y);

    DrawCircle(absNodo.first, absNodo.second, 6.0f, BLUE);
    DrawCircleLines(absNodo.first, absNodo.second, 10.0f, BLUE);

    string idxStr = "Nodo: " + to_string(DESTINO);
    string latStr = "Lat: " + to_string(nodLat);
    string lonStr = "Lon: " + to_string(nodLon);
    text(idxStr.c_str(), 320, 255, 16, BLUE);
    text(latStr.c_str(), 320, 280, 16, BLUE);
    text(lonStr.c_str(), 320, 305, 16, BLUE);
  }
}

static void dibujarBordes() {
  // Tapan el 10% de margen en cada lado para ocultar nodos/aristas que salen del área del mapa
  int w = GetScreenWidth();
  int h = GetScreenHeight();
  int margenX = (int) (w * 0.1f);
  int margenY = (int) (h * 0.1f);
  Color colorBorde = RAYWHITE;
  DrawRectangle(0, 0, w, margenY, colorBorde);           // arriba
  DrawRectangle(0, h - margenY, w, margenY, colorBorde); // abajo
  DrawRectangle(0, 0, margenX, h, colorBorde);           // izquierda
  DrawRectangle(w - margenX, 0, margenX, h, colorBorde); // derecha
}

static void dibujarBotonesYMenues() {
  vector<Button> buttons;

  // Botones de Bloqueo (siempre visibles)
  string txtBloqueo = ELIGIENDO_BLOQUEOS ? "Bloqueo: Dibujar..." : "Dibujar Bloqueo";
  addButton(-220, 400, 200, 50, txtBloqueo, buttons);
  Button btnModoBloqueo = buttons.back();

  addButton(220, 400, 200, 50, "Limpiar", buttons);
  Button btnLimpiarBloqueo = buttons.back();

  if (isButtonPressed(btnModoBloqueo)) {
    ELIGIENDO_BLOQUEOS = !ELIGIENDO_BLOQUEOS;
    if (ELIGIENDO_BLOQUEOS) {
      CLICKS_BLOQUEO = 0;
      TIENE_BLOQUEO = false;
    }
  }

  if (isButtonPressed(btnLimpiarBloqueo)) {
    TIENE_BLOQUEO = false;
    CLICKS_BLOQUEO = 0;
    ELIGIENDO_BLOQUEOS = false;
    CAMINO_CALCULADO = false;
    caminoCalculado.clear();
  }

  // Botón Calcular Dijkstra (solo si origen y destino están seleccionados)
  if (ORIGEN != -1 && DESTINO != -1) {
    addButton(0, 400, 200, 50, "Calcular Djikstra", buttons);
    Button btnDijkstra = buttons.back();
    if (isButtonPressed(btnDijkstra) && !CAMINO_CALCULADO) {
      vector<double> distancia;
      vector<int> prev;
      djikstra(ORIGEN, distancia, prev, BLOQUEO_LAT1, BLOQUEO_LON1, BLOQUEO_LAT2, BLOQUEO_LON2, TIENE_BLOQUEO);
      caminoCalculado = ReconstruirCamino(prev, DESTINO);
      CAMINO_CALCULADO = true;
    }
  }
  buttons.clear();

  // Dibujar selectores de zonas
  vector<Button> zoneButtons;
  text("Zonas:", 400, -485, 18, DARKGRAY);

  addButton(400, -450, 160, 36, "Miraflores", zoneButtons);
  addButton(400, -400, 160, 36, "Sopocachi", zoneButtons);
  addButton(400, -350, 160, 36, "Obrajes", zoneButtons);
  addButton(400, -300, 160, 36, "Calacoto", zoneButtons);
  addButton(400, -250, 160, 36, "Irpavi", zoneButtons);

  struct ZoneCoord {
    double lat;
    double lon;
  };
  vector<ZoneCoord> zoneCoords = {
    { -16.5055, -68.1222 },  // Miraflores
    { -16.5108, -68.1275 },  // Sopocachi
    { -16.52171, -68.1157 }, // Obrajes (Calle 0)
    { -16.5397, -68.0856 },  // Calacoto (Calle 8)
    { -16.528, -68.083 }     // Irpavi
  };

  for (size_t i = 0; i < zoneButtons.size(); i++) {
    if (isButtonPressed(zoneButtons[i])) {
      double targetLat = zoneCoords[i].lat;
      double targetLon = zoneCoords[i].lon;
      double halfHeight = (NORTH - SOUTH) / 2.0;
      double halfWidth = (EAST - WEST) / 2.0;
      NORTH = targetLat + halfHeight;
      SOUTH = targetLat - halfHeight;
      WEST = targetLon - halfWidth;
      EAST = targetLon + halfWidth;
      break;
    }
  }
}

static void dibujarBloqueo() {
  if (CLICKS_BLOQUEO >= 1) {
    double lat2 = (CLICKS_BLOQUEO == 2) ? BLOQUEO_LAT2 : 0.0;
    double lon2 = (CLICKS_BLOQUEO == 2) ? BLOQUEO_LON2 : 0.0;

    if (CLICKS_BLOQUEO == 1) {
      float mx = (float) GetMouseX();
      float my = (float) GetMouseY();
      MapMouseToGeo(mx, my, lat2, lon2);
    }

    Vector2 p1 = MapGeoToRelativeScreen(BLOQUEO_LAT1, BLOQUEO_LON1);
    Vector2 p2 = MapGeoToRelativeScreen(lat2, lon2);

    pair<int, int> absP1 = Coord((int) p1.x, (int) p1.y);
    pair<int, int> absP2 = Coord((int) p2.x, (int) p2.y);

    int minX = min(absP1.first, absP2.first);
    int maxX = max(absP1.first, absP2.first);
    int minY = min(absP1.second, absP2.second);
    int maxY = max(absP1.second, absP2.second);

    int width = maxX - minX;
    int height = maxY - minY;

    Color relleno = { 253, 249, 0, 80 };
    Color borde = { 253, 249, 0, 255 };

    DrawRectangle(minX, minY, width, height, relleno);
    DrawRectangleLinesEx(Rectangle{ (float) minX, (float) minY, (float) width, (float) height }, 3.0f, borde);
  }
}

void drawOpcion1() {
  // Para ir a la declaración de una función = Ctrl + Click Izquierdo
  //  --- 0. ZOOM Y PAN ---
  procesarZoomYPan();

  // --- 1. FONDO DEL MAPA ---
  dibujarFondoMapa();

  // --- 2. RENDERIZADO DE CALLES Y AVENIDAS (GRAFO) ---
  dibujarCalles();
  dibujarCaminoCalculado();
  dibujarNodos();
  dibujarBloqueo();

  // --- 2. INTERACCIÓN Y DETECCIÓN DE COORDENADAS ---
  procesarInteraccion();
  dibujarIndicadores();

  // --- 4. BORDES ---
  dibujarBordes();

  // --- 3. BOTONES Y MENÚS DEL SISTEMA ---
  dibujarBotonesYMenues();
}