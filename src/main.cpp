#include "raylib.h"
#include <bits/stdc++.h>
#include "../include/interfazDjikstra.h"
#include "../include/interfazMochila.h"
#include "../include/cola_prioridad.h"
#include "../include/utils.h"
#include "../include/grafo_la_paz.h"

using namespace std;

// Punto de entrada principal de la aplicación.
// Coordina la carga inicial del mapa y gestiona la máquina de estados principal
// para la navegación entre las tres opciones del proyecto (Dijkstra, Mochila y Cola de Prioridad).
int main() {

  // 1. Cargar el mapa geográfico completo desde el archivo binario pre-procesado
  if (!CargarMapaBinario("mapa.bin")) {
    std::cerr << "Error crítico: No se pudo encontrar o cargar 'mapa.bin'" << std::endl;
    return -1;
  }

  // 2. Definir los límites geográficos por defecto de la ventana de vista del mapa
  NORTH = -16.46;
  SOUTH = -16.56;
  EAST = -68.06;
  WEST = -68.16;

  std::cout << "Mapa cargado con éxito. Nodos: " << NUM_NODOS << std::endl;

  // Estado actual de navegación ("menu", "camino mas corto", "mochila_bb", "cola_prioridad")
  string state = "menu";

  // Vector temporal para detectar la interacción con los botones en cada frame
  vector<Button> buttons;

  // Dimensiones estáticas de la ventana gráfica
  const int screenWidth = 1000;
  const int screenHeight = 1000;

  // Inicializar la ventana con Raylib
  InitWindow(screenWidth, screenHeight, "Proyecto Algoritmica");

  // Configurar límite de 60 cuadros por segundo para evitar sobrecarga de CPU
  SetTargetFPS(60);

  // Bucle de juego principal
  while (!WindowShouldClose()) {

    BeginDrawing();

    // ─── ESTADO: MENÚ PRINCIPAL ─────────────────────────────────────────────
    if (state == "menu") {
      ClearBackground(RAYWHITE);
      DrawText(state.c_str(), 0, 0, 20, BLACK);

      // Dibujar botones principales de navegación
      addButton(0, -100, 200, 50, "Camino mas corto", buttons);
      addButton(0, 0, 200, 50, "Mochila bb", buttons);
      addButton(0, 100, 200, 50, "Cola de Prioridad", buttons);

      // Manejar la transición de estado al hacer clic en los botones
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
    } 
    // ─── ESTADO: PANTALLAS DE TRABAJO (Con botón de retroceso a menú) ───────
    else {

      // Renderizar el contenido específico según el estado seleccionado
      if (state == "camino mas corto") {
        ClearBackground({ 13, 17, 23, 255 });
        drawInterfazDjikstra();
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
      
      // Dibujar botón flotante en la esquina superior izquierda para regresar al menú
      addButton(-400, -430, 200, 50, "Menu", buttons);
      if (isButtonPressed(buttons[0])) {
        state = "menu";
      }
    }
    
    buttons.clear();
    EndDrawing();
  }
  
  // Cerrar el contexto de la ventana y liberar recursos gráficos de la GPU
  CloseWindow();

  return 0;
}