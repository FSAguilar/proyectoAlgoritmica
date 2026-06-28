#include "../../include/cola_prioridad.h"
#include "../../include/utils.h"
#include "raylib.h"
#include <bits/stdc++.h>
using namespace std;

struct Emergencia {
    string nombre;
    int severidad;
};

struct Paciente {
    string nombre;
    string emergencia;
    int edad;
    double horaLlegada;
};

static vector<Emergencia> TABLA_EMERGENCIAS = {
    {"Paro cardiorrespiratorio", 150},
    {"Obstruccion total de via aerea", 148},
    {"Aneurisma disecante de aorta", 145},
    {"Shock anafilactico", 143},
    {"Hemorragia subaracnoidea", 140},
    {"Infarto agudo de miocardio", 138},
    {"ACV", 137},
    {"Arritmia grave", 136},
    {"Shock hipovolemico", 134},
    {"Eclampsia", 130},
    {"Shock septico", 128},
    {"Insuficiencia respiratoria aguda severa", 125},
    {"Meningitis bacteriana aguda", 122},
    {"Edema agudo de pulmon", 120},
    {"Trauma craneoencefalico severo", 118},
    {"Trauma raquimedular con compromiso neurologico", 116},
    {"Politraumatismo grave", 114},
    {"Hipoglucemia severa", 112},
    {"Hemorragia digestiva alta severa", 108},
    {"Pancreatitis aguda severa", 102},
    {"Embarazo ectopico roto", 98},
    {"Fiebre en paciente oncologico", 95},
    {"Preeclampsia severa", 92},
    {"Neumotorax", 90},
    {"Quemaduras graves", 88},
    {"Mordedura de serpiente venenosa", 85},
    {"Crisis vaso-oclusiva", 80},
    {"Obstruccion intestinal", 76},
    {"Cetoacidosis diabetica", 74},
    {"Crisis asmatica severa", 72},
    {"Estado epileptico", 70},
    {"Sincope de origen cardiaco", 66},
    {"Intoxicacion o sobredosis con compromiso", 64},
    {"Crisis hipertensiva", 60},
    {"Colecistitis aguda complicada", 56},
    {"Abdomen agudo", 54},
    {"Cuerpo extrano en via aerea", 52},
    {"Celulitis facial por absceso dental", 48},
    {"Fractura expuesta con compromiso vascular", 46},
    {"Trauma ocular penetrante", 44},
    {"Quemadura quimica ocular", 40},
    {"Deshidratacion severa", 36},
    {"Apendicitis aguda no complicada", 34},
    {"Reaccion alergica moderada", 30},
    {"Crisis convulsiva febril", 28},
    {"Colico renal", 26},
    {"Intoxicacion alimentaria con vomitos persistentes", 22},
    {"Fractura cerrada simple", 18},
    {"Luxacion articular", 14},
    {"Laceracion profunda", 10}
};

int buscarSeveridad(const string &nombre) {
    for (auto &e : TABLA_EMERGENCIAS)
        if (e.nombre == nombre)
            return e.severidad;
    return -1;
}

int bonusEdad(int edad) {
    if (edad <= 1) return 6;
    if (edad <= 5) return 4;
    if (edad <= 12) return 2;
    if (edad <= 17) return 1;
    if (edad <= 59) return 0;
    if (edad <= 74) return 3;
    return 6;
}

int puntaje(const Paciente &p, double ahora) {
    int sev = buscarSeveridad(p.emergencia);
    int espera = (int)((ahora - p.horaLlegada) / 600) * 5;
    return sev * 1000 + bonusEdad(p.edad) + espera;
}

bool clickEnCaja(int x, int y, int width, int height) {
    auto topLeft = Coord(x - width / 2, y - height / 2);
    auto bottomRight = Coord(x + width / 2, y + height / 2);
    int mx = GetMouseX();
    int my = GetMouseY();
    return IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && mx >= topLeft.first && mx <= bottomRight.first && my >= topLeft.second && my <= bottomRight.second;
}

void cajaTexto(int x, int y, int width, int height, const string &label, const string &valor, bool activa) {
    Rect(x, y, width, height, activa ? Color{220, 220, 255, 255} : LIGHTGRAY);
    string mostrar = label + valor;
    text(mostrar.c_str(), x, y, 18, BLACK);
}

void drawColaPrioridad() {
    static vector<Paciente> cola;
    static string inputNombre, inputEmergencia, inputEdad;
    static int campoActivo = 0;

    double ahora = GetTime();

    sort(cola.begin(), cola.end(), [ahora](const Paciente &a, const Paciente &b) {
        return puntaje(a, ahora) > puntaje(b, ahora);
    });

    text("Cola", -300, -280, 30, BLACK);
    {
        auto topLeft = Coord(-450, -240);
        DrawRectangleLines(topLeft.first, topLeft.second, 320, 520, BLACK);
    }

    int yOffset = -220;
    for (auto &p : cola) {
        string linea = p.nombre + " - " + p.emergencia + " (" + to_string(p.edad) + ")";
        text(linea.c_str(), -300, yOffset, 16, BLACK);
        yOffset += 30;
    }

    DrawLine(GetScreenWidth() / 2, 0, GetScreenWidth() / 2, GetScreenHeight(), BLACK);

    text("Nuevo Paciente", 250, -280, 26, BLACK);

    if (clickEnCaja(250, -200, 300, 50)) campoActivo = 1;
    if (clickEnCaja(250, -130, 300, 50)) campoActivo = 2;
    if (clickEnCaja(250, -60, 300, 50)) campoActivo = 3;

    cajaTexto(250, -200, 300, 50, "Nombre: ", inputNombre, campoActivo == 1);
    cajaTexto(250, -130, 300, 50, "Emergencia: ", inputEmergencia, campoActivo == 2);
    cajaTexto(250, -60, 300, 50, "Edad: ", inputEdad, campoActivo == 3);

    if (campoActivo == 1) {
        int key = GetCharPressed();
        while (key > 0) {
            if (inputNombre.size() < 20) inputNombre += (char)key;
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && !inputNombre.empty()) inputNombre.pop_back();
    } else if (campoActivo == 2) {
        int key = GetCharPressed();
        while (key > 0) {
            if (inputEmergencia.size() < 50) inputEmergencia += (char)key;
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && !inputEmergencia.empty()) inputEmergencia.pop_back();
    } else if (campoActivo == 3) {
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= '0' && key <= '9' && inputEdad.size() < 3) inputEdad += (char)key;
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && !inputEdad.empty()) inputEdad.pop_back();
    }

    vector<Button> btns;
    addButton(250, 20, 150, 50, "ANADIR", btns);
    addButton(250, 220, 150, 50, "ASIGNAR", btns);

    if (isButtonPressed(btns[0])) {
        if (!inputNombre.empty() && !inputEdad.empty() && buscarSeveridad(inputEmergencia) != -1) {
            cola.push_back({inputNombre, inputEmergencia, stoi(inputEdad), GetTime()});
            inputNombre.clear();
            inputEmergencia.clear();
            inputEdad.clear();
            campoActivo = 0;
        }
    }

    if (isButtonPressed(btns[1])) {
        if (!cola.empty()) cola.erase(cola.begin());
    }

    text("Sala Disponible", 250, 150, 22, BLACK);
}