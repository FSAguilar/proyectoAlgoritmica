import osmnx as ox
import struct
import os

print("=========================================================")
print("Cargando mapa desde el archivo XML local 'map.osm'...")
print("=========================================================")

try:
    # 1. Leer el archivo XML/OSM local que descargaste manualmente
    # No consume internet y se procesa directamente en tu máquina
    G = ox.graph_from_xml("map.osm")
except Exception as e:
    print(f"\n[ERROR] No se pudo abrir el archivo local: {e}")
    print("Asegúrate de que el archivo se llame 'map.osm' y esté en la misma carpeta que este script.")
    exit()

print("Procesando la topología del grafo de La Paz...")

# 2. Mapear los IDs largos de OpenStreetMap a enteros continuos (0 a N-1)
nodos_mapping = {nodo: i for i, nodo in enumerate(G.nodes)}
num_nodos = len(G.nodes)

# 3. Construir la estructura de la lista de adyacencia temporal en memoria
grafo_vectores = [[] for _ in range(num_nodos)]
for u, v, data in G.edges(data=True):
    u_idx = nodos_mapping[u]
    v_idx = nodos_mapping[v]
    
    # Peso de la arista en metros (forzado a float estándar de Python)
    peso = float(round(data.get("length", 0.0), 2))
    grafo_vectores[u_idx].append((v_idx, peso))

# 4. Procesar las coordenadas reales y calcular los límites geográficos máximos/mínimos
metadatos_nodos = [None] * num_nodos
lats = []
lons = []

for nodo_osm, idx in nodos_mapping.items():
    nodo_data = G.nodes[nodo_osm]
    lat = nodo_data.get("y", 0.0)
    lon = nodo_data.get("x", 0.0)
    lats.append(lat)
    lons.append(lon)
    metadatos_nodos[idx] = {"lat": lat, "lon": lon}

min_lat, max_lat = min(lats), max(lats)
min_lon, max_lon = min(lons), max(lons)

# Contar cuántas aristas totales se van a escribir en el bloque secuencial plano
total_aristas = sum(len(v) for v in grafo_vectores)

# Asegurar que existan las carpetas de destino de tu estructura C++
os.makedirs("include", exist_ok=True)
os.makedirs("src", exist_ok=True)

# 5. EXPORTAR EL ARCHIVO BINARIO DE DATOS (mapa.bin)
print("\nGenerando archivo binario estructurado 'mapa.bin'...")
with open("mapa.bin", "wb") as f:
    # A. Escribir Cabecera Fija de Control: NumNodos (int) y TotalAristas (int)
    f.write(struct.pack("ii", num_nodos, total_aristas))
    
    # B. Escribir Límites Geográficos Extremos del Bounding Box: (double x 4)
    f.write(struct.pack("dddd", max_lat, min_lat, max_lon, min_lon))
    
    # C. Escribir Coordenadas Geográficas de los Nodos en bloque continuo (double x 2 por nodo)
    for meta in metadatos_nodos:
        f.write(struct.pack("dd", meta['lat'], meta['lon']))
        
    # D. Escribir las Aristas en bloque continuo: Origen (int), Destino (int), Peso (double)
    for u in range(num_nodos):
        for dest, peso in grafo_vectores[u]:
            f.write(struct.pack("iid", u, dest, peso))

# 6. EXPORTAR EL ARCHIVO DE CABECERA (include/grafo_la_paz.h)
print("Generando archivo de interfaz 'include/grafo_la_paz.h'...")
with open("include/grafo_la_paz.h", "w", encoding="utf-8") as f:
    f.write("#ifndef GRAFO_LA_PAZ_H\n")
    f.write("#define GRAFO_LA_PAZ_H\n\n")
    f.write("#include <vector>\n")
    f.write("#include <utility>\n")
    f.write("#include <fstream>\n")
    f.write("#include <iostream>\n\n")
    f.write("extern int NUM_NODOS;\n")
    f.write("extern double NORTH, SOUTH, EAST, WEST;\n\n")
    f.write("struct NodeMetadata {\n")
    f.write("    double lat;\n")
    f.write("    double lon;\n")
    f.write("};\n\n")
    f.write("extern std::vector<NodeMetadata> METADATOS_NODOS;\n")
    f.write("extern std::vector<std::vector<std::pair<int, double>>> MAPA_GRAFO;\n\n")
    f.write("bool CargarMapaBinario(const char* rutaArchivo);\n\n")
    f.write("#endif // GRAFO_LA_PAZ_H\n")

# 7. EXPORTAR LA IMPLEMENTACIÓN DEL LECTOR SECUENCIAL (src/grafo_la_paz.cpp)
print("Generando cargador de datos plano 'src/grafo_la_paz.cpp'...")
with open("src/grafo_la_paz.cpp", "w", encoding="utf-8") as f:
    f.write('#include "grafo_la_paz.h"\n\n')
    f.write("int NUM_NODOS = 0;\n")
    f.write("double NORTH = 0.0, SOUTH = 0.0, EAST = 0.0, WEST = 0.0;\n")
    f.write("std::vector<NodeMetadata> METADATOS_NODOS;\n")
    f.write("std::vector<std::vector<std::pair<int, double>>> MAPA_GRAFO;\n\n")
    f.write("bool CargarMapaBinario(const char* rutaArchivo) {\n")
    f.write("    std::ifstream f(rutaArchivo, std::ios::binary);\n")
    f.write("    if (!f.is_open()) return false;\n\n")
    f.write("    int total_aristas = 0;\n")
    f.write("    f.read((char*)&NUM_NODOS, sizeof(int));\n")
    f.write("    f.read((char*)&total_aristas, sizeof(int));\n")
    f.write("    f.read((char*)&NORTH, sizeof(double));\n")
    f.write("    f.read((char*)&SOUTH, sizeof(double));\n")
    f.write("    f.read((char*)&EAST, sizeof(double));\n")
    f.write("    f.read((char*)&WEST, sizeof(double));\n\n")
    f.write("    // Cargar bloque continuo de coordenadas de nodos\n")
    f.write("    METADATOS_NODOS.resize(NUM_NODOS);\n")
    f.write("    f.read((char*)METADATOS_NODOS.data(), NUM_NODOS * sizeof(NodeMetadata));\n\n")
    f.write("    // Inicializar la lista de adyacencia vacía para rellenar bajo demanda\n")
    f.write("    MAPA_GRAFO.assign(NUM_NODOS, std::vector<std::pair<int, double>>());\n\n")
    f.write("    // Cargar bloque continuo de aristas planas\n")
    f.write("    for (int i = 0; i < total_aristas; i++) {\n")
    f.write("        int u = 0, v = 0;\n")
    f.write("        double w = 0.0;\n")
    f.write("        f.read((char*)&u, sizeof(int));\n")
    f.write("        f.read((char*)&v, sizeof(int));\n")
    f.write("        f.read((char*)&w, sizeof(double));\n\n")
    f.write("        // Validación de seguridad para prevenir corrupción de memoria\n")
    f.write("        if (u >= 0 && u < NUM_NODOS) {\n")
    f.write("            MAPA_GRAFO[u].push_back({v, w});\n")
    f.write("        }\n")
    f.write("    }\n")
    f.write("    f.close();\n")
    f.write("    return true;\n")
    f.write("}\n")

print("\n=========================================================")
print(f"¡PROCESO COMPLETADO CON ÉXITO!")
print(f"Nodos indexados: {num_nodos}")
print(f"Aristas mapeadas: {total_aristas}")
print("=========================================================")