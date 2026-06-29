#include "../../include/interfazDjikstra.h"
#include "../../include/utils.h"
#include "../../include/grafo_la_paz.h" // Cabecera sincronizada con el nuevo nombre
#include "../../include/djikstra.h"
#include <bits/stdc++.h>
#include <raylib.h>

using namespace std;

// ID de los nodos de origen y destino seleccionados por el usuario
int ORIGEN = -1;
int DESTINO = -1;

// Indica si el camino más corto ya fue calculado en el frame actual
bool CAMINO_CALCULADO = false;

// Transforma coordenadas geográficas (latitud, longitud) a píxeles
// RELATIVOS al centro (0,0) de la pantalla. Esto permite dibujar las calles
// y nodos alineados con la función Coord() de utils.h.
Vector2 MapGeoToRelativeScreen(double lat, double lon) {
  int w = GetScreenWidth();
  int h = GetScreenHeight();

  // 1. Mapear lat/lon linealmente a un rango entre 0 y ancho/alto de pantalla
  float absX = (float) ((lon - WEST) / (EAST - WEST) * w);
  float absY = (float) ((NORTH - lat) / (NORTH - SOUTH) * h);

  // 2. Convertir a coordenadas relativas centradas
  // Reducimos con un factor de 0.8 para dejar un margen del 10% en los bordes de la pantalla
  float relX = (absX - (w / 2.0f)) * 0.8f;
  float relY = (absY - (h / 2.0f)) * 0.8f;

  return Vector2{ relX, relY };
}

// Convierte la posición física del cursor del mouse (píxeles de pantalla)
// a coordenadas reales de latitud/longitud geográfica de La Paz.
void MapMouseToGeo(float mouseX, float mouseY, double &outLat, double &outLon) {
  int w = GetScreenWidth();
  int h = GetScreenHeight();

  // 1. Revertir el sistema relativo centrado con margen (0.8) a píxeles absolutos
  float absX = ((mouseX - (w / 2.0f)) / 0.8f) + (w / 2.0f);
  float absY = ((mouseY - (h / 2.0f)) / 0.8f) + (h / 2.0f);

  // 2. Interpolar linealmente para obtener los valores geográficos reales
  outLon = WEST + (absX / (double) w) * (EAST - WEST);
  outLat = NORTH - (absY / (double) h) * (NORTH - SOUTH);
}

// Escanea linealmente todos los nodos del mapa y retorna el ID del nodo más cercano
// a las coordenadas geográficas del clic de entrada. (Complejidad O(n))
int NodoMasCercano(double clickLat, double clickLon) {
  int mejorIdx = -1;
  double mejorDist = 1e18;

  for (int i = 0; i < NUM_NODOS; i++) {
    double dLat = METADATOS_NODOS[i].lat - clickLat;
    double dLon = METADATOS_NODOS[i].lon - clickLon;
    double dist = dLat * dLat + dLon * dLon; // Distancia euclidiana aproximada

    if (dist < mejorDist) {
      mejorDist = dist;
      mejorIdx = i;
    }
  }
  return mejorIdx;
}

// Variables de estado del mapa persistentes entre frames
static bool mapaClickeado = false;
static double lastLat = 0.0;
static double lastLon = 0.0;
static int nodoSeleccionado = -1;
static vector<int> caminoCalculado; // Almacena el camino de nodos de la última ruta calculada

// Variables de estado específicas para el dibujo y lógica de bloqueos viales
static bool ELIGIENDO_BLOQUEOS = false;
static bool TIENE_BLOQUEO = false;
static double BLOQUEO_LAT1 = 0.0;
static double BLOQUEO_LON1 = 0.0;
static double BLOQUEO_LAT2 = 0.0;
static double BLOQUEO_LON2 = 0.0;
static int CLICKS_BLOQUEO = 0; // Registra si llevamos 0, 1 o 2 clics para el rectángulo de bloqueo

// Textura de fondo del mapa cargada desde disco (mapa_fondo.png)
static Texture2D texturaMapa = { 0 };
static bool texturaMapaCargada = false;

// Límites geográficos originales (coordenadas reales de OpenStreetMap)
// con los que fue recortada la imagen estática de fondo mapa_fondo.png
static const double IMG_NORTH = -16.410549788;
static const double IMG_SOUTH = -16.794315612;
static const double IMG_EAST = -67.60832563599999;
static const double IMG_WEST = -68.169195764;

// Variables de control para arrastrar (panear) la cámara del mapa
static bool panActivo = false;
static float panMousePrevX = 0.0f;
static float panMousePrevY = 0.0f;

// Procesa el zoom con la rueda del ratón y el arrastre con el clic derecho.
// Modifica dinámicamente las coordenadas geográficas de los extremos visibles
// para realizar traslaciones y escalas sobre la pantalla.
static void procesarZoomYPan() {
  // ─── Control de ZOOM (Rueda del ratón) ─────────────────────────────────────
  float rueda = GetMouseWheelMove();
  if (rueda != 0.0f) {
    float mx = (float) GetMouseX();
    float my = (float) GetMouseY();

    // Convertir posición del cursor a coordenadas geográficas antes del zoom
    double geoLat, geoLon;
    MapMouseToGeo(mx, my, geoLat, geoLon);

    // Factor de escala: zoom in reduce el bounding box visible, zoom out lo amplía
    double factor = (rueda > 0) ? 0.85 : 1.0 / 0.85;

    double rangoLat = (NORTH - SOUTH) * factor;
    double rangoLon = (EAST - WEST) * factor;

    // Calcular proporciones relativas del cursor dentro del mapa para centrar el zoom
    double propLat = (NORTH - geoLat) / (NORTH - SOUTH);
    double propLon = (geoLon - WEST) / (EAST - WEST);

    // Recalcular límites de coordenadas para que el punto geográfico del cursor se mantenga fijo
    NORTH = geoLat + propLat * rangoLat;
    SOUTH = NORTH - rangoLat;
    WEST = geoLon - propLon * rangoLon;
    EAST = WEST + rangoLon;
  }

  // ─── Control de PAN (Arrastrar con Clic Derecho) ───────────────────────────
  if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
    float mx = (float) GetMouseX();
    float my = (float) GetMouseY();
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    // Solo permitir paneo si el clic se realiza dentro del recuadro activo del mapa (80% central)
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

    // Convertir desplazamiento de píxeles en pantalla a diferencias geográficas (latitud/longitud)
    double deltaLon = -(dx / (w * 0.8)) * (EAST - WEST);
    double deltaLat = (dy / (h * 0.8)) * (NORTH - SOUTH);

    // Desplazar las coordenadas geográficas de visualización del mapa
    NORTH += deltaLat;
    SOUTH += deltaLat;
    EAST += deltaLon;
    WEST += deltaLon;
  }
}

// Renderiza la imagen PNG de fondo del mapa de La Paz ajustándola
// proporcionalmente al nivel de Zoom y Pan geográfico activo.
static void dibujarFondoMapa() {
  // Cargar la textura en memoria de vídeo una sola vez por seguridad
  if (!texturaMapaCargada) {
    texturaMapa = LoadTexture("mapa_fondo.png");
    texturaMapaCargada = true;
  }

  if (texturaMapa.id != 0) {
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    // Calcular qué porción de la imagen original (fuente) corresponde al zoom geográfico actual
    float srcX = (float) ((WEST - IMG_WEST) / (IMG_EAST - IMG_WEST) * texturaMapa.width);
    float srcY = (float) ((IMG_NORTH - NORTH) / (IMG_NORTH - IMG_SOUTH) * texturaMapa.height);
    float srcW = (float) ((EAST - WEST) / (IMG_EAST - IMG_WEST) * texturaMapa.width);
    float srcH = (float) ((NORTH - SOUTH) / (IMG_NORTH - IMG_SOUTH) * texturaMapa.height);

    // Ajustar los márgenes de visualización (10% en cada lado)
    float margenX = w * 0.1f;
    float margenY = h * 0.1f;
    Rectangle fuente = { srcX, srcY, srcW, srcH };
    Rectangle destino = { margenX, margenY, w * 0.8f, h * 0.8f };

    // Dibujar la textura estirada y recortada usando transformaciones de Raylib
    DrawTexturePro(texturaMapa, fuente, destino, { 0, 0 }, 0.0f, WHITE);
  }
}

// Dibuja en la pantalla todas las calles (aristas del grafo) representadas como líneas grises oscuras
static void dibujarCalles() {
  for (int u = 0; u < NUM_NODOS; u++) {
    // Evitar renderizar nodos erróneos/desconectados en los límites del mapa
    if (METADATOS_NODOS[u].lon == -67.6191116) {
      continue;
    }

    // Convertir coordenadas del nodo inicial u a coordenadas de píxeles en pantalla
    Vector2 p1 = MapGeoToRelativeScreen(METADATOS_NODOS[u].lat, METADATOS_NODOS[u].lon);
    pair<int, int> absP1 = Coord((int) p1.x, (int) p1.y);

    // Recorrer los vecinos de u y trazar líneas hasta cada uno de ellos
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

// Dibuja en la pantalla el camino más corto calculado usando una línea verde gruesa (5px)
static void dibujarCaminoCalculado() {
  if (CAMINO_CALCULADO && ORIGEN != -1 && DESTINO != -1) {
    for (int i = 0; i < (int) caminoCalculado.size() - 1; i++) {
      int nodo = caminoCalculado[i];
      int nodoDestino = caminoCalculado[i + 1];

      // Obtener las coordenadas del segmento
      Vector2 p1 = MapGeoToRelativeScreen(METADATOS_NODOS[nodo].lat, METADATOS_NODOS[nodo].lon);
      Vector2 p2 = MapGeoToRelativeScreen(METADATOS_NODOS[nodoDestino].lat, METADATOS_NODOS[nodoDestino].lon);
      pair<int, int> absP1 = Coord((int) p1.x, (int) p1.y);
      pair<int, int> absP2 = Coord((int) p2.x, (int) p2.y);

      // Dibujar la arista del camino
      DrawLineEx({ (float) absP1.first, (float) absP1.second }, { (float) absP2.first, (float) absP2.second },
                 5.0f, GREEN);
    }
  }
}

// Dibuja pequeños círculos de color granate (maroon) para indicar las posiciones físicas de los nodos del mapa
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

// Procesa toda la interacción por clic izquierdo sobre el mapa.
// Si el modo bloqueo está activo (ELIGIENDO_BLOQUEOS), registra las dos esquinas contrarias.
// De lo contrario, registra la selección de los nodos de Origen y Destino.
static void procesarInteraccion() {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (ELIGIENDO_BLOQUEOS || ORIGEN == -1 || DESTINO == -1)) {
    float mx = (float) GetMouseX();
    float my = (float) GetMouseY();
    float w = GetScreenWidth();
    float h = GetScreenHeight();

    // Solo procesar clics dentro del área visible del mapa (80% central)
    bool enElMapa = (mx >= w * 0.1f && mx <= w * 0.9f && my >= h * 0.1f && my <= h * 0.9f);

    // Comprobación de colisión para no capturar clics en el botón invisible del menú anterior
    pair<int, int> btnC = Coord(0, -100);
    bool clickEnBoton = (mx >= btnC.first - 100 && mx <= btnC.first + 100 &&
                         my >= btnC.second - 25 && my <= btnC.second + 25);

    // Comprobación de colisión para no capturar clics en los selectores de zonas (arriba a la derecha)
    bool clickEnZona = false;
    for (int yTarget : { -450, -400, -350, -300, -250 }) {
      pair<int, int> zc = Coord(400, yTarget);
      if (mx >= zc.first - 80 && mx <= zc.first + 80 &&
          my >= zc.second - 18 && my <= zc.second + 18) {
        clickEnZona = true;
        break;
      }
    }

    // Comprobación de colisión para no capturar clics en los botones del panel inferior (Dijkstra/Bloqueos)
    bool clickEnBotonInferior = (mx >= w / 2 - 330 && mx <= w / 2 + 330 &&
                                 my >= h / 2 + 370 && my <= h / 2 + 430);

    if (enElMapa && !clickEnBoton && !clickEnZona && !clickEnBotonInferior) {
      // ── MODO: Captura de rectángulo de bloqueo ──────────────────────────────
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
          ELIGIENDO_BLOQUEOS = false; // Desactivar modo de dibujo tras definir el área
        } else {
          // Si ya existía uno anterior, reescribe e inicia uno nuevo
          BLOQUEO_LAT1 = clickedLat;
          BLOQUEO_LON1 = clickedLon;
          CLICKS_BLOQUEO = 1;
          TIENE_BLOQUEO = false;
        }
      }
      // ── MODO: Captura de Origen y Destino de ruta ───────────────────────────
      else {
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

  // Soporte para borrar origen, destino, cálculo y bloqueos presionando Backspace
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

// Dibuja en pantalla los paneles laterales de información con los IDs y coordenadas
// de los nodos de origen, destino y nodo seleccionado actual
static void dibujarIndicadores() {
  // Dibujar indicador del nodo más cercano seleccionado bajo el ratón
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

  // Dibujar indicador visual y textos del nodo de ORIGEN seleccionado (círculo rojo)
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

  // Dibujar indicador visual y textos del nodo de DESTINO seleccionado (círculo azul)
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

// Dibuja cuatro rectángulos blancos opacos en los cuatro márgenes de la pantalla (10% en cada lado)
// para tapar e impedir que se rendericen los nodos y las aristas que salgan del área delimitada del mapa.
static void dibujarBordes() {
  int w = GetScreenWidth();
  int h = GetScreenHeight();
  int margenX = (int) (w * 0.1f);
  int margenY = (int) (h * 0.1f);
  Color colorBorde = RAYWHITE;
  DrawRectangle(0, 0, w, margenY, colorBorde);           // Arriba
  DrawRectangle(0, h - margenY, w, margenY, colorBorde); // Abajo
  DrawRectangle(0, 0, margenX, h, colorBorde);           // Izquierda
  DrawRectangle(w - margenX, 0, margenX, h, colorBorde); // Derecha
}

// Dibuja la barra de botones inferior para el cálculo de Dijkstra y los controles de bloqueos,
// además de registrar y procesar las zonas rápidas arriba a la derecha.
static void dibujarBotonesYMenues() {
  vector<Button> buttons;

  // ── Botón: Activar/desactivar dibujo de bloqueo (X = -220) ─────────────────
  string txtBloqueo = ELIGIENDO_BLOQUEOS ? "Bloqueo: Dibujar..." : "Dibujar Bloqueo";
  addButton(-220, 400, 200, 50, txtBloqueo, buttons);
  Button btnModoBloqueo = buttons.back();

  // ── Botón: Limpiar área de bloqueo y restaurar ruta (X = 220) ──────────────
  addButton(220, 400, 200, 50, "Limpiar", buttons);
  Button btnLimpiarBloqueo = buttons.back();

  // Alternar el modo de selección de esquinas de bloqueo
  if (isButtonPressed(btnModoBloqueo)) {
    ELIGIENDO_BLOQUEOS = !ELIGIENDO_BLOQUEOS;
    if (ELIGIENDO_BLOQUEOS) {
      CLICKS_BLOQUEO = 0;
      TIENE_BLOQUEO = false;
    }
  }

  // Limpiar el estado de los bloqueos y reiniciar variables
  if (isButtonPressed(btnLimpiarBloqueo)) {
    TIENE_BLOQUEO = false;
    CLICKS_BLOQUEO = 0;
    ELIGIENDO_BLOQUEOS = false;
    CAMINO_CALCULADO = false;
    caminoCalculado.clear();
  }

  // ── Botón: Calcular Dijkstra (solo si origen y destino están seleccionados) ────
  if (ORIGEN != -1 && DESTINO != -1) {
    addButton(0, 400, 200, 50, "Calcular Djikstra", buttons);
    Button btnDijkstra = buttons.back();
    if (isButtonPressed(btnDijkstra) && !CAMINO_CALCULADO) {
      vector<double> distancia;
      vector<int> prev;
      // Ejecutar búsqueda del camino más corto con los datos de bloqueo
      djikstra(ORIGEN, distancia, prev, BLOQUEO_LAT1, BLOQUEO_LON1, BLOQUEO_LAT2, BLOQUEO_LON2, TIENE_BLOQUEO);
      caminoCalculado = ReconstruirCamino(prev, DESTINO);
      CAMINO_CALCULADO = true;
    }
  }
  buttons.clear();

  // ── Botones: Selectores rápidos de Zonas (arriba a la derecha) ─────────────────
  vector<Button> zoneButtons;
  text("Zonas:", 400, -485, 18, DARKGRAY);

  addButton(400, -450, 160, 36, "Miraflores", zoneButtons);
  addButton(400, -400, 160, 36, "Sopocachi", zoneButtons);
  addButton(400, -350, 160, 36, "Obrajes", zoneButtons);
  addButton(400, -300, 160, 36, "Calacoto", zoneButtons);
  addButton(400, -250, 160, 36, "Irpavi", zoneButtons);

  // Coordenadas geográficas centrales de cada zona para la traslación
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

  // Centrar la vista del mapa en la zona cliqueada conservando el zoom actual
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

// Dibuja en pantalla el rectángulo de bloqueo (amarillo semitransparente con borde opaco grueso).
// Si el dibujo está en proceso (CLICKS_BLOQUEO == 1), la segunda esquina sigue dinámicamente al cursor del mouse.
static void dibujarBloqueo() {
  if (CLICKS_BLOQUEO >= 1) {
    double lat2 = (CLICKS_BLOQUEO == 2) ? BLOQUEO_LAT2 : 0.0;
    double lon2 = (CLICKS_BLOQUEO == 2) ? BLOQUEO_LON2 : 0.0;

    // Si está en proceso, interpolar las coordenadas geográficas de la posición actual del cursor
    if (CLICKS_BLOQUEO == 1) {
      float mx = (float) GetMouseX();
      float my = (float) GetMouseY();
      MapMouseToGeo(mx, my, lat2, lon2);
    }

    // Convertir ambas esquinas a píxeles de pantalla
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

    Color relleno = { 253, 249, 0, 80 }; // Amarillo semitransparente (Alpha = 80)
    Color borde = { 253, 249, 0, 255 };  // Amarillo opaco (Alpha = 255)

    // Dibujar relleno y contorno grueso de 3px
    DrawRectangle(minX, minY, width, height, relleno);
    DrawRectangleLinesEx(Rectangle{ (float) minX, (float) minY, (float) width, (float) height }, 3.0f, borde);
  }
}

// Función principal orquestadora llamada en cada fotograma cuando la opción de Dijkstra está activa
void drawInterfazDjikstra() {

  // 1. Renderizar imagen estática de fondo del mapa
  dibujarFondoMapa();

  // 2. Renderizar las aristas (calles), el camino óptimo calculado, los nodos y los bloqueos viales
  dibujarCalles();
  dibujarCaminoCalculado();
  dibujarNodos();
  dibujarBloqueo();

  // 3. Procesar transformaciones de Zoom y Paneo
  procesarZoomYPan();

  // 4. Capturar entradas de teclado/mouse para seleccionar nodos y dibujar bloqueos
  procesarInteraccion();
  dibujarIndicadores();

  // 5. Ocultar los elementos que salen del recuadro del mapa usando bordes opacos
  dibujarBordes();

  // 6. Trazar botones inferiores y selectores rápidos de zonas
  dibujarBotonesYMenues();
}