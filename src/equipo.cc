#include "equipo.h"
#ifdef DEBUG
#include <iostream>
#endif // DEBUG

Equipo::Equipo(color _equipo, estrategia _strat, int _quantum, class GameMaster *_belcebu, class Config& config) {
  belcebu = _belcebu;
  equipo = _equipo;
  strat = _strat;
  quantum = _quantum;

  posiciones = config.jugadores[equipo];

  threads.resize(config.cantJugadores);

  switch (strat) { //A: Preparo la estrategia elegida
    case SECUENCIAL:
      seq_mtx.unlock();
      seq_turno = 0;
      break;
    case RR:
      quantumLeft = quantum;
      rr_mtx.resize(config.cantJugadores);
      for (int i = 0; i < config.cantJugadores; i++) { //A: Inicio todos los mutex sin que los dejen pasar
        rr_mtx[i] = new std::mutex();
        rr_mtx[i]->lock();
      }
      rr_mtx[0]->unlock(); //A: Dejo pasar al primero
      break;
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
  return belcebu->waitTurn(equipo) != INDEFINIDO; //A: Espero a belcebu y me fijo si se terminó el juego
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
  seq_mtx.lock(); //A: Espero a mi turno
#ifdef DEBUG
    std::cout << equipo << " " << nroJugador << " STEP" << std::endl;
#endif // DEBUG
  
  struct Pos to = (equipo == ROJO) ? Pos({4, 4}) : Pos({1, 1}); //TODO: Elegir de alguna manera
  moverseHacia(nroJugador, to);

  if (++seq_turno == threads.size()) { //A: Soy el último
#ifdef DEBUG
    std::cout << equipo << " " << nroJugador << " END" << std::endl;
#endif // DEBUG

    belcebu->terminoRonda(equipo); //A: Le aviso a belcebu
    seq_turno = 0;
  }

  seq_mtx.unlock(); //A: Dejo jugar a alguien más
}

void Equipo::roundRobin(int nroJugador) {
  bool seguir = quantum > 0;
  while (seguir) {
    rr_mtx[nroJugador]->lock(); //A: Espero a mi turno //NOTA: Soy el único corriendo durante mi turno
#ifdef DEBUG
    std::cout << equipo << " " << nroJugador << " " << quantumLeft << " STEP" << std::endl;
#endif // DEBUG

    struct Pos to = (equipo == ROJO) ? Pos({4, 4}) : Pos({1, 1}); //TODO: Elegir de alguna manera
    moverseHacia(nroJugador, to);

    if (--quantumLeft == 0) { //A: Soy el último
#ifdef DEBUG
      std::cout << equipo << " " << nroJugador << " LAST" << std::endl;
#endif // DEBUG
      belcebu->terminoRonda(equipo); //A: Le aviso a belcebú
      quantumLeft = quantum; //A: Reinicio el quantumLeft
      rr_mtx[0]->unlock(); //A: Dejo pasar al primero para la siguiente ronda
      break; //A: Salgo
    } else { //A: Le toca al siguiente
      seguir = quantumLeft >= threads.size(); //A: Queda suficiente quantum para que me vuelva a tocar
      rr_mtx[(nroJugador + 1) % rr_mtx.size()]->unlock(); //A: Dejo pasar al siguiente
    }
  }

#ifdef DEBUG
  std::cout << equipo << " " << nroJugador << " END" << std::endl;
#endif // DEBUG
}

void Equipo::shortestDistanceFirst(int nroJugador) {
}

void Equipo::ustedes(int nroJugador) {
}

struct Pos Equipo::buscarBanderaContraria(void) {
}

void Equipo::moverseHacia(int nroJugador, struct Pos to) {
  struct Pos from = posiciones[nroJugador];

  direccion dir;
  if (from.x > to.x) dir = IZQUIERDA;
  else if (from.x < to.x) dir = DERECHA;
  else if (from.y < to.y) dir = ARRIBA;
  else if (from.y > to.y) dir = ABAJO;
  else dir = QUIETO;
  //TODO: Pathfinding

  belcebu->moverJugador(nroJugador, dir);
}
