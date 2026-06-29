#include "../../include/cola_prioridad.h"
#include "../../include/utils.h"
#include "raylib.h"
#include <bits/stdc++.h>
using namespace std;

// Representa una fila de la tabla de las 50 emergencias: nombre y su severidad
struct Emergencia {
  string nombre;
  int severidad;
};

// Representa a un paciente que entró a la cola
struct Paciente {
  string nombre;
  string emergencia; // debe coincidir exactamente con un "nombre" de TABLA_EMERGENCIAS
  int edad;
  double horaLlegada; // momento (en segundos desde que arrancó el programa) en que se añadió
};

// Índice de la emergencia actualmente seleccionada en TABLA_EMERGENCIAS, mientras
// el usuario navega con las flechas UP/DOWN en el campo "Emergencia"
int indexEmergencia = 0;

// Tabla estática con las 50 emergencias y su severidad (clave primaria de prioridad)
static vector<Emergencia> TABLA_EMERGENCIAS = {
  { "Paro cardiorrespiratorio", 150 },
  { "Obstruccion total de via aerea", 148 },
  { "Aneurisma disecante de aorta", 145 },
  { "Shock anafilactico", 143 },
  { "Hemorragia subaracnoidea", 140 },
  { "Infarto agudo de miocardio", 138 },
  { "ACV", 137 },
  { "Arritmia grave", 136 },
  { "Shock hipovolemico", 134 },
  { "Eclampsia", 130 },
  { "Shock septico", 128 },
  { "Insuficiencia respiratoria aguda severa", 125 },
  { "Meningitis bacteriana aguda", 122 },
  { "Edema agudo de pulmon", 120 },
  { "Trauma craneoencefalico severo", 118 },
  { "Trauma raquimedular con compromiso neurologico", 116 },
  { "Politraumatismo grave", 114 },
  { "Hipoglucemia severa", 112 },
  { "Hemorragia digestiva alta severa", 108 },
  { "Pancreatitis aguda severa", 102 },
  { "Embarazo ectopico roto", 98 },
  { "Fiebre en paciente oncologico", 95 },
  { "Preeclampsia severa", 92 },
  { "Neumotorax", 90 },
  { "Quemaduras graves", 88 },
  { "Mordedura de serpiente venenosa", 85 },
  { "Crisis vaso-oclusiva", 80 },
  { "Obstruccion intestinal", 76 },
  { "Cetoacidosis diabetica", 74 },
  { "Crisis asmatica severa", 72 },
  { "Estado epileptico", 70 },
  { "Sincope de origen cardiaco", 66 },
  { "Intoxicacion o sobredosis con compromiso", 64 },
  { "Crisis hipertensiva", 60 },
  { "Colecistitis aguda complicada", 56 },
  { "Abdomen agudo", 54 },
  { "Cuerpo extrano en via aerea", 52 },
  { "Celulitis facial por absceso dental", 48 },
  { "Fractura expuesta con compromiso vascular", 46 },
  { "Trauma ocular penetrante", 44 },
  { "Quemadura quimica ocular", 40 },
  { "Deshidratacion severa", 36 },
  { "Apendicitis aguda no complicada", 34 },
  { "Reaccion alergica moderada", 30 },
  { "Crisis convulsiva febril", 28 },
  { "Colico renal", 26 },
  { "Intoxicacion alimentaria con vomitos persistentes", 22 },
  { "Fractura cerrada simple", 18 },
  { "Luxacion articular", 14 },
  { "Laceracion profunda", 10 }
};

// Variable global a nivel de archivo. El "static" aquí significa "privada de este
// archivo .cpp" (ningún otro .cpp del proyecto puede verla), no tiene que ver con
// las "static" de adentro de drawColaPrioridad(). La actualizamos al inicio de cada
// frame para que puntaje() pueda leer el tiempo actual sin que se la pasemos como
// parámetro ni la capturemos en una lambda.
static double g_ahora;

// Busca el nombre de emergencia en la tabla y devuelve su severidad.
// Si no existe ninguna coincidencia exacta, devuelve -1 (esto es lo que usamos para detectar "emergencia inválida")
int buscarSeveridad(const string &nombre) {
  for (auto &e : TABLA_EMERGENCIAS)
    if (e.nombre == nombre)
      return e.severidad;
  return -1;
}

// Bonus de desempate según la edad (niños muy pequeños y ancianos suben un poco de prioridad)
int bonusEdad(int edad) {
  if (edad <= 1) return 6;
  if (edad <= 5) return 4;
  if (edad <= 12) return 2;
  if (edad <= 17) return 1;
  if (edad <= 59) return 0;
  if (edad <= 74) return 3;
  return 6;
}

// Calcula el puntaje total de un paciente para poder ordenar la cola.
// Ya no recibe "ahora" como parámetro: lee directamente la variable global g_ahora.
int puntaje(const Paciente &p) {
  int sev = buscarSeveridad(p.emergencia);

  // Cada 600 segundos (10 min) de espera suma 5 puntos de desempate.
  // g_ahora - p.horaLlegada = segundos que el paciente lleva esperando
  int espera = (int) ((g_ahora - p.horaLlegada) / 600) * 5;

  // Multiplicamos la severidad por 1000 para que SIEMPRE domine sobre
  // cualquier combinación de bonus de edad + espera (ningún desempate puede
  // hacer que una fractura supere a un infarto)
  return sev * 1000 + bonusEdad(p.edad) + espera;
}

// Función comparadora "normal" (no lambda), usada por sort().
// Ya no necesita capturar nada porque puntaje() lee g_ahora por su cuenta.
// Debe devolver true si "a" va ANTES que "b" en el orden final.
bool comparadorPacientes(const Paciente &a, const Paciente &b) {
  return puntaje(a) > puntaje(b); // mayor puntaje primero
}

// Revisa si el usuario hizo click dentro de una caja de texto (mismo sistema de coordenadas
// centradas que usa Rect/Coord en utils.h)
bool clickEnCaja(int x, int y, int width, int height) {
  auto topLeft = Coord(x - width / 2, y - height / 2);
  auto bottomRight = Coord(x + width / 2, y + height / 2);
  int mx = GetMouseX();
  int my = GetMouseY();
  return IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && mx >= topLeft.first && mx <= bottomRight.first && my >= topLeft.second && my <= bottomRight.second;
}

// Dibuja una caja de texto: el rectángulo de fondo (más claro si está "activa", es decir,
// si el usuario hizo click en ella y está escribiendo/seleccionando) y el texto label + valor actual
void cajaTexto(int x, int y, int width, int height, const string &label, const string &valor, bool activa) {
  Rect(x, y, width, height, activa ? Color{ 220, 220, 255, 255 } : LIGHTGRAY);
  string mostrar = label + valor;
  text(mostrar.c_str(), x, y, 18, BLACK);
}

// Función principal que se llama una vez por frame cuando el usuario está en esta pantalla.
// Acá vive tanto el dibujo como el manejo de input (clicks y teclado).
void drawColaPrioridad() {

  // "static" aquí sí significa lo de siempre: conservan su valor entre llamadas,
  // así la cola y los inputs no se reinician cada frame.
  static vector<Paciente> cola;
  static string inputNombre, inputEmergencia, inputEdad;
  static int campoActivo = 0; // 0 = ninguno, 1 = nombre, 2 = emergencia, 3 = edad

  // Almacena una copia del último paciente atendido/asignado para mostrarlo en pantalla
  static Paciente *prevPaciente = nullptr;

  // Actualizamos la global ANTES de ordenar, para que puntaje() use el tiempo de este frame
  g_ahora = GetTime();

  // Reordenamos la cola cada frame: el de mayor puntaje queda primero (índice 0).
  // Pasamos la función normal comparadorPacientes en vez de una lambda con captura.
  sort(cola.begin(), cola.end(), comparadorPacientes);

  // --- Panel izquierdo: "Cola" ---
  text("Cola", -300, -280, 30, BLACK);
  {
    // Solo el marco/borde del panel de la cola
    auto topLeft = Coord(-450, -240);
    DrawRectangleLines(topLeft.first, topLeft.second, 320, 520, BLACK);
  }

  // Dibujamos cada paciente, uno debajo del otro, de mayor a menor prioridad
  int yOffset = -220;
  for (auto &p : cola) {
    string linea = p.nombre + " - " + p.emergencia + " (" + to_string(p.edad) + ")";
    text(linea.c_str(), -300, yOffset, 16, BLACK);
    yOffset += 30; // bajamos 30px para la siguiente línea
  }

  // Línea divisoria vertical entre el panel "Cola" y "Nuevo Paciente"
  DrawLine(GetScreenWidth() / 2, 0, GetScreenWidth() / 2, GetScreenHeight(), BLACK);

  // --- Panel derecho: "Nuevo Paciente" ---
  text("Nuevo Paciente", 250, -280, 26, BLACK);

  // Si se hace click sobre alguna caja, esa pasa a ser la "activa"
  if (clickEnCaja(250, -200, 300, 50)) campoActivo = 1;
  if (clickEnCaja(250, -130, 300, 50)) campoActivo = 2;
  if (clickEnCaja(250, -60, 300, 50)) campoActivo = 3;

  // Dibujamos las 3 cajas con su contenido actual
  cajaTexto(250, -200, 300, 50, "Nombre: ", inputNombre, campoActivo == 1);
  cajaTexto(250, -130, 300, 50, "Emergencia: ", inputEmergencia, campoActivo == 2);
  cajaTexto(250, -60, 300, 50, "Edad: ", inputEdad, campoActivo == 3);

  if (campoActivo == 1) {
    // GetCharPressed() devuelve un caracter por llamada (0 si no hay más); el while
    // captura todos los caracteres tecleados en este frame (por si se escribe rápido)
    int key = GetCharPressed();
    while (key > 0) {
      if (inputNombre.size() < 20) inputNombre += (char) key;
      key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE) && !inputNombre.empty()) inputNombre.pop_back();
  } else if (campoActivo == 2) {
    // Ya no se escribe libremente: el campo "Emergencia" siempre muestra el nombre
    // que está en indexEmergencia, y las flechas UP/DOWN mueven ese índice dentro
    // de la tabla (con wraparound: si pasa del final vuelve al inicio, y viceversa)
    inputEmergencia = TABLA_EMERGENCIAS[indexEmergencia].nombre;
    if (IsKeyPressed(KEY_DOWN)) indexEmergencia = (indexEmergencia + 1) % TABLA_EMERGENCIAS.size();
    if (IsKeyPressed(KEY_UP)) indexEmergencia = (indexEmergencia - 1 + TABLA_EMERGENCIAS.size()) % TABLA_EMERGENCIAS.size();
  } else if (campoActivo == 3) {
    int key = GetCharPressed();
    while (key > 0) {
      // Solo aceptamos dígitos para la edad
      if (key >= '0' && key <= '9' && inputEdad.size() < 3) inputEdad += (char) key;
      key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE) && !inputEdad.empty()) inputEdad.pop_back();
  }

  // addButton dibuja el botón Y lo registra en btns para poder detectar el click después
  vector<Button> btns;
  addButton(250, 20, 150, 50, "ANADIR", btns);
  addButton(250, 220, 150, 50, "ASIGNAR", btns);

  // Botón AÑADIR: solo agrega si nombre y edad no están vacíos Y la emergencia
  // existe en la tabla (siempre va a existir ahora, porque se elige con UP/DOWN
  // en vez de escribirse, pero se deja la validación igual por seguridad)
  if (isButtonPressed(btns[0])) {
    if (!inputNombre.empty() && !inputEdad.empty() && buscarSeveridad(inputEmergencia) != -1) {
      // push_back agrega el paciente al final del vector; no importa el orden de
      // inserción porque el sort de arriba lo va a reordenar en el siguiente frame
      cola.push_back({ inputNombre, inputEmergencia, stoi(inputEdad), GetTime() });
      // Limpiamos los campos para el siguiente paciente
      inputNombre.clear();
      inputEmergencia.clear();
      inputEdad.clear();
      campoActivo = 0;
      indexEmergencia = 0; // reiniciamos el selector de emergencia al primero de la tabla
    }
  }

  // Botón ASIGNAR: como la cola ya está ordenada (mayor prioridad primero),
  // el primer elemento SIEMPRE es el paciente con más puntaje.
  if (isButtonPressed(btns[1])) {
    if (!cola.empty()) {
      // Si aún no hemos creado la instancia persistente del paciente previo, la creamos
      if (prevPaciente == nullptr) {
        prevPaciente = new Paciente();
      }
      // Guardamos los datos del paciente antes de quitarlo de la cola
      *prevPaciente = cola[0];

      // erase(begin()) lo elimina y todos los demás se recorren una posición hacia adelante.
      cola.erase(cola.begin());
    }
  }

  text("Sala Disponible", 250, 150, 22, BLACK);

  // Imprimir los datos del paciente anterior si existe
  if (prevPaciente != nullptr) {
    string datosPrev = prevPaciente->nombre + " - " + prevPaciente->emergencia + " (" + to_string(prevPaciente->edad) + ")";
    text("Último Asignado:", 250, 290, 18, DARKGRAY);
    text(datosPrev.c_str(), 250, 320, 16, MAROON);
  }
}