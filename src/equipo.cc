#include "equipo.h"
#include "utils.h"

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
  buscarBanderaContraria();
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
  int height, width; belcebu->tableroSize(height, width);
  bool found[2] = {false, false};

  const auto processorCount = std::thread::hardware_concurrency();
  std::vector<std::thread> searchThreads((width > processorCount ? processorCount : width));
  int block = width / searchThreads.size();
  for (int i = 0; i < searchThreads.size(); i++) {
    int from = block * i, to = std::min(from + block + 1, width); //TODO: Revisar redondeo
    searchThreads[i] = std::thread(&Equipo::buscarThread, this, height, from, to, std::ref(found[equipo]), std::ref(found[contrincante(equipo)]));
  }
  for (int i = 0; i < searchThreads.size(); i++)
    searchThreads[i].join();

  struct Pos posNuestra = banderas[equipo],
             posEnemigo = banderas[contrincante(equipo)];
  logMsg("EQUIPO buscarBanderaContraria equipo=%i, threads=%i, block=%i, posNuestra=(%i, %i), posEnemigo=(%i, %i), threads=%i\n", equipo, searchThreads.size(), block, posNuestra.x, posEnemigo.y, posEnemigo.x, posEnemigo.y);
  return posEnemigo;
}

void Equipo::buscarThread(int height, int from, int to, bool& foundOurs, bool& foundEnemy) {
  logMsg("EQUIPO buscarThread from=%i, to=%i\n", from, to);
  for (int x = from; x < to && !(foundOurs && foundEnemy); x++) //A: Recorro mi porción del tablero
    for (int y = 0; y < height && !(foundOurs && foundEnemy); y++) {
    struct Pos pos({x, y});
    //logMsg("EQUIPO buscarThread TRY nroThread=%i, pos=(%i, %i)\n", pos.x, pos.y);
    color enPos = belcebu->enPosicion(pos);
    if (enPos == bandera(equipo)) {
      foundOurs = true;
      banderas[equipo] = pos;
    } else if (enPos == bandera(contrincante(equipo))) {
      foundEnemy = true;
      banderas[contrincante(equipo)] = pos;
    }
  }
}

void Equipo::moverse(int nroJugador) {
  struct Pos from = posiciones[nroJugador],
             banderaEnemiga = banderas[contrincante(equipo)];

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
