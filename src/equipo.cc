#include "equipo.h"
#include "utils.h"
#include <iostream>
#include <semaphore.h>
#include <utility>

Equipo::Equipo(color equipo, estrategia strat, int quantum, struct GameMaster& belcebu, struct Config& config) {
  this->belcebu = &belcebu;
  this->equipo = equipo;
  this->strat = strat;
  this->quantum = quantum;
  this->posiciones = belcebu.jugadores[equipo];

  switch (this->strat) { //A: Preparo la estrategia elegida
    case SECUENCIAL:
      sem_init(&seqSem, 1, 0);
      break;
    case RR: break;
    case SHORTEST: break;
    case USTEDES: break;
  }
}

void Equipo::comenzar(void) {
  for (int i = 0; i < this->belcebu->jugadores[this->equipo].size(); i++) //A: Creo los threads de mis jugadores
    this->jugadores.emplace_back(std::thread(&Equipo::jugador, this, i));
}

void Equipo::terminar(void) {
  for (auto& t : this->jugadores) t.join();
}

void Equipo::jugador(int nroJugador) {
  while (belcebu->ganador == INDEFINIDO) {
    belcebu->barriers[equipo]->wait(); //A: Espero a que le toque a mi equipo 
    switch (this->strat) { //A: Aplico la estrategia elegida
      case SECUENCIAL: this->secuencial(nroJugador); break;
      case RR: this->roundRobin(nroJugador); break;
      case SHORTEST: this->shortestDistanceFirst(nroJugador); break;
      case USTEDES: this->ustedes(nroJugador); break;
    }
    //TODO: ¿Qué más?
  }
}

void Equipo::secuencial(int nroJugador) {
  sem_wait(&this->seqSem); //A: Espero a mi turno

  struct Pos to = {0, 0}; //TODO: Elegir de alguna manera
  this->moverseHacia(nroJugador, to);

  sem_post(&this->seqSem); //A: Dejo jugar a alguien más
}

void Equipo::roundRobin(int nroJugador) {
}

void Equipo::shortestDistanceFirst(int nroJugador) {
}

void Equipo::ustedes(int nroJugador) {
}

struct Pos Equipo::buscarBanderaContraria(void) {
}

int Equipo::moverseHacia(int nroJugador, struct Pos to) {
  direccion dir = QUIETO;
  struct Pos from = this->posiciones[nroJugador];

  if (from.x < to.x) dir = IZQUIERDA;
  else if (from.x > to.x) dir = DERECHA;
  else if (from.y < to.y) dir = ARRIBA;
  else if (from.y > to.y) dir = ABAJO;
  //TODO: Check for collisions
  //TODO: Pathfinding

  return this->belcebu->moverJugador(dir, nroJugador);
}
