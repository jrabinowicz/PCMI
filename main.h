#ifndef CODIGO_MAIN_H
#define CODIGO_MAIN_H

#include "auxiliares.h"

vector<int> heuristicaUno();

void heuristicaUnoAux(int rep, vector<int> &coloreo);

vector<int> heuristicaDos();

vector<int> tabuSearch(bool flagMemoria, bool flagHeuristica, int iterMax);

#endif //CODIGO_MAIN_H