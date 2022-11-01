#include "gameMaster.h"
#include "equipo.h"
#include "config.h"
#include "utils.h"

#include <ctime>
#include <stdio.h>
#include <iostream>

enum test {
  GAME, SEARCH
};

#define NS_PER_SECOND 1000000000

unsigned long nsBetween(const struct timespec& t0, const struct timespec& t1) {
  return (t1.tv_sec - t0.tv_sec) * NS_PER_SECOND + (t1.tv_nsec - t0.tv_nsec);
}

int testGame(estrategia strat, int quantum) {
  logMsg("[testGame] INIT strat=%i quantum=%i\n", strat, quantum);

  class Config config;
  class GameMaster belcebu(config);

  class Equipo rojo(&belcebu, ROJO, strat, config.cantJugadores, quantum, config.posiciones[ROJO]),
               azul(&belcebu, AZUL, strat, config.cantJugadores, quantum, config.posiciones[AZUL]);

  struct timespec t_start, t_end;
  if (clock_gettime(CLOCK_REALTIME, &t_start) != 0) { perror("[testGame] clock_gettime"); return 1; };
  rojo.comenzar();
  azul.comenzar();
  rojo.terminar();
  azul.terminar();
  if (clock_gettime(CLOCK_REALTIME, &t_end) != 0) { perror("[testGame] clock_gettime"); return 1; };

  printf("[testGame] RESULTS %d %lu\n", belcebu.ganador, nsBetween(t_start, t_end));
  return 0;
}

int testSearch(void) {
  logMsg("[testSearch] INIT\n");

  class Config config;
  class GameMaster belcebu(config);

  estrategia strat = SECUENCIAL;
  int quantum = 10;
  class Equipo rojo(&belcebu, ROJO, strat, config.cantJugadores, quantum, config.posiciones[ROJO]);

  struct timespec t_start, t_seq, t_thread;
  if (clock_gettime(CLOCK_REALTIME, &t_start) != 0) { perror("[testSearch] clock_gettime"); return 1; };
  rojo.buscarSecuencial();
  if (clock_gettime(CLOCK_REALTIME, &t_seq) != 0) { perror("[testSearch] clock_gettime"); return 1; };
  rojo.buscarBanderas();
  if (clock_gettime(CLOCK_REALTIME, &t_thread) != 0) { perror("[testSearch] clock_gettime"); return 1; };

  printf("[testSearch] RESULTS %lu %lu %lu\n", nsBetween(t_start, t_thread), nsBetween(t_start, t_seq), nsBetween(t_seq, t_thread));
  return 0;
}

int main(int argc, char* argv[]) {
  if (argc != 4) { std::cerr << "Invalid arguments" << std::endl ; return -1; }
  enum test test = (enum test)std::atoi(argv[1]); //TODO: Check valid test
  enum estrategia strat = (enum estrategia)std::atoi(argv[2]); //TODO: Check valid strat
  int quantum = std::atoi(argv[3]); //TODO: Check larger than 0

  switch (test) {
    case GAME: return testGame(strat, quantum);
    case SEARCH: return testSearch();
  }
  return 0;
}
