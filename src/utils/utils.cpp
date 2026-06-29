#include "../../include/utils.h"
#include "raylib.h"
#include <bits/stdc++.h>

using namespace std;

// Dibuja texto centrado en pantalla, usando un sistema de coordenadas relativo
// donde el centro de la pantalla es el punto (0, 0).
void text(const char *text, int x, int y, int size, Color color) {
  // Calculamos el origen de dibujo restando la mitad de las dimensiones del texto al centro físico de la ventana
  int xCenter = (GetScreenWidth() - MeasureText(text, size)) / 2;
  int yCenter = (GetScreenHeight() - size) / 2;
  DrawText(text, xCenter + x, yCenter + y, size, color);
}

// Transforma coordenadas relativas (con centro en 0, 0) a coordenadas absolutas de píxeles
// tradicionales del sistema de la pantalla de Raylib (donde 0, 0 es la esquina superior izquierda).
pair<int, int> Coord(int x, int y) {
  return { GetScreenWidth() / 2 + x, GetScreenHeight() / 2 + y };
}

// Dibuja un rectángulo relleno centrado en las coordenadas relativas dadas.
void Rect(int x, int y, int width, int height, Color color) {
  pair<int, int> c = Coord(x, y);
  // Restamos la mitad del ancho y alto para que las coordenadas (x, y) definan exactamente el centro de la figura
  DrawRectangle(c.first - width / 2, c.second - height / 2, width, height, color);
}

// Comprueba si las coordenadas actuales del cursor del mouse se encuentran dentro de la caja de colisión del botón,
// y si el usuario acaba de hacer clic izquierdo en este frame.
bool isButtonPressed(Button btn) {
  int x = GetMouseX();
  int y = GetMouseY();
  return x >= btn.left && x <= btn.right && y >= btn.top && y <= btn.bottom && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

// Dibuja un botón interactivo (rectángulo + texto), calcula sus fronteras absolutas en la pantalla
// y lo registra dentro de la lista de botones activos para comprobar posteriormente clics en el frame actual.
void addButton(int x, int y, int width, int height, string str, vector<Button> &btns) {
  // 1. Dibujar el cuerpo gris del botón y el texto en el centro
  Rect(x, y, width, height, LIGHTGRAY);
  text(str.c_str(), x, y, 20, BLACK);
  
  // 2. Calcular los extremos superior izquierdo e inferior derecho en píxeles absolutos de pantalla
  auto topLeft = Coord(x - width / 2, y - height / 2);
  auto bottomRight = Coord(x + width / 2, y + height / 2);
  
  // 3. Registrar el botón en la lista para colisiones
  btns.push_back({ str, topLeft.first, bottomRight.first, topLeft.second, bottomRight.second });
}