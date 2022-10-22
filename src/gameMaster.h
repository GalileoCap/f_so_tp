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
  
  color waitTurn(color equipo);
  void moverJugador(direccion dir, int nroJugador);
  void terminoRonda(color equipo);

  color enPosicion(const struct Pos& pos);
  bool mePuedoMover(struct Pos pos, direccion dir);

  color getGanador(void);
  void tableroSize(int& height, int& width);

  sem_t semBandera;
  color ganador;
#ifndef TEST
private:
#endif // TEST
  bool esPosicionValida(const struct Pos& pos);
  bool isEmpty(const struct Pos& pos);
  bool hasFlag(const struct Pos& pos, color equipo);

  void logTablero(void);

  color currEquipo;
  int movidas[2]; bool atentoMovidas = false;
  std::vector<struct Pos> posiciones[2];
  std::vector<std::vector<color>> tablero;

  class Barrier *barriers[2]; //TODO: No punteros, tira un error de compilación si no lo son
};

#endif // __GAMEMASTER_H__
