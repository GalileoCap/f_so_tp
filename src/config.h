#ifndef CONFIG_H
#define CONFIG_H

#include "utils.h"
#include <vector>

struct Config {
  Config(void);

  std::vector<struct Pos> posiciones[2];
  struct Pos banderas[2];
  int ancho, alto,
      cantJugadores;
};

#endif // CONFIG_H
