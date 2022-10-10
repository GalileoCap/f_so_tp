#include "gameMaster.h"
#include "config.h"

#include <iostream>

const estrategia strat = SECUENCIAL;
const int quantum = 10;

int main(void) {
  class Config config;
  class GameMaster belcebu(strat, quantum, config);

  belcebu.comenzar();
  int ganador = belcebu.terminar();

  std::cout << "Bandera capturada por el equipo " << ganador << ". Felicidades!" << std::endl;
  return 0;
}

