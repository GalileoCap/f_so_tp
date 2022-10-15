#include "equipo.h"

Equipo::Equipo(class GameMaster *belcebu, color equipo, estrategia strat, int cantJugadores, int quantum, const std::vector<struct Pos>& posiciones) : belcebu(belcebu), equipo(equipo), strat(strat), quantum(quantum), posiciones(posiciones) {
  threads.resize(cantJugadores);

  switch (strat) { //A: Preparo la estrategia elegida
    case SECUENCIAL:
      seq_mtx.unlock(); //A: Dejo pasar al primer jugador
      seq_turno = 0;
      break;

    case RR:
      quantumLeft = quantum;
      rr_mtx.resize(cantJugadores);
      for (int i = 0; i < cantJugadores; i++) { //A: Inicio todos los mutex sin que los dejen pasar
        rr_mtx[i] = new std::mutex();
        rr_mtx[i]->lock();
      }
      rr_mtx[0]->unlock(); //A: Dejo pasar al primero
      break;

    case SHORTEST: break; //TODO

    case USTEDES: break; //TODO
  }

  logMsg("EQUIPO DONE equipo=%i\n", equipo);
}

void Equipo::comenzar(void) {
  logMsg("EQUIPO comenzar equipo=%i\n", equipo);

  sem_wait(&belcebu->semStart); //A: Espero a poder buscar la bandera
  banderaEnemiga = buscarBanderaContraria();
  sem_post(&belcebu->semStart); //A: Dejo que el otro equipo busque la bandera

  logMsg("EQUIPO comenzar THREADS equipo=%i\n", equipo);
  for (int i = 0; i < threads.size(); i++) //A: Creo los threads de mis jugadores
    threads[i] = std::thread(&Equipo::jugador, this, i);
}

void Equipo::terminar(void) {
  for (auto& t : threads) t.join();

  logMsg("EQUIPO terminar equipo=%i\n", equipo);
}

void Equipo::jugador(int nroJugador) {
  while (true) {
    logMsg("EQUIPO jugador WAIT equipo=%i, nroJugador=%i\n", equipo, nroJugador);
    belcebu->waitTurn(equipo); //A: Espero a que le toque a mi equipo
    if (belcebu->getGanador() != INDEFINIDO) break; //A: Si se terminó el juego termino acá

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
  logMsg("EQUIPO secuencial STEP equipo=%i, nroJugador=%i\n", equipo, nroJugador);
  
  moverse(nroJugador);

  if (++seq_turno == threads.size()) { //A: Soy el último
    logMsg("EQUIPO secuencial LAST equipo=%i, nroJugador=%i\n", equipo, nroJugador);

    belcebu->terminoRonda(equipo); //A: Le aviso a belcebu
    seq_turno = 0;
  }

  logMsg("EQUIPO secuencial END equipo=%i, nroJugador=%i\n", equipo, nroJugador);
  seq_mtx.unlock(); //A: Dejo jugar a alguien más
}

void Equipo::roundRobin(int nroJugador) {
  bool seguir = quantum > 0;
  while (seguir) {
    rr_mtx[nroJugador]->lock(); //A: Espero a mi turno //NOTA: Soy el único corriendo durante mi turno
    logMsg("EQUIPO roundRobin STEP equipo=%i, nroJugador=%i\n", equipo, nroJugador);

    moverse(nroJugador);

    if (--quantumLeft == 0) { //A: Soy el último
      logMsg("EQUIPO roundRobin LAST equipo=%i, nroJugador=%i\n", equipo, nroJugador);
      belcebu->terminoRonda(equipo); //A: Le aviso a belcebú
      quantumLeft = quantum; //A: Reinicio el quantumLeft
      rr_mtx[0]->unlock(); //A: Dejo pasar al primero para la siguiente ronda
      break; //A: Salgo
    } else { //A: Le toca al siguiente
      seguir = quantumLeft >= threads.size(); //A: Queda suficiente quantum para que me vuelva a tocar
      rr_mtx[(nroJugador + 1) % rr_mtx.size()]->unlock(); //A: Dejo pasar al siguiente
    }
  }

  logMsg("EQUIPO roundRobin END equipo=%i, nroJugador=%i\n", equipo, nroJugador);
}

void Equipo::shortestDistanceFirst(int nroJugador) {
  //TODO
}

void Equipo::ustedes(int nroJugador) {
  //TODO
}

struct Pos Equipo::buscarBanderaContraria(void) {
  struct Pos pos;

  pos = (equipo == ROJO) ? Pos({4, 4}) : Pos({1, 1}); //TODO

  logMsg("EQUIPO buscarBanderaContraria equipo=%i, pos=(%i, %i)\n", equipo, pos.x, pos.y);
  return pos;
}

void Equipo::moverse(int nroJugador) {
  struct Pos from = posiciones[nroJugador];

  direccion dir;
  if (from.x > banderaEnemiga.x && belcebu->mePuedoMover(from, IZQUIERDA)) dir = IZQUIERDA;
  else if (from.x < banderaEnemiga.x && belcebu->mePuedoMover(from, DERECHA)) dir = DERECHA;
  else if (from.y < banderaEnemiga.y && belcebu->mePuedoMover(from, ARRIBA)) dir = ARRIBA;
  else if (from.y > banderaEnemiga.y && belcebu->mePuedoMover(from, ABAJO)) dir = ABAJO;
  else /* banderaEnemiga == from */ return;

  logMsg("EQUIPO moverse equipo=%i, nroJugador=%i, from=(%i, %i), dir=%i\n", equipo, nroJugador, from.x, from.y, dir);
  posiciones[nroJugador] = from.mover(dir);
  belcebu->moverJugador(dir, nroJugador);
}
