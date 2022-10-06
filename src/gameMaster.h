#ifndef __GAMEMASTER_H__
#define __GAMEMASTER_H__

#include "barrier.h"
#include "config.h"
#include "utils.h"

#include <atomic>
#include <mutex>

struct GameMaster {
  GameMaster(struct Config& config);
  
  void moverJugador(int nroJugador, direccion dir);
  void terminoRonda(color _equipo);

  bool isEmpty(struct Pos pos);
  bool hasFlag(struct Pos pos, color _equipo);

  std::vector<struct Pos> jugadores[2];
  std::vector<std::vector<color>> tablero;

  color equipo, ganador;

  std::mutex mtx;
  struct Barrier *barriers[2];
};

#endif // __GAMEMASTER_H__
