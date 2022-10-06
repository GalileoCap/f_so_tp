#ifndef __UTILS_H__
#define __UTILS_H__

enum direccion {
  ARRIBA, ABAJO, IZQUIERDA, DERECHA, QUIETO
};

enum color {
  AZUL = 0,
  ROJO = 1,
  INDEFINIDO,
  VACIO,
  EMPATE,
  BANDERA_ROJA,
  BANDERA_AZUL
};

enum estrategia {
  SECUENCIAL,
  RR,
  SHORTEST,
  USTEDES
};

struct Pos {
  int x, y;
};

#define EMPIEZA ROJO
#define CONFIG_FPATH "./config/config_parameters.csv"

color contrincante(color equipo);
struct Pos moverseDireccion(struct Pos pos, direccion dir);

#endif // __UTILS_H__
