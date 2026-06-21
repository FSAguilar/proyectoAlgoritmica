#include "raylib.h"
#include <bits/stdc++.h>
using namespace std;

// Poner Texto teniendo en cuenta que el centro de la pantalla tiene coords (0,0)
void text(const char *text, int x, int y, int size, Color color) {
  int xCenter = (GetScreenWidth() - MeasureText(text, size)) / 2;
  int yCenter = (GetScreenHeight() - size) / 2;
  DrawText(text, xCenter + x, yCenter + y, size, color);
}

int main() {

  const int screenWidth = 800;
  const int screenHeight = 600;

  InitWindow(screenWidth, screenHeight, "Soluciones con Raylib");

  SetTargetFPS(60);

  while (!WindowShouldClose()) {

    BeginDrawing();

    ClearBackground(RAYWHITE);

    text("Hola Mundo", 0, 0, 20, BLACK);

    EndDrawing();
  }
  CloseWindow();

  return 0;
}
