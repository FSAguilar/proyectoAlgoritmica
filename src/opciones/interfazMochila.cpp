#include "../../include/interfazMochila.h"
#include "../../include/mochila.h"
#include "../../include/inventario.h"
#include "../../include/utils.h"
#include "raylib.h"
#include <string>
#include <vector>

using namespace std;

// Sub-estados internos para separar la captura de datos (presupuesto)
// de la visualización de los resultados (lista de compras óptima sugerida).
enum SubEstado { SUB_INPUT,
                 SUB_RESULTADOS };

// ─── Asigna un color identificativo a cada categoría de productos ────────────────
static Color colorCategoria(const string &cat) {
  if (cat == "Despensa") return { 34, 139, 34, 255 };  // Verde bosque
  if (cat == "Lácteos") return { 70, 130, 180, 255 };  // Azul acero
  if (cat == "Carnes") return { 178, 34, 34, 255 };    // Rojo fuego
  if (cat == "Limpieza") return { 218, 165, 32, 255 }; // Dorado/Amarillo
  if (cat == "Bebidas") return { 148, 0, 211, 255 };   // Violeta
  if (cat == "Snacks") return { 205, 92, 92, 255 };    // Rojo indio
  return DARKGRAY;
}

// ─── Dibuja la tabla de catálogo con scroll por rueda del ratón ──────────────────
// Utiliza coordenadas relativas para posicionarse y dibuja solo las filas que
// asoman dentro de la caja visible (clip manual/scissor visual).
static void drawCatalogo(const vector<Item> &catalogo, int scrollY) {
  const int ROW_H = 24;                // Alto de cada fila en píxeles
  const int MAX_ROWS = 10;             // Número máximo de filas visibles a la vez
  const int PANEL_W = 680;             // Ancho del catálogo
  const int HEADER_H = 30;             // Alto del encabezado fijo
  const int BODY_H = MAX_ROWS * ROW_H; // Alto del cuerpo de la tabla (240px)
  const int HINT_H = 18;               // Leyenda informativa inferior
  const int PANEL_H = HEADER_H + BODY_H + HINT_H + 4;

  // Posición de anclaje del panel (relativo al centro de la ventana)
  pair<int, int> orig = Coord(-340, -120);
  int px = orig.first;
  int py = orig.second;

  // Fondo grisáceo claro del catálogo
  DrawRectangle(px, py, PANEL_W, PANEL_H, { 245, 245, 245, 255 });
  DrawRectangleLines(px, py, PANEL_W, PANEL_H, LIGHTGRAY);

  // ── Encabezado fijo (no se desplaza con el scroll) ─────────────────────────────
  int hx = px + 8;
  int hy = py + 8;
  DrawText("PRODUCTO", hx, hy, 14, DARKGRAY);
  DrawText("CATEGORÍA", hx + 280, hy, 14, DARKGRAY);
  DrawText("PRECIO (Bs)", hx + 520, hy, 14, DARKGRAY);
  DrawLine(px, py + HEADER_H, px + PANEL_W, py + HEADER_H, LIGHTGRAY);

  // ── Filas con desplazamiento ───────────────────────────────────────────────────
  int bodyTop = py + HEADER_H;
  int bodyBottom = bodyTop + BODY_H;

  // Calcular el rango de índices de productos que caen dentro del área visible
  int startIdx = scrollY / ROW_H;
  int endIdx = min((int) catalogo.size(), startIdx + MAX_ROWS + 1);

  // Desplazamiento fino en píxeles de la primera fila cortada superior
  int pixelOffset = scrollY % ROW_H;

  for (int i = startIdx; i < endIdx; i++) {
    const Item &item = catalogo[i];
    // Posición Y de dibujo absoluta en pantalla
    int ry = bodyTop + (i - startIdx) * ROW_H - pixelOffset;

    // Comprobación de seguridad: omitir si está completamente fuera por arriba o por abajo
    if (ry + ROW_H <= bodyTop) continue;
    if (ry >= bodyBottom) break;

    // Alternancia de fondo blanco translúcido en filas pares
    if (i % 2 == 0) {
      int clipY = max(ry, bodyTop);
      int clipH = min(ry + ROW_H, bodyBottom) - clipY;
      DrawRectangle(px + 1, clipY, PANEL_W - 2, clipH, { 255, 255, 255, 200 });
    }

    // Dibujar etiquetas de texto (solo si la fila es visible en la caja)
    if (ry + 2 >= bodyTop && ry < bodyBottom) {
      DrawText(item.nombre.c_str(), hx, ry + 4, 14, BLACK);
      DrawText(item.categoria.c_str(), hx + 280, ry + 4, 14, colorCategoria(item.categoria));
      string p = to_string(item.costo) + " Bs";
      DrawText(p.c_str(), hx + 520, ry + 4, 14, DARKBLUE);
    }
  }

  // ── Indicador inferior de scroll ───────────────────────────────────────────────
  if ((int) catalogo.size() > MAX_ROWS) {
    DrawText("[ Rueda del ratón para desplazar ]",
             px + 8, bodyBottom + 3, 12, GRAY);
  }
}

// ─── Interfaz principal de control ───────────────────────────────────────────
void drawInterfazMochila() {
  // Variables persistentes que conservan su valor entre frames sucesivos para la GUI
  static SubEstado estadoActual = SUB_INPUT;
  static string presupuestoStr = "";
  static int presupuestoFinal = 0;
  static bool cajaActiva = false;
  static ResultadoMochila resultado;

  // Carga todos los mercados una sola vez al inicio (vector estático)
  static vector<Mercado> mercados = obtenerTodosLosMercados();
  static int mercadoActual = 0; // Índice del supermercado seleccionado (0 = Hipermaxi, 1 = Ketal, 2 = Fidalga)

  static int catalogoScroll = 0;
  static int carritoScroll = 0;

  vector<Button> buttons;

  // Referencia rápida al catálogo del mercado actual
  const vector<Item> &catalogo = mercados[mercadoActual].catalogo;
  const string &nombreMercado = mercados[mercadoActual].nombre;

  // ════════════════════════════════════════════════════════════════════════
  // SUB-ESTADO: Captura de presupuesto y visualización de catálogo
  // ════════════════════════════════════════════════════════════════════════
  if (estadoActual == SUB_INPUT) {

    text("OPTIMIZADOR DE COMPRAS", 0, -310, 26, MAROON);

    // ── Selector de supermercado ─────────────────────────────────────────────
    // Dibuja botones con flechas para cambiar entre supermercados
    text("Supermercado:", -200, -270, 18, DARKGRAY);

    // Botón flecha izquierda
    addButton(0, -270, 36, 36, "<", buttons);
    // Nombre del mercado actual centrado
    string etiquetaMercado = "[ " + nombreMercado + " ]";
    text(etiquetaMercado.c_str(), 130, -270, 20, MAROON);
    // Botón flecha derecha
    addButton(260, -270, 36, 36, ">", buttons);

    // Procesar cambio de mercado con las flechas
    if (isButtonPressed(buttons[0])) {
      // Flecha izquierda: ir al mercado anterior (circular)
      mercadoActual = (mercadoActual - 1 + (int) mercados.size()) % (int) mercados.size();
      catalogoScroll = 0; // Reiniciar scroll al cambiar
    }
    if (isButtonPressed(buttons[1])) {
      // Flecha derecha: ir al siguiente mercado (circular)
      mercadoActual = (mercadoActual + 1) % (int) mercados.size();
      catalogoScroll = 0;
    }

    // Subtítulo dinámico con el nombre del mercado
    string subtitulo = nombreMercado + " - Catálogo disponible hoy";
    text(subtitulo.c_str(), 0, -240, 16, DARKGRAY);

    // Desplazamiento por scroll de la lista del catálogo
    float wheel = GetMouseWheelMove();
    if (wheel != 0.0f) {
      const int ROW_H = 24;
      const int MAX_ROWS = 10;
      int maxScroll = max(0, ((int) catalogo.size() - MAX_ROWS) * ROW_H);
      catalogoScroll -= (int) (wheel * ROW_H * 2);
      if (catalogoScroll < 0) catalogoScroll = 0;
      if (catalogoScroll > maxScroll) catalogoScroll = maxScroll;
    }

    // Dibujar el panel del catálogo con el scroll calculado
    drawCatalogo(catalogo, catalogoScroll);

    // ── Caja de texto interactiva para presupuesto ───────────────────────
    text("Ingresa tu presupuesto:", -150, 240, 18, DARKGRAY);

    // Si la caja de texto está activa, se dibuja con borde púrpura, sino gris claro
    Color colorCaja = cajaActiva ? PURPLE : LIGHTGRAY;
    Rect(80, 240, 180, 42, colorCaja);

    if (!presupuestoStr.empty())
      text((presupuestoStr + " Bs").c_str(), 80, 240, 22, WHITE);
    else
      text("Ej. 150", 80, 240, 22, { 200, 200, 200, 255 }); // Placeholder

    // Detectar si el usuario hace clic dentro de la caja de presupuesto
    pair<int, int> posCaja = Coord(80, 240);
    int mx = GetMouseX(), my = GetMouseY();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      bool sobre = (mx >= posCaja.first - 90 && mx <= posCaja.first + 90 &&
                    my >= posCaja.second - 21 && my <= posCaja.second + 21);
      cajaActiva = sobre;
    }

    // Si la caja está enfocada/activa, capturamos los caracteres numéricos introducidos
    if (cajaActiva) {
      int key = GetCharPressed();
      while (key > 0) {
        if (key >= '0' && key <= '9' && presupuestoStr.length() < 5)
          presupuestoStr += (char) key;
        key = GetCharPressed();
      }
      // Soporte para borrar caracteres con Backspace
      if (IsKeyPressed(KEY_BACKSPACE) && !presupuestoStr.empty())
        presupuestoStr.pop_back();
    }

    // Registrar botón para calcular la mochila
    addButton(80, 300, 275, 42, "Calcular compra óptima", buttons);

    // Transicionar al subestado de resultados si se hace clic en calcular o se pulsa Enter
    if ((isButtonPressed(buttons[2]) || (cajaActiva && IsKeyPressed(KEY_ENTER))) &&
        !presupuestoStr.empty()) {
      presupuestoFinal = stoi(presupuestoStr);
      // Ejecutamos el algoritmo de Programación Dinámica con el catálogo del mercado actual
      resultado = resolverMochila(presupuestoFinal, catalogo);
      estadoActual = SUB_RESULTADOS;
    }
  }

  // ════════════════════════════════════════════════════════════════════════
  // SUB-ESTADO: Visualización de resultados (Productos recomendados)
  // ════════════════════════════════════════════════════════════════════════
  else if (estadoActual == SUB_RESULTADOS) {

    text("COMPRA ÓPTIMA SUGERIDA", 0, -270, 24, MAROON);

    // Mostrar el nombre del mercado del cual se calculó la compra
    string mercadoInfo = "Mercado: " + nombreMercado;
    text(mercadoInfo.c_str(), 0, -245, 15, { 139, 0, 0, 255 });

    // Mostrar resumen general de compras
    string resumen = "Presupuesto: " + to_string(presupuestoFinal) +
                     " Bs  |  Gasto: " + to_string(resultado.costoTotal) +
                     " Bs  |  Vuelto: " +
                     to_string(presupuestoFinal - resultado.costoTotal) + " Bs";
    text(resumen.c_str(), 0, -225, 15, DARKGRAY);

    // ── Constantes visuales de diseño del carrito ───────────────────────
    const int ROW_H = 24;
    const int MAX_ROWS = 10;             // Filas visibles a la vez
    const int HEADER_H = 32;             // Cabecera del panel de compras
    const int COL_H = 30;                // Nombres de las columnas
    const int BODY_H = MAX_ROWS * ROW_H; // Altura fija de filas (240px)
    const int FOOT_H = 30;               // Pie con la suma total
    const int HINT_H = 18;               // Leyenda de desplazamiento
    const int PANEL_W = 600;             // Ancho de la tabla de resultados
    const int PANEL_H = HEADER_H + COL_H + BODY_H + FOOT_H + HINT_H + 4;

    pair<int, int> orig = Coord(-300, -200);
    int px = orig.first;
    int py = orig.second;

    // ── Desplazamiento del carrito por scroll ────────────────────────────
    int totalItems = (int) resultado.itemsSeleccionados.size();
    float wheel = GetMouseWheelMove();
    if (wheel != 0.0f && totalItems > MAX_ROWS) {
      int maxScroll = max(0, (totalItems - MAX_ROWS) * ROW_H);
      carritoScroll -= (int) (wheel * ROW_H * 2);
      if (carritoScroll < 0) carritoScroll = 0;
      if (carritoScroll > maxScroll) carritoScroll = maxScroll;
    }

    // ── Dibujar panel base ───────────────────────────────────────────────
    DrawRectangle(px, py, PANEL_W, PANEL_H, { 250, 250, 250, 255 });
    DrawRectangleLines(px, py, PANEL_W, PANEL_H, LIGHTGRAY);

    // Cabecera roja de compras con nombre del mercado
    DrawRectangle(px, py, PANEL_W, HEADER_H, { 139, 0, 0, 255 });
    string headerText = "CARRITO DE COMPRAS - " + nombreMercado;
    DrawText(headerText.c_str(), px + 12, py + 8, 17, WHITE);

    // Nombres fijos de las columnas
    int cx = px + 12;
    int cy = py + HEADER_H + 6;
    DrawText("PRODUCTO", cx, cy, 13, DARKGRAY);
    DrawText("CATEGORÍA", cx + 280, cy, 13, DARKGRAY);
    DrawText("PRECIO", cx + 480, cy, 13, DARKGRAY);
    DrawLine(px, cy + 20, px + PANEL_W, cy + 20, LIGHTGRAY);

    // ── Dibujar filas recomendadas (con soporte de scroll) ───────────────
    int bodyTop = py + HEADER_H + COL_H;
    int bodyBottom = bodyTop + BODY_H;

    if (resultado.itemsSeleccionados.empty()) {
      DrawText("El presupuesto es insuficiente para cualquier producto.",
               cx, bodyTop + 8, 15, RED);
    } else {
      int pixelOffset = carritoScroll % ROW_H;
      int startIdx = carritoScroll / ROW_H;
      int endIdx = min(totalItems, startIdx + MAX_ROWS + 1);

      for (int i = startIdx; i < endIdx; i++) {
        const Item &item = resultado.itemsSeleccionados[i];
        int ry = bodyTop + (i - startIdx) * ROW_H - pixelOffset;

        // Validación de bordes superior e inferior
        if (ry + ROW_H <= bodyTop) continue;
        if (ry >= bodyBottom) break;

        // Fondo de filas intercaladas
        if (i % 2 == 0) {
          int clipY = max(ry, bodyTop);
          int clipH = min(ry + ROW_H, bodyBottom) - clipY;
          DrawRectangle(px + 1, clipY, PANEL_W - 2, clipH, { 255, 255, 255, 200 });
        }

        // Escribir datos del producto seleccionado
        if (ry + 2 >= bodyTop && ry < bodyBottom) {
          DrawText(item.nombre.c_str(), cx, ry + 4, 14, BLACK);
          DrawText(item.categoria.c_str(), cx + 280, ry + 4, 14, colorCategoria(item.categoria));
          string p = to_string(item.costo) + " Bs";
          DrawText(p.c_str(), cx + 480, ry + 4, 14, DARKBLUE);
        }
      }
    }

    // ── Pie de tabla con total y leyenda de scroll ───────────────────────
    int footY = bodyBottom;
    DrawLine(px, footY, px + PANEL_W, footY, GRAY);
    string total = "TOTAL: " + to_string(resultado.costoTotal) + " Bs";
    DrawText(total.c_str(), cx, footY + 6, 15, MAROON);

    if (totalItems > MAX_ROWS) {
      DrawText("[ Rueda del ratón para desplazar ]",
               px + 8, footY + FOOT_H + 2, 12, GRAY);
    }

    // ── Botón para reiniciar presupuesto y regresar a captura ─────────────
    addButton(0, -30 + PANEL_H / 2 + 36, 200, 42, "Nueva Compra", buttons);
    if (isButtonPressed(buttons[0])) {
      presupuestoStr = "";
      catalogoScroll = 0;
      carritoScroll = 0;
      estadoActual = SUB_INPUT;
    }
  }
}