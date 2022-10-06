#include "utils.h"

struct Pos Pos::mover(direccion dir) const {
  struct Pos pos = *this;

  switch (dir) {
    case IZQUIERDA: pos.x--; break;
    case DERECHA: pos.x++; break;
    case ARRIBA: pos.y++; break;
    case ABAJO: pos.y--; break;
    case QUIETO: break;
  }

  return pos;
}

color contrincante(color equipo) {
  return (equipo == ROJO) ? AZUL : ROJO;
}
