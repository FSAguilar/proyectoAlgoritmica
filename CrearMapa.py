import osmnx as ox

print("Cargando mapa desde map.osm...")
G = ox.graph_from_xml("map.osm")

print("Generando imagen del mapa...")
fig, ax = ox.plot_graph(
    G,
    bgcolor="#0d1117",
    edge_color="#aaaaaa",
    node_size=0,
    edge_linewidth=0.5,
    figsize=(40, 40),
    save=False,   # guardamos manualmente para obtener los límites primero
    show=False,
    close=False,
)

# Obtener los límites exactos que matplotlib usó para renderizar
x_min, x_max = ax.get_xlim()  # longitudes (WEST, EAST)
y_min, y_max = ax.get_ylim()  # latitudes  (SOUTH, NORTH)

print(f"\n=== LIMITES EXACTOS DEL RENDER ===")
print(f"IMG_NORTH = {y_max}")
print(f"IMG_SOUTH = {y_min}")
print(f"IMG_EAST  = {x_max}")
print(f"IMG_WEST  = {x_min}")
print(f"==================================\n")

# Guardar sin márgenes extra
fig.savefig("mapa_fondo.png", dpi=300, bbox_inches="tight", pad_inches=0)
import matplotlib.pyplot as plt
plt.close(fig)

print("Imagen guardada como mapa_fondo.png")
print("Copia los valores IMG_* de arriba a opcion1.cpp")