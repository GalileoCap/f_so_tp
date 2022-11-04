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
      quantum = std::max(quantum, cantJugadores); //A: En Round Robin todos los jugadores se tienen que mover al menos una vez
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

    case USTEDES:
      us_range[0] = 0; us_range[1] = quantum;
      us_count = 0;
      break;
  }

  logMsg("[Equipo] DONE equipo=%i, strat=%i\n", equipo, strat);
}

void Equipo::comenzar(void) { //U: Busca las banderas y luego inicia a sus jugadores
  logMsg("[comenzar] equipo=%i\n", equipo);

  buscarBanderas();

  logMsg("[comenzar] THREADS equipo=%i\n", equipo);
  for (int i = 0; i < posiciones.size(); i++) //A: Creo los threads de mis jugadores
    threads.emplace_back(&Equipo::jugador, this, i);
}

void Equipo::terminar(void) { //U: Espera a que terminen todos los jugadores de este equipo
  for (auto& t : threads) t.join();

  logMsg("[terminar] equipo=%i\n", equipo);
}

bool Equipo::esperarBelcebu(void) {
  return belcebu->waitTurn(equipo) != INDEFINIDO; //A: Espero a que le toque a mi equipo, devuelve true si se terminó el juego
}

void Equipo::jugador(int nroJugador) {
  while (true) {
    logMsg("[jugador] WAIT equipo=%i, nroJugador=%i\n", equipo, nroJugador);

    if (esperarBelcebu()) break;

    switch (strat) { //A: Aplico la estrategia elegida
      case SECUENCIAL: secuencial(nroJugador); break;
      case RR: roundRobin(nroJugador); break;
      case SHORTEST: shortestDistanceFirst(nroJugador); break;
      case USTEDES: ustedes(nroJugador); break;
    }
  }

  logMsg("[jugador] END equipo=%i, nroJugador=%i\n", equipo, nroJugador);
}

void Equipo::moverse(int nroJugador, const struct Pos& to) { //U: Mueve al jugador hacia una posición
  //NOTA: Por como lo implementamos, cada estrategia se asegura por su cuenta que sólo un jugador se mueva a la vez
  struct Pos from = posiciones[nroJugador];

  direccion dir;
  if (from.x > to.x && belcebu->mePuedoMover(from, IZQUIERDA)) dir = IZQUIERDA;
  else if (from.x < to.x && belcebu->mePuedoMover(from, DERECHA)) dir = DERECHA;
  else if (from.y < to.y && belcebu->mePuedoMover(from, ARRIBA)) dir = ARRIBA;
  else if (from.y > to.y && belcebu->mePuedoMover(from, ABAJO)) dir = ABAJO;
  //TODO: else if (mePuedoMoverAlguna) dir = movidaValidaRandom
  else /* from == to || noMePuedoMover */ return;

  logMsg("[moverse] equipo=%i, nroJugador=%i, from=(%i, %i), dir=%i\n", equipo, nroJugador, from.x, from.y, dir);

  posiciones[nroJugador] = from.mover(dir); //A: Actualizo la posición
  belcebu->moverJugador(dir, nroJugador); //A: Le pido a belcebú que me mueva
}

//************************************************************
//S: Estrategias

void Equipo::secuencial(int nroJugador) {
  seq_mtx.lock(); //A: Espero a mi turno
  logMsg("[secuencial] STEP equipo=%i, nroJugador=%i\n", equipo, nroJugador);
  
  moverse(nroJugador, banderas[contrincante(equipo)]);

  if (++seq_turno == threads.size()) { //A: Soy el último
    logMsg("[secuencial] LAST equipo=%i, nroJugador=%i\n", equipo, nroJugador);

    belcebu->terminoRonda(equipo); //A: Le aviso a belcebu
    seq_turno = 0;
  }

  logMsg("[secuencial] END equipo=%i, nroJugador=%i\n", equipo, nroJugador);
  seq_mtx.unlock(); //A: Dejo jugar a alguien más
}

void Equipo::roundRobin(int nroJugador) {
  while (true) {
    rr_mtx[nroJugador]->lock(); //A: Espero a mi turno //NOTA: Soy el único corriendo durante mi turno
    logMsg("[roundRobin] STEP equipo=%i, nroJugador=%i, quantumLeft=%i\n", equipo, nroJugador, quantumLeft);

    moverse(nroJugador, banderas[contrincante(equipo)]);

    if (--quantumLeft == 0) { //A: Soy el último
      logMsg("[roundRobin] LAST equipo=%i, nroJugador=%i\n", equipo, nroJugador);

      belcebu->terminoRonda(equipo); //A: Le aviso a belcebú
      quantumLeft = quantum; //A: Reinicio el quantumLeft
      rr_mtx[0]->unlock(); //A: Dejo pasar al primero para la siguiente ronda

      break; //A: Salgo
    } else { //A: Le toca al siguiente
      rr_mtx[(nroJugador + 1) % rr_mtx.size()]->unlock(); //A: Dejo pasar al siguiente
      if (quantumLeft < threads.size()) break; //A: No queda suficiente quantum para que me vuelva a tocar
    }
  }

  logMsg("[roundRobin] END equipo=%i, nroJugador=%i\n", equipo, nroJugador);
}

void Equipo::shortestDistanceFirst(int nroJugador) {
  sdf_distances[nroJugador] = posiciones[nroJugador].distance(banderas[contrincante(equipo)]); //A: Calculo mi distancia //NOTA: Como todos sólo modifican su propio valor, no es parte del area crítica
  sdf_mtx.lock();
  if (++sdf_turno == threads.size()) { //A: Soy el último, todos ya calcularon sus distancias
    for (int i = 0; i < threads.size(); i++) sem_post(&sdf_step[0]); //A: Dejo pasar a todos
    sdf_closest = 0; //A: Reinicio el closest
  }
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
  bool juegoEste = nroJugador >= us_range[0]; //A: Estoy por arriba del piso
  juegoEste &= nroJugador != us_range[1]; //A: Y, o estoy por abajo del techo, o el techo loopea lo que igual significa que me toca (ver informe)
  if (juegoEste) { //A: Me toca este turno
    logMsg("[ustedes] IN equipo=%i, nroJugador=%i\n", equipo, nroJugador);
    us_moveMtx.lock(); //A: Sólo se mueve un jugador a la vez
    moverse(nroJugador, banderas[contrincante(equipo)]);
    us_moveMtx.unlock();
  }

  us_mtx.lock();
  if (++us_count == threads.size()) { //A: Soy el último en pasar
      us_range[0] = (us_range[0] + quantum) % threads.size(); //A: Shifteo el rango para les que les toca el siguiente turno
      us_range[1] = (us_range[1] + quantum) % threads.size();
      us_count = 0;
      belcebu->terminoRonda(equipo);
      logMsg("[ustedes] LAST equipo=%i, nroJugador=%i, newRange=(%i, %i)\n", equipo, nroJugador, us_range[0], us_range[1]);
  }
  us_mtx.unlock();
}

//************************************************************
//S: BuscarBandera

void Equipo::buscarBanderas(void) { //U: Busca ambas banderas en el tablero
  sem_wait(&belcebu->semBandera); //A: Espero a mi turno de buscar la bandera
  logMsg("[buscarBanderas] equipo=%i\n", equipo);

  int height, width; belcebu->tableroSize(height, width);
  bool found = false; //U: True cuando se encontró a esa bandera

  std::vector<std::thread> searchThreads(posiciones.size()); //A: Limito los threads al ancho del tablero //TODO: Dividir a lo ancho y a lo alto, para no limitar así

  int block = (width-1) / searchThreads.size() + 1; //U: Ancho del bloque que va revisar cada thread
  for (int i = 0; i < searchThreads.size(); i++) { //A: Creo los threads
    int from = block * i, to = std::min(from + block, width);
    searchThreads[i] = std::thread(
      &Equipo::buscarThread, this,
      height, from, to, std::ref(found)
    );
  }
  for (auto& t : searchThreads) t.join(); //A: Espero a que terminen

  struct Pos pos = banderas[contrincante(equipo)];
  logMsg("[buscarBanderas] equipo=%i, threads=%i, block=%i, pos=(%i, %i)\n", equipo, searchThreads.size(), block, pos.x, pos.y);

  sem_post(&belcebu->semBandera); //A: Dejo que el otro equipo busque la bandera //NOTA: El segundo equipo en buscarla va hacer un post que puede ser ignorado ya que nadie va a esperarlo
}

void Equipo::buscarThread(int height, int from, int to, bool& found) { //U: Cada thread recorre una porción del tablero hasta terminar o que entre todos hayan encontrado ambas banderas
  logMsg("[buscarThread] from=%i, to=%i\n", from, to);

  for (int x = from; x < to && !found; x++) //A: Recorro el tablero
    for (int y = 0; y < height && !found; y++) {
    struct Pos pos({x, y});
    color enPos = belcebu->enPosicion(pos);
    if (enPos == bandera(contrincante(equipo))) { //A: Encontré la bandera contraria
      found = true;
      banderas[contrincante(equipo)] = pos;
    }
  }
  //NOTA: No hay problema con que estemos asignando los valores sin un mtx porque vamos a setear el mismo valor
  //      Aunque sí es posible que haya problemas de sincronización al revisar si ya se encontaron las banderas, pero arreglarlos agrega complejidad y probablemente tarde más que el thread revise una posición extra
}

void Equipo::buscarSecuencial(void) {
  int height, width; belcebu->tableroSize(height, width);
  logMsg("[buscarSecuencial] equipo=%i, width=%i, height=%i\n", equipo, width, height);
  bool found = false; //U: True cuando se encontró a esa bandera

  for (int x = 0; x < width && !found; x++) //A: Recorro el tablero
    for (int y = 0; y < height && !found; y++) {
    struct Pos pos({x, y});
    color enPos = belcebu->enPosicion(pos);
    if (enPos == bandera(contrincante(equipo))) { //A: Encontré la bandera contraria
      found = true;
      banderas[contrincante(equipo)] = pos;
    }
  }

  struct Pos pos = banderas[contrincante(equipo)];
  logMsg("[buscarSecuencial] equipo=%i, pos=(%i, %i)\n", equipo, pos.y, pos.x);
}
