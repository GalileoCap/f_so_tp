#include "utils.h"

color contrincante(color equipo) { //TODO: Mover
  return (equipo == ROJO) ? AZUL : ROJO;
}

struct Pos moverseDireccion(struct Pos pos, direccion dir) {
  switch (dir) {
    case IZQUIERDA: pos.x--; break;
    case DERECHA: pos.x++; break;
    case ARRIBA: pos.y++; break;
    case ABAJO: pos.y--; break;
    case QUIETO: break;
  }
  return pos;
}


