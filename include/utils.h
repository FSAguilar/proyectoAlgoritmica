#ifndef UTILS_H
#define UTILS_H

#include "raylib.h"
#include <string>
#include <utility>
#include <vector>

struct Button {
  std::string text;
  int left, right, top, bottom;
};
void text(const char *text, int x, int y, int size, Color color);

std::pair<int, int> Coord(int x, int y);

void Rect(int x, int y, int width, int height, Color color);

bool isButtonPressed(Button btn);

void addButton(int x, int y, int width, int height, std::string str, std::vector<Button> &btns);

#endif