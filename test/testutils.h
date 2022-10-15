#ifndef __TESTUTILS_H__
#define __TESTUTILS_H__

#include <iostream>
#include <fstream>
#include <vector>

#include "utils.h"

void createConfig(int ancho, int alto, int cantJugadores, struct Pos banderas[2], std::vector<struct Pos> posiciones[2]);

#endif // __TESTUTILS_H__
