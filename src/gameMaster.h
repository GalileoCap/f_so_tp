#ifndef GAMEMASTER_H
#define GAMEMASTER_H

#include "barrier.h"
#include "config.h"
#include "utils.h"

struct GameMaster {
  GameMaster(struct Config& config);
  
  int moverJugador(const direccion dir, const int jugador);
  void terminoRonda(const color equipo);

  std::vector<struct Pos> jugadores[2];
  std::vector<std::vector<color>> tablero;

  color equipo, ganador;
  struct Barrier *barriers[2];
};

#endif // GAMEMASTER_H
