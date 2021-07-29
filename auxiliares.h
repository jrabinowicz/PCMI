//
// Created by lucho on 13/11/2020.
//

#ifndef CODIGO_AUXILIARES_H
#define CODIGO_AUXILIARES_H

#include <iostream>
#include <vector>
#include <set>
#include <queue>
#include <chrono>
#include <map>
#include <algorithm>
#include <tuple>
#include <random>

using namespace std;

typedef vector<vector<int>> MatrizAdy;

MatrizAdy restarMatrices();

void pintarComponente(int vert, MatrizAdy const &Matriz, int color, vector<int> &coloreo);

void arreglarColoreo(vector<int> &coloreo, int color);

int adyMaximo(vector<int> coloreo);

set<int> representantes(MatrizAdy const &Matriz);

set<int> ady(MatrizAdy Matriz, int vertice);

int colorPosible(int vertex, vector<int> const &coloreo);

int impacto(vector<int> coloreo);

tuple<int, vector<int>> crearVecindad(tuple<int, vector<int>> solucionActual,
                                      queue<tuple<int, vector<int>>> &listaTabu, bool flagMemoria, int &impactoActual);

bool valida(tuple<int, vector<int>> const &aux, queue<tuple<int, vector<int>>> &listaTabu, bool flagMemoria);

#endif //CODIGO_AUXILIARES_H