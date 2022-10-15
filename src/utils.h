#ifndef __UTILS_H__
#define __UTILS_H__

#ifdef DEBUG
#include <cstdio>
#include <cstdarg>
#endif // DEBUG

#define EMPIEZA ROJO
#define CONFIG_FPATH "./config/config_parameters.csv"

enum direccion {
  ARRIBA, ABAJO, IZQUIERDA, DERECHA
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
  struct Pos mover(direccion dir) const;
  int x, y;
};

color contrincante(const color equipo);
color bandera(const color equipo);

void logMsg(const char msg[], ...);

#endif // __UTILS_H__
