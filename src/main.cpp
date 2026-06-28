#include "raylib.h"
#include <bits/stdc++.h>
#include "../include/opcion1.h"
#include "../include/interfazMochila.h"
#include "../include/cola_prioridad.h"
#include "../include/utils.h"
#include "../include/grafo_la_paz.h"
using namespace std;

int main() {

  if (!CargarMapaBinario("mapa.bin")) {
    std::cerr << "Error crítico: No se pudo encontrar o cargar 'mapa.bin'" << std::endl;
    return -1;
  }

  NORTH = -16.46;
  SOUTH = -16.56;
  EAST = -68.06;
  WEST = -68.16;

  std::cout << "Mapa cargado con éxito. Nodos: " << NUM_NODOS << std::endl;

  string state = "menu";

  vector<Button> buttons;

  const int screenWidth = 1000;
  const int screenHeight = 1000;

  InitWindow(screenWidth, screenHeight, "Proyecto Algoritmica");

  SetTargetFPS(60);

  while (!WindowShouldClose()) {

    BeginDrawing();

    if (state == "menu") {
      ClearBackground(RAYWHITE);
      DrawText(state.c_str(), 0, 0, 20, BLACK);

      addButton(0, -100, 200, 50, "Camino mas corto", buttons);
      addButton(0, 0, 200, 50, "Mochila bb", buttons);
      addButton(0, 100, 200, 50, "Cola de Prioridad", buttons);

      if (isButtonPressed(buttons[0])) {
        cout << buttons[0].text << "\n";
        state = "camino mas corto";
      }
      if (isButtonPressed(buttons[1])) {
        cout << buttons[1].text << "\n";
        state = "mochila_bb";
      }
      if (isButtonPressed(buttons[2])) {
        cout << buttons[2].text << "\n";
        state = "cola_prioridad";
      }
      buttons.clear();
    } else if (state == "camino mas corto") {
      ClearBackground({ 13, 17, 23, 255 });
      drawOpcion1();
      DrawText(state.c_str(), 0, 0, 20, BLACK);
    } else if (state == "mochila_bb") {
      ClearBackground(RAYWHITE);
      DrawText(state.c_str(), 0, 0, 20, BLACK);
      drawInterfazMochila();
    } else if (state == "cola_prioridad") {
      ClearBackground(RAYWHITE);
      DrawText(state.c_str(), 0, 0, 20, BLACK);
      drawColaPrioridad();
    }

    EndDrawing();
  }
  CloseWindow();

  return 0;
}