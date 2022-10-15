#include "utils.h"
#include <cmath>

struct Pos Pos::mover(direccion dir) const {
  struct Pos pos = *this;

  switch (dir) {
    case IZQUIERDA: pos.x--; break;
    case DERECHA: pos.x++; break;
    case ARRIBA: pos.y++; break;
    case ABAJO: pos.y--; break;
  }

  return pos;
}

bool Pos::operator==(const struct Pos& pos) const {
  return x == pos.x && y == pos.y;
}

int Pos::distance(const struct Pos& pos) const { //A: Distancia Manhattan
  return std::abs(x - pos.x) + std::abs(y - pos.y);
}

color contrincante(color equipo) {
  return (equipo == ROJO) ? AZUL : ROJO;
}

color bandera(color equipo) {
  return (equipo == ROJO) ? BANDERA_ROJA : BANDERA_AZUL;
}

void logMsg(const char msg[], ...) { //U: Imprime un mensaje sólo si estamos en modo debug
#ifdef DEBUG
  va_list args;
  va_start(args, msg);
  vprintf(msg, args);
  va_end(args);
#endif //DEBUG
}
