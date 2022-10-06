#include "equipo.h"

Equipo::Equipo(color equipo, estrategia strat, int quantum, struct GameMaster& belcebu, struct Config& config) {
  this->belcebu = &belcebu;
  this->equipo = equipo;
  this->strat = strat;
  this->posiciones = config.jugadores[equipo];
}

void Equipo::comenzar(void) {
  //TODO: Esperar a mi turno y correr los threads
}

void Equipo::terminar(void) {
  //TODO: Unir todos los threads
}

void Equipo::jugador(int nroJugador) {
  //TODO: Esperar a mi turno (como equipo y como jugador) y aplicar la estrategia elegida
}

struct Pos Equipo::buscarBanderaContraria(void) {
}
