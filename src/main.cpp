#include "raylib.h"
#include <bits/stdc++.h>
#include "../include/opcion1.h"
#include "../include/utils.h"
using namespace std;

int main() {

  string state = "menu";

  vector<Button> buttons;

  const int screenWidth = 800;
  const int screenHeight = 600;

  InitWindow(screenWidth, screenHeight, "Soluciones con Raylib");

  SetTargetFPS(60);

  while (!WindowShouldClose()) {

    BeginDrawing();

    if (state == "menu") {
      ClearBackground(RAYWHITE);
      DrawText(state.c_str(), 0, 0, 20, BLACK);

      addButton(0, -100, 200, 50, "Opcion 1", buttons);
      addButton(0, 0, 200, 50, "Mochila bb", buttons);
      addButton(0, 100, 200, 50, "Opcion 3", buttons);

      if (isButtonPressed(buttons[0])) {
        cout << buttons[0].text << "\n";
        state = "opcion1";
      }
      if (isButtonPressed(buttons[1])) {
        cout << buttons[1].text << "\n";
        state = "mochila_bb";
      }
      if (isButtonPressed(buttons[2])) {
        cout << buttons[2].text << "\n";
        state = "opcion3";
      }
      buttons.clear();
    } else if (state == "mochila_bb") {
      ClearBackground(RAYWHITE);
      DrawText(state.c_str(), 0, 0, 20, BLACK);
      drawOpcion1();
    }
    EndDrawing();
  }
  CloseWindow();

  return 0;
}
