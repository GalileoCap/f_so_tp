#include "equipo.h"

Equipo::Equipo(color _equipo, estrategia _strat, int _quantum, struct GameMaster& _belcebu, struct Config& _config) {
  belcebu = &_belcebu;
  equipo = _equipo;
  strat = _strat;
  quantum = _quantum;

  threads.resize(_config.cantJugadores);

  switch (strat) { //A: Preparo la estrategia elegida
    case SECUENCIAL:
      sem_init(&seq_sem, 1, 1);
      seq_turno = 0;
      break;
    case RR: break;
    case SHORTEST: break;
    case USTEDES: break;
  }
}

void Equipo::comenzar(void) {
  for (int i = 0; i < threads.size(); i++) //A: Creo los threads de mis jugadores
    threads[i] = std::thread(&Equipo::jugador, this, i);
}

void Equipo::terminar(void) {
  for (auto& t : threads) t.join();
}

bool Equipo::esperarBelcebu(void) {
  return belcebu->barriers[equipo]->wait() != INDEFINIDO; //A: Espero a belcebu y me fijo si se terminó el juego
}

void Equipo::jugador(int nroJugador) {
  while (true) {
    if (esperarBelcebu()) break; //A: Espero a que le toque a mi equipo y si se terminó el juego termino

    switch (strat) { //A: Aplico la estrategia elegida
      case SECUENCIAL: secuencial(nroJugador); break;
      case RR: roundRobin(nroJugador); break;
      case SHORTEST: shortestDistanceFirst(nroJugador); break;
      case USTEDES: ustedes(nroJugador); break;
    }
  }
}

void Equipo::secuencial(int nroJugador) {
  sem_wait(&seq_sem); //A: Espero a mi turno
  
  struct Pos to = (equipo == ROJO) ? Pos({4, 4}) : Pos({1, 1}); //TODO: Elegir de alguna manera
  moverseHacia(nroJugador, to);

  if (++seq_turno == threads.size()) { //A: Soy el último
    belcebu->terminoRonda(equipo); //A: Le aviso a belcebu
    seq_turno = 0;
  }

  sem_post(&seq_sem); //A: Dejo jugar a alguien más
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
  struct Pos from = belcebu->jugadores[equipo][nroJugador];

  direccion dir;
  if (from.x > to.x) dir = IZQUIERDA;
  else if (from.x < to.x) dir = DERECHA;
  else if (from.y < to.y) dir = ARRIBA;
  else if (from.y > to.y) dir = ABAJO;
  else dir = QUIETO;
  //TODO: Pathfinding

  belcebu->moverJugador(nroJugador, dir);
}
