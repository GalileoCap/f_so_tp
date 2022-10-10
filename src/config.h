#ifndef CONFIG_H
#define CONFIG_H

#include "utils.h"

#include <vector>

class Config {
public:
  Config(void);

  std::vector<struct Pos> jugadores[2];
  struct Pos banderas[2];
  int ancho, alto,
      cantJugadores;
private:
};

#endif // CONFIG_H
