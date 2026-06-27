#include "../../include/interfazMochila.h"
#include "../../include/mochila.h"
#include "../../include/inventario.h"
#include "../../include/utils.h"
#include "raylib.h"
#include <string>
#include <vector>

using namespace std;

enum SubEstado { SUB_INPUT, SUB_RESULTADOS };

// ─── Color por categoría ─────────────────────────────────────────────────────
static Color colorCategoria(const string& cat) {
    if (cat == "Despensa") return { 34, 139, 34, 255 };
    if (cat == "Lácteos")  return { 70, 130, 180, 255 };
    if (cat == "Carnes")   return { 178, 34, 34, 255 };
    if (cat == "Limpieza") return { 218, 165, 32, 255 };
    if (cat == "Bebidas")  return { 148, 0, 211, 255 };
    return DARKGRAY;
}

// ─── Tabla de catálogo con scroll interno ────────────────────────────────────
// El panel permanece fijo; solo las filas se desplazan con un scissor/clip.
static void drawCatalogo(const vector<Item>& catalogo, int scrollY) {
    const int ROW_H      = 24;
    const int MAX_ROWS   = 10;   // filas visibles a la vez
    const int PANEL_W    = 680;
    const int HEADER_H   = 30;   // alto del encabezado fijo
    const int BODY_H     = MAX_ROWS * ROW_H;  // 240 px
    const int HINT_H     = 18;
    const int PANEL_H    = HEADER_H + BODY_H + HINT_H + 4;

    // Posición fija del panel (NO depende de scrollY)
    pair<int,int> orig = Coord(-340, -160);
    int px = orig.first;
    int py = orig.second;

    // Fondo del panel
    DrawRectangle(px, py, PANEL_W, PANEL_H, { 245, 245, 245, 255 });
    DrawRectangleLines(px, py, PANEL_W, PANEL_H, LIGHTGRAY);

    // ── Encabezado fijo ───────────────────────────────────────────────────
    int hx = px + 8;
    int hy = py + 8;
    DrawText("PRODUCTO",    hx,       hy, 14, DARKGRAY);
    DrawText("CATEGORÍA",   hx + 280, hy, 14, DARKGRAY);
    DrawText("PRECIO (Bs)", hx + 520, hy, 14, DARKGRAY);
    DrawLine(px, py + HEADER_H, px + PANEL_W, py + HEADER_H, LIGHTGRAY);

    // ── Filas con scroll ──────────────────────────────────────────────────
    // Área visible del cuerpo (clip manual: solo dibujamos si la fila cae dentro)
    int bodyTop    = py + HEADER_H;
    int bodyBottom = bodyTop + BODY_H;

    int startIdx = scrollY / ROW_H;
    int endIdx   = min((int)catalogo.size(), startIdx + MAX_ROWS + 1);

    // Offset en píxeles dentro de la fila de inicio
    int pixelOffset = scrollY % ROW_H;

    for (int i = startIdx; i < endIdx; i++) {
        const Item& item = catalogo[i];
        // posición Y real de la fila dentro del panel
        int ry = bodyTop + (i - startIdx) * ROW_H - pixelOffset;

        // Solo dibujamos si la fila está dentro del área visible
        if (ry + ROW_H <= bodyTop) continue;
        if (ry >= bodyBottom)      break;

        // Fondo alternado
        if (i % 2 == 0) {
            int clipY    = max(ry, bodyTop);
            int clipH    = min(ry + ROW_H, bodyBottom) - clipY;
            DrawRectangle(px + 1, clipY, PANEL_W - 2, clipH, { 255, 255, 255, 200 });
        }

        // Solo dibujamos el texto si la fila al menos asoma por arriba del clip
        if (ry + 2 >= bodyTop && ry < bodyBottom) {
            DrawText(item.nombre.c_str(),    hx,       ry + 4, 14, BLACK);
            DrawText(item.categoria.c_str(), hx + 280, ry + 4, 14, colorCategoria(item.categoria));
            string p = to_string(item.costo) + " Bs";
            DrawText(p.c_str(),              hx + 520, ry + 4, 14, DARKBLUE);
        }
    }

    // ── Hint de scroll ────────────────────────────────────────────────────
    if ((int)catalogo.size() > MAX_ROWS) {
        DrawText("[ Rueda del ratón para desplazar ]",
                 px + 8, bodyBottom + 3, 12, GRAY);
    }
}

// ─── Interfaz principal ───────────────────────────────────────────────────────
void drawInterfazMochila() {
    static SubEstado estadoActual  = SUB_INPUT;
    static string    presupuestoStr = "";
    static int       presupuestoFinal = 0;
    static bool      cajaActiva    = false;
    static ResultadoMochila resultado;
    static vector<Item> catalogo   = obtenerInventarioHipermaxi();
    static int       catalogoScroll = 0;
    static int       carritoScroll  = 0;

    vector<Button> buttons;

    // ════════════════════════════════════════════════════════════════════════
    if (estadoActual == SUB_INPUT) {

        text("OPTIMIZADOR DE COMPRAS", 0, -270, 26, MAROON);
        text("Hipermaxi — Catálogo disponible hoy", 0, -240, 16, DARKGRAY);

        // Scroll del catálogo
        float wheel = GetMouseWheelMove();
        if (wheel != 0.0f) {
            const int ROW_H    = 24;
            const int MAX_ROWS = 10;
            int maxScroll = max(0, ((int)catalogo.size() - MAX_ROWS) * ROW_H);
            catalogoScroll -= (int)(wheel * ROW_H * 2);
            if (catalogoScroll < 0)          catalogoScroll = 0;
            if (catalogoScroll > maxScroll)   catalogoScroll = maxScroll;
        }
        drawCatalogo(catalogo, catalogoScroll);

        // ── Caja de presupuesto ───────────────────────────────────────────
        text("Ingresa tu presupuesto:", -150, 200, 18, DARKGRAY);

        Color colorCaja = cajaActiva ? PURPLE : LIGHTGRAY;
        Rect(80, 200, 180, 42, colorCaja);

        if (!presupuestoStr.empty())
            text((presupuestoStr + " Bs").c_str(), 80, 200, 22, WHITE);
        else
            text("Ej. 150", 80, 200, 22, { 200, 200, 200, 255 });

        pair<int,int> posCaja = Coord(80, 200);
        int mx = GetMouseX(), my = GetMouseY();
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            bool sobre = (mx >= posCaja.first - 90 && mx <= posCaja.first + 90 &&
                          my >= posCaja.second - 21 && my <= posCaja.second + 21);
            cajaActiva = sobre;
        }

        if (cajaActiva) {
            int key = GetCharPressed();
            while (key > 0) {
                if (key >= '0' && key <= '9' && presupuestoStr.length() < 5)
                    presupuestoStr += (char)key;
                key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && !presupuestoStr.empty())
                presupuestoStr.pop_back();
        }

        addButton(80, 260, 275, 42, "Calcular compra óptima", buttons);

        if ((isButtonPressed(buttons[0]) || (cajaActiva && IsKeyPressed(KEY_ENTER))) &&
            !presupuestoStr.empty()) {
            presupuestoFinal = stoi(presupuestoStr);
            resultado        = resolverMochila(presupuestoFinal, catalogo);
            estadoActual     = SUB_RESULTADOS;
        }
    }

    // ════════════════════════════════════════════════════════════════════════
    else if (estadoActual == SUB_RESULTADOS) {

        text("COMPRA ÓPTIMA SUGERIDA", 0, -270, 24, MAROON);

        string resumen = "Presupuesto: " + to_string(presupuestoFinal) +
                         " Bs  |  Gasto: "  + to_string(resultado.costoTotal) +
                         " Bs  |  Vuelto: " +
                         to_string(presupuestoFinal - resultado.costoTotal) + " Bs";
        text(resumen.c_str(), 0, -240, 15, DARKGRAY);

        // ── Constantes de layout ──────────────────────────────────────────
        const int ROW_H    = 24;
        const int MAX_ROWS = 10;   // filas visibles a la vez en el cuerpo
        const int HEADER_H = 32;   // barra roja "CARRITO"
        const int COL_H    = 30;   // fila de encabezado de columnas
        const int BODY_H   = MAX_ROWS * ROW_H;   // área scrolleable fija
        const int FOOT_H   = 30;   // pie con el total
        const int HINT_H   = 18;
        const int PANEL_W  = 600;
        const int PANEL_H  = HEADER_H + COL_H + BODY_H + FOOT_H + HINT_H + 4;

        pair<int,int> orig = Coord(-300, -220);
        int px = orig.first;
        int py = orig.second;

        // ── Scroll del carrito ────────────────────────────────────────────
        int totalItems = (int)resultado.itemsSeleccionados.size();
        float wheel = GetMouseWheelMove();
        if (wheel != 0.0f && totalItems > MAX_ROWS) {
            int maxScroll = max(0, (totalItems - MAX_ROWS) * ROW_H);
            carritoScroll -= (int)(wheel * ROW_H * 2);
            if (carritoScroll < 0)          carritoScroll = 0;
            if (carritoScroll > maxScroll)   carritoScroll = maxScroll;
        }

        // ── Panel de fondo (tamaño fijo) ──────────────────────────────────
        DrawRectangle(px, py, PANEL_W, PANEL_H, { 250, 250, 250, 255 });
        DrawRectangleLines(px, py, PANEL_W, PANEL_H, LIGHTGRAY);

        // Barra roja de título
        DrawRectangle(px, py, PANEL_W, HEADER_H, { 139, 0, 0, 255 });
        DrawText("CARRITO DE COMPRAS", px + 12, py + 8, 17, WHITE);

        // Encabezado de columnas (fijo, no scrollea)
        int cx  = px + 12;
        int cy  = py + HEADER_H + 6;
        DrawText("PRODUCTO",  cx,       cy, 13, DARKGRAY);
        DrawText("CATEGORÍA", cx + 280, cy, 13, DARKGRAY);
        DrawText("PRECIO",    cx + 480, cy, 13, DARKGRAY);
        DrawLine(px, cy + 20, px + PANEL_W, cy + 20, LIGHTGRAY);

        // ── Filas scrolleables ────────────────────────────────────────────
        int bodyTop    = py + HEADER_H + COL_H;
        int bodyBottom = bodyTop + BODY_H;

        if (resultado.itemsSeleccionados.empty()) {
            DrawText("El presupuesto es insuficiente para cualquier producto.",
                     cx, bodyTop + 8, 15, RED);
        } else {
            int pixelOffset = carritoScroll % ROW_H;
            int startIdx    = carritoScroll / ROW_H;
            int endIdx      = min(totalItems, startIdx + MAX_ROWS + 1);

            for (int i = startIdx; i < endIdx; i++) {
                const Item& item = resultado.itemsSeleccionados[i];
                int ry = bodyTop + (i - startIdx) * ROW_H - pixelOffset;

                if (ry + ROW_H <= bodyTop) continue;
                if (ry >= bodyBottom)      break;

                // Fondo alternado con clip
                if (i % 2 == 0) {
                    int clipY = max(ry, bodyTop);
                    int clipH = min(ry + ROW_H, bodyBottom) - clipY;
                    DrawRectangle(px + 1, clipY, PANEL_W - 2, clipH, { 255, 255, 255, 200 });
                }

                if (ry + 2 >= bodyTop && ry < bodyBottom) {
                    DrawText(item.nombre.c_str(),    cx,       ry + 4, 14, BLACK);
                    DrawText(item.categoria.c_str(), cx + 280, ry + 4, 14, colorCategoria(item.categoria));
                    string p = to_string(item.costo) + " Bs";
                    DrawText(p.c_str(),              cx + 480, ry + 4, 14, DARKBLUE);
                }
            }
        }

        // ── Pie fijo: línea + total ────────────────────────────────────────
        int footY = bodyBottom;
        DrawLine(px, footY, px + PANEL_W, footY, GRAY);
        string total = "TOTAL: " + to_string(resultado.costoTotal) + " Bs";
        DrawText(total.c_str(), cx, footY + 6, 15, MAROON);

        // Hint de scroll (solo si hay más filas que las visibles)
        if (totalItems > MAX_ROWS) {
            DrawText("[ Rueda del ratón para desplazar ]",
                     px + 8, footY + FOOT_H + 2, 12, GRAY);
        }

        // ── Botón volver ──────────────────────────────────────────────────
        addButton(0, -30 + PANEL_H/2 + 36, 200, 42, "Nueva Compra", buttons);
        if (isButtonPressed(buttons[0])) {
            presupuestoStr = "";
            catalogoScroll = 0;
            carritoScroll  = 0;
            estadoActual   = SUB_INPUT;
        }
    }
}