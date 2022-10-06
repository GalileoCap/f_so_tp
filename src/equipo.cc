#include "equipo.h"
#include "utils.h"
#include <iostream>

Equipo::Equipo(color equipo, estrategia strat, int quantum, struct GameMaster& belcebu, struct Config& config) {
  this->belcebu = &belcebu;
  this->equipo = equipo;
  this->strat = strat;
  this->quantum = quantum;
  this->posiciones = belcebu.jugadores[equipo];

  switch (this->strat) { //A: Preparo la estrategia elegida
    case SECUENCIAL:
      sem_init(&this->seq_sem, 1, 1);
      this->seq_turno = 0;
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
  while (true) {
    this->belcebu->barriers[this->equipo]->wait(); //A: Espero a que le toque a mi equipo 
    if (this->belcebu->ganador.load() != INDEFINIDO) break; //A: Terminó el juego

    switch (this->strat) { //A: Aplico la estrategia elegida
      case SECUENCIAL: this->secuencial(nroJugador); break;
      case RR: this->roundRobin(nroJugador); break;
      case SHORTEST: this->shortestDistanceFirst(nroJugador); break;
      case USTEDES: this->ustedes(nroJugador); break;
    }

    this->belcebu->barriers[this->equipo]->wait(); //A: Espero a que termine todo mi equipo
    if (this->belcebu->ganador.load() != INDEFINIDO) break; //A: Terminó el juego
  }
  std::cout << this->equipo << " " << nroJugador << " END" << std::endl;
}

void Equipo::secuencial(int nroJugador) {
  sem_wait(&this->seq_sem); //A: Espero a mi turno
  std::cout << this->equipo << " " << nroJugador << " IN" << std::endl;
  
  struct Pos to = {1, 1}; //TODO: Elegir de alguna manera
  this->moverseHacia(nroJugador, to);

  if (++this->seq_turno == this->jugadores.size()) { //A: Soy el último
    this->belcebu->terminoRonda(this->equipo); //A: Le aviso a belcebu
    this->seq_turno = 0;
  }

  std::cout << this->equipo << " " << nroJugador << " OUT" << std::endl;
  sem_post(&this->seq_sem); //A: Dejo jugar a alguien más
}

void Equipo::roundRobin(int nroJugador) {
}

void Equipo::shortestDistanceFirst(int nroJugador) {
}

void Equipo::ustedes(int nroJugador) {
}

struct Pos Equipo::buscarBanderaContraria(void) {
}

void Equipo::moverseHacia(int nroJugador, struct Pos to) {
  direccion dir = QUIETO;
  struct Pos from = this->posiciones[nroJugador];

  if (from.x < to.x) dir = IZQUIERDA;
  else if (from.x > to.x) dir = DERECHA;
  else if (from.y < to.y) dir = ARRIBA;
  else if (from.y > to.y) dir = ABAJO;
  //TODO: Check for collisions
  //TODO: Pathfinding

  this->belcebu->moverJugador(dir, nroJugador);
}
