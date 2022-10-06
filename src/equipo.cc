#include "equipo.h"
#include "utils.h"
#include <iostream>

Equipo::Equipo(color equipo, estrategia strat, int quantum, struct GameMaster& belcebu, struct Config& config) {
  this->belcebu = &belcebu;
  this->equipo = equipo;
  this->strat = strat;
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
    switch (this->strat) { //A: Aplico la estrategia elegida
      case SECUENCIAL: this->secuencial(nroJugador);
      case RR: this->roundRobin(nroJugador);
      case SHORTEST: this->shortestDistanceFirst(nroJugador);
      case USTEDES: this->ustedes(nroJugador);
    }
    //TODO: ¿Qué más?
  }
}

void Equipo::secuencial(int nroJugador) {
}

void Equipo::roundRobin(int nroJugador) {
}

void Equipo::shortestDistanceFirst(int nroJugador) {
}

void Equipo::ustedes(int nroJugador) {
}

struct Pos Equipo::buscarBanderaContraria(void) {
}
