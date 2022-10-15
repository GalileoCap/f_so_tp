#include "utils.h"

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

color contrincante(color equipo) {
  //TODO: Assert
  return (equipo == ROJO) ? AZUL : ROJO;
}

color bandera(const color equipo) {
  //TODO: Assert
  return (equipo == ROJO) ? BANDERA_ROJA : BANDERA_AZUL;
}

void logMsg(const char msg[], ...) {
#ifdef DEBUG
  va_list args;
  va_start(args, msg);
  vprintf(msg, args);
  va_end(args);
#endif //DEBUG
}
