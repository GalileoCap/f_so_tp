#ifndef __GAMEMASTER_H__
#define __GAMEMASTER_H__

#include "equipo.h"
#include "barrier.h"
#include "config.h"
#include "utils.h"

#include <mutex>

class GameMaster {
public:
  GameMaster(estrategia strat, int quantum, class Config& config);
  
  void comenzar(void);
  int terminar(void);

  void moverJugador(int nroJugador, direccion dir);
  void terminoRonda(color equipo);

  int waitTurn(color equipo);
private:
  bool isEmpty(struct Pos pos);
  bool hasFlag(struct Pos pos, color equipo);

  class Equipo* equipos[2];
  std::vector<std::vector<color>> tablero;

  color currEquipo, ganador;

  std::mutex mtx;
  class Barrier *barriers[2];
};

#endif // __GAMEMASTER_H__
