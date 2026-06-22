#include "../../include/utils.h"
#include "raylib.h"
#include <bits/stdc++.h>
using namespace std;

// Poner Texto teniendo en cuenta que el centro de la pantalla tiene coords (0,0)
void text(const char *text, int x, int y, int size, Color color) {
  int xCenter = (GetScreenWidth() - MeasureText(text, size)) / 2;
  int yCenter = (GetScreenHeight() - size) / 2;
  DrawText(text, xCenter + x, yCenter + y, size, color);
}

pair<int, int> Coord(int x, int y) {
  return { GetScreenWidth() / 2 + x, GetScreenHeight() / 2 + y };
}

void Rect(int x, int y, int width, int height, Color color) {
  pair<int, int> c = Coord(x, y);

  DrawRectangle(c.first - width / 2, c.second - height / 2, width, height, color);
}

bool isButtonPressed(Button btn) {
  int x = GetMouseX();
  int y = GetMouseY();
  return x >= btn.left && x <= btn.right && y >= btn.top && y <= btn.bottom && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void addButton(int x, int y, int width, int height, string str, vector<Button> &btns) {
  Rect(x, y, width, height, LIGHTGRAY);
  text(str.c_str(), x, y, 20, BLACK);
  auto topLeft = Coord(x - width / 2, y - height / 2);
  auto bottomRight = Coord(x + width / 2, y + height / 2);
  btns.push_back({ str, topLeft.first, bottomRight.first, topLeft.second, bottomRight.second });
}