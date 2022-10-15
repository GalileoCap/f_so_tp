#ifndef __GAMEMASTER_H__
#define __GAMEMASTER_H__

#include "barrier.h"
#include "config.h"
#include "utils.h"

#include <mutex>
#include <semaphore.h>

class GameMaster {
public:
  GameMaster(const class Config& config);
  
  void waitTurn(color equipo);
  void moverJugador(direccion dir, int nroJugador);
  void terminoRonda(color equipo);

  color enPosicion(const struct Pos& pos);
  bool mePuedoMover(struct Pos pos, direccion dir);

  color getGanador(void);

  sem_t semStart;
private:
  bool esPosicionValida(const struct Pos& pos);
  bool isEmpty(const struct Pos& pos);
  bool hasFlag(const struct Pos& pos, color equipo);

  void logTablero(void);

  color currEquipo, ganador;
  std::vector<struct Pos> posiciones[2];
  std::vector<std::vector<color>> tablero;

  std::mutex mtx;
  class Barrier *barriers[2]; //TODO: No punteros
};

#endif // __GAMEMASTER_H__
