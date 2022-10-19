#include "gameMaster.h"
#include "equipo.h"
#include "config.h"
#include "utils.h"

#include <iostream>

const estrategia strat = USTEDES;
const int quantum = 1;

int main(void) {
  logMsg("MAIN strat=%i quantum=%i\n", strat, quantum);

  class Config config;
  class GameMaster belcebu(config);

  class Equipo rojo(&belcebu, ROJO, strat, config.cantJugadores, quantum, config.posiciones[ROJO]),
               azul(&belcebu, AZUL, strat, config.cantJugadores, quantum, config.posiciones[AZUL]);

  rojo.comenzar();
  azul.comenzar();
  rojo.terminar();
  azul.terminar();

  std::cout << "Bandera capturada por el equipo " << belcebu.ganador << ". Felicidades!" << std::endl;
  return 0;
}

