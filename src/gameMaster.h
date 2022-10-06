#ifndef GAMEMASTER_H
#define GAMEMASTER_H

#include "config.h"
#include "utils.h"

#include <semaphore.h>

struct GameMaster {
  GameMaster(struct Config& config);
  
  int moverJugador(const direccion dir, const int jugador);
  void terminoRonda(const color equipo);

  std::vector<struct Pos> jugadores[2];
  std::vector<std::vector<color>> tablero;

  color equipo, ganador;
  sem_t semaphores[2];
};

#endif // GAMEMASTER_H
