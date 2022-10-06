#ifndef GAMEMASTER_H
#define GAMEMASTER_H

#include "barrier.h"
#include "config.h"
#include "utils.h"

#include <atomic>
#include <mutex>

struct GameMaster {
  GameMaster(struct Config& config);
  
  void moverJugador(const direccion dir, const int jugador);
  void terminoRonda(const color equipo);

  std::vector<struct Pos> jugadores[2];
  std::vector<std::vector<color>> tablero;

  color equipo;
  std::atomic<color> ganador;
  int turno;

  std::mutex mtx;
  struct Barrier *barriers[2];
};

#endif // GAMEMASTER_H
