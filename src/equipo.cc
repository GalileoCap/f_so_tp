#include "equipo.h"

Equipo::Equipo(
  class GameMaster *belcebu,
  color equipo, estrategia strat, int cantJugadores, int quantum,
  const std::vector<struct Pos>& posiciones
) : belcebu(belcebu), equipo(equipo), strat(strat), quantum(quantum), posiciones(posiciones) {
  threads.reserve(cantJugadores); //NOTA: Las inicio en comenzar

  switch (strat) { //A: Preparo la estrategia elegida
    case SECUENCIAL:
      seq_mtx.unlock(); //A: Dejo pasar al primer jugador
      seq_turno = 0;
      break;

    case RR:
      quantumLeft = quantum;
      rr_mtx.reserve(cantJugadores);
      for (int i = 0; i < cantJugadores; i++) { //A: Inicio todos los mutex sin que los dejen pasar
        rr_mtx.push_back(new std::mutex);
        rr_mtx[i]->lock();
      }
      rr_mtx[0]->unlock(); //A: Dejo pasar al primero
      break;

    case SHORTEST:
      sdf_distances.resize(cantJugadores);
      sem_init(&sdf_step[0], 1, 0); //A: Inicio los semáforos sin dejar pasar
      sem_init(&sdf_step[1], 1, 0);
      sdf_turno = 0;
      break;

    case USTEDES: break; //TODO
  }

  logMsg("EQUIPO DONE equipo=%i\n", equipo);
}

void Equipo::comenzar(void) { //U: Busca las banderas y luego inicia a sus jugadores
  logMsg("EQUIPO comenzar equipo=%i\n", equipo);

  buscarBanderas();

  logMsg("EQUIPO comenzar THREADS equipo=%i\n", equipo);
  for (int i = 0; i < posiciones.size(); i++) //A: Creo los threads de mis jugadores
    threads.emplace_back(&Equipo::jugador, this, i);
}

void Equipo::terminar(void) { //U: Espera a que terminen todos los jugadores de este equipo
  for (auto& t : threads) t.join();

  logMsg("EQUIPO terminar equipo=%i\n", equipo);
}

bool Equipo::esperarBelcebu(void) {
  return belcebu->waitTurn(equipo) != INDEFINIDO; //A: Espero a que le toque a mi equipo, devuelve true si se terminó el juego
}

void Equipo::jugador(int nroJugador) {
  while (true) {
    logMsg("EQUIPO jugador WAIT equipo=%i, nroJugador=%i\n", equipo, nroJugador);

    if (esperarBelcebu()) break;

    switch (strat) { //A: Aplico la estrategia elegida
      case SECUENCIAL: secuencial(nroJugador); break;
      case RR: roundRobin(nroJugador); break;
      case SHORTEST: shortestDistanceFirst(nroJugador); break;
      case USTEDES: ustedes(nroJugador); break;
    }
  }

  logMsg("EQUIPO jugador END equipo=%i, nroJugador=%i\n", equipo, nroJugador);
}

void Equipo::moverse(int nroJugador, const struct Pos& to) { //U: Mueve al jugador hacia una posición
  struct Pos from = posiciones[nroJugador];

  direccion dir;
  if (from.x > to.x && belcebu->mePuedoMover(from, IZQUIERDA)) dir = IZQUIERDA;
  else if (from.x < to.x && belcebu->mePuedoMover(from, DERECHA)) dir = DERECHA;
  else if (from.y < to.y && belcebu->mePuedoMover(from, ARRIBA)) dir = ARRIBA;
  else if (from.y > to.y && belcebu->mePuedoMover(from, ABAJO)) dir = ABAJO;
  else /* from == to || noMePuedoMover */ return;

  logMsg("EQUIPO moverse equipo=%i, nroJugador=%i, from=(%i, %i), dir=%i\n", equipo, nroJugador, from.x, from.y, dir);

  posiciones[nroJugador] = from.mover(dir); //A: Actualizo la posición
  belcebu->moverJugador(dir, nroJugador); //A: Le pido a belcebú que me mueva
}

//************************************************************
//S: Estrategias

void Equipo::secuencial(int nroJugador) {
  seq_mtx.lock(); //A: Espero a mi turno
  logMsg("EQUIPO secuencial STEP equipo=%i, nroJugador=%i\n", equipo, nroJugador);
  
  moverse(nroJugador, banderas[contrincante(equipo)]);

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

    moverse(nroJugador, banderas[contrincante(equipo)]); //TODO: Hacia otro lugar?

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
  sdf_distances[nroJugador] = posiciones[nroJugador].distance(banderas[contrincante(equipo)]); //A: Calculo mi distancia //NOTA: Como todos sólo modifican su propio valor, no es parte del area crítica
  sdf_closest = 0; //A: Reinicio el closest //NOTA: Como todos ponen el mismo valor, no pertenece al area crítica
  sdf_mtx.lock();
  if (++sdf_turno == threads.size()) //A: Soy el último
    for (int i = 0; i < threads.size(); i++) sem_post(&sdf_step[0]); //A: Dejo pasar a todos
  sdf_mtx.unlock();
  sem_wait(&sdf_step[0]);

  sdf_mtx.lock();
  if (sdf_distances[nroJugador] < sdf_distances[sdf_closest]) sdf_closest = nroJugador; //A: Si soy el más cercano hasta ahora, me anoto

  if (--sdf_turno == 0) //A: Soy el último
    for (int i = 0; i < threads.size(); i++) sem_post(&sdf_step[1]); //A: Dejo pasar a todos
  sdf_mtx.unlock();
  sem_wait(&sdf_step[1]);

  if (nroJugador == sdf_closest) { //A: Soy el más cercano
    moverse(nroJugador, banderas[contrincante(equipo)]);
    belcebu->terminoRonda(equipo);
  }

  //TODO: ¿Se hace esto hasta agotar el quantum?
}

void Equipo::ustedes(int nroJugador) {
  //TODO
}

//************************************************************
//S: BuscarBandera

void Equipo::buscarBanderas(void) { //U: Busca ambas banderas en el tablero
  sem_wait(&belcebu->semBandera); //A: Espero a mi turno de buscar la bandera
  logMsg("EQUIPO buscarBanderas equipo=%i\n", equipo);

  int height, width; belcebu->tableroSize(height, width);
  bool found[2] = {false, false}; //U: True cuando se encontró a esa bandera

  const auto processorCount = std::thread::hardware_concurrency(); //A: Me fijo cuantos threads máximo puedo usar //NOTA: Devuelve 0 si no se puede averiguar por alguna razón
  std::vector<std::thread> searchThreads((width > processorCount ? processorCount : width)); //A: Limito los threads al ancho del tablero //TODO: Dividir a lo ancho y a lo alto, para no limitar así //NOTA: Si processorCount es 0 también se limita

  int block = width / searchThreads.size(); //U: Ancho del bloque que va revisar cada thread //TODO: Revisar redondeo
  for (int i = 0; i < searchThreads.size(); i++) { //A: Creo los threads
    int from = block * i, to = std::min(from + block, width);
    searchThreads[i] = std::thread(
      &Equipo::buscarThread, this,
      height, from, to, std::ref(found[equipo]), std::ref(found[contrincante(equipo)])
    );
  }
  for (auto& t : searchThreads) t.join(); //A: Espero a que terminen

  struct Pos posNuestra = banderas[equipo],
             posEnemigo = banderas[contrincante(equipo)];
  logMsg("EQUIPO buscarBanderas equipo=%i, threads=%i, block=%i, posNuestra=(%i, %i), posEnemigo=(%i, %i), threads=%i\n", equipo, searchThreads.size(), block, posNuestra.x, posEnemigo.y, posEnemigo.x, posEnemigo.y);

  sem_post(&belcebu->semBandera); //A: Dejo que el otro equipo busque la bandera //NOTA: El segundo equipo en buscarla va hacer un post que puede ser ignorado ya que nadie va a esperarlo
}

void Equipo::buscarThread(int height, int from, int to, bool& foundOurs, bool& foundEnemy) { //U: Cada thread recorre una porción del tablero hasta terminar o que entre todos hayan encontrado ambas banderas
  logMsg("EQUIPO buscarThread from=%i, to=%i\n", from, to);

  for (int x = from; x < to && !(foundOurs && foundEnemy); x++) //A: Recorro el tablero
    for (int y = 0; y < height && !(foundOurs && foundEnemy); y++) {
    struct Pos pos({x, y});
    color enPos = belcebu->enPosicion(pos);
    if (enPos == bandera(equipo)) { //A: Encontré mi bandera
      foundOurs = true;
      banderas[equipo] = pos;
    } else if (enPos == bandera(contrincante(equipo))) { //A: Encontré la bandera contraria
      foundEnemy = true;
      banderas[contrincante(equipo)] = pos;
    }
  }
  //NOTA: No hay problema con que estemos asignando los valores sin un mtx porque vamos a setear el mismo valor
  //      Aunque sí es posible que haya problemas de sincronización al revisar si ya se encontaron las banderas, pero arreglarlos agrega complejidad y probablemente tarde más que el thread revise una posición extra
}
