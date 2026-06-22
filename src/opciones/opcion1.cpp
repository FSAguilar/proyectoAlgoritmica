#include "../../include/opcion1.h"
#include "../../include/utils.h"
#include "raylib.h"
#include <bits/stdc++.h>

using namespace std;

void drawOpcion1() {
  vector<Button> buttons;

  addButton(0, -100, 200, 50, "Hola we", buttons);
  if (isButtonPressed(buttons[0])) {
    cout << buttons[0].text << "\n";
  }
  buttons.clear();
}