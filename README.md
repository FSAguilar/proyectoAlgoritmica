# Guía de Configuración Rápida

Sigue estas instrucciones paso a paso para configurar tu entorno, instalar las dependencias y compilar el proyecto usando la terminal.

## 1. Instalar C++ (Compilador)

### Windows (usando MSYS2)
**Paso previo:** Windows no trae MSYS2 por defecto. Debes descargar e instalar [MSYS2 desde su página oficial (msys2.org)](https://www.msys2.org/). Una vez instalado, busca en tu menú de inicio el programa **"MSYS2 UCRT64"**.

Abre la terminal de **MSYS2 UCRT64** y ejecuta el siguiente comando para instalar el compilador GCC:
```bash
pacman -S mingw-w64-ucrt-x86_64-gcc
```

### Linux (Ubuntu/Debian)
Abre tu terminal y ejecuta:
```bash
sudo apt update
sudo apt install build-essential
```

---

## 2. Instalar Raylib

### Windows (usando MSYS2)
En la misma terminal de **MSYS2 UCRT64**, ejecuta este comando para instalar Raylib precompilado:
```bash
pacman -S mingw-w64-ucrt-x86_64-raylib
```

### Linux (Ubuntu/Debian)
En tu terminal, instala la librería de desarrollo de Raylib:
```bash
sudo apt install wget
wget https://github.com/raysan5/raylib/releases/download/5.0/raylib-5.0_linux_amd64.tar.gz
```
*(Si usas Arch Linux, puedes usar `sudo pacman -S raylib`)*

---

## 3. Compilar y Ejecutar


### Para compilar:

**En Windows:**
```bash
g++ src/*.cpp -o main.exe -Iinclude -lraylib -lopengl32 -lgdi32 -lwinmm
```

**En Linux:**
```bash
g++ src/*.cpp -o main -Iinclude -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
```
*(Nota: He incluido `-Iinclude` por si tienes archivos `.h` o `.hpp` en una carpeta `include/`, y `src/algorithms/*.cpp` asumiendo que tu código está ahí).*

### Para ejecutar:

**En Windows:**
```bash
./juego.exe
```

**En Linux:**
```bash
./juego
```


Como usar GIT explicado en git.md
