//
// Created by lucho on 13/11/2020.
//

#include "auxiliares.h"

extern MatrizAdy MG;
extern MatrizAdy MH;

/*
Cambia de color los vértices que generan adyancencias problemáticas en G, uno
 por uno.
*/
void arreglarColoreo(vector<int> &coloreo, int color) {
    while (adyMaximo(coloreo) != -1) {
        coloreo[adyMaximo(coloreo)] = color;
        color++;
    }
}

/*
Devuelve el vértice v que tenga más vértices adyacentes pintados del mismo color.
Si no hay adyancencias malas, entonces devuelve -1.
*/
int adyMaximo(vector<int> coloreo) {
    int verticeMax = -1; //Inicializo la respuesta con un valor fuera de rango
    int adyacenciasMalas = 0;
    for (int vert = 0; vert < MG.size(); ++vert) {
        int adyacenciasMalasAux = 0;
        set<int> adyacentes = ady(MG, vert);
        for (int ady : adyacentes) {
            if (coloreo[vert] == coloreo[ady]) {
                adyacenciasMalasAux++;
            }
        }
        if (adyacenciasMalas < adyacenciasMalasAux) {
            verticeMax = vert;
            adyacenciasMalas = adyacenciasMalasAux;
        }
    }
    return verticeMax;
}

/*
Realiza la resta de matrices de adyacencia. Si en alguna coordenada el resultado es -1, se lo
cambio por un cero.
*/
MatrizAdy restarMatrices() {
    vector<vector<int> > res(MH.size(), vector<int>(MH.size()));
    for (int i = 0; i < MH.size(); ++i) {
        for (int j = 0; j < MH.size(); ++j) {
            res[i][j] = MH[i][j] - MG[i][j];
            if (res[i][j] < 0)
                res[i][j] = 0;
        }
    }
    return res;
}

/*
Practicamente, es BFS. La única diferencia es que, en vez de guardar los visitados, los pintamos
*/
void pintarComponente(int vert, MatrizAdy const &Matriz, int color, vector<int> &coloreo) {
    queue<int> Q;
    coloreo[vert] = color;
    Q.push(vert);
    while (!(Q.empty())) {
        int w = Q.front();
        Q.pop();
        for (int ad : ady(Matriz, w)) {
            if (coloreo[ad] == 0) {
                coloreo[ad] = color;
                Q.push(ad);
            }
        }
    }
}

/*
Se recorre el grafo con BFS. Para cada vértice, si no fue visitado, se agrega a ConjRep, se marca como visitado
 y se marcan todos los vértices de su componente conexa como visitados. Así, queda un vértice de cada comp.conexa
*/
set<int> representantes(MatrizAdy const &Matriz) { // Conjunto de vértices, uno de cada comp. conexa
    set<int> ConjRep;
    vector<bool> visitados(Matriz.size(), false);
    queue<int> cola;
    for (int i = 0; i < Matriz.size(); i++) {
        if (!visitados[i]) {
            cola.push(i);
            visitados[i] = true;
            ConjRep.insert(i);
            while (!cola.empty()) {
                int v = cola.front();
                cola.pop();
                for (int w : ady(Matriz, v)) {
                    if (!visitados[w]) {
                        visitados[w] = true;
                        cola.push(w);
                    }
                }
            }
        }
    }
    return ConjRep;
}

set<int> ady(MatrizAdy Matriz, int vertice) { // Devuelve el conjunto de vértices ady a "vertice"
    set<int> Conjunto = {};
    for (int i = 0; i < Matriz.size(); i++) {
        if (Matriz[vertice][i] == 1) {
            Conjunto.insert(i);
        }
    }
    return Conjunto;
}

/*
Si hay un vértice que sea ady a vertex en H, que no lo es en G y que este pintado en G,
se pinta del mismo color a vertex.
*/
int colorPosible(int vertex, vector<int> const &coloreo) {
    int color = -1;
    set<int> adyVertex = ady(MG, vertex);
    for (int i = 0; i < MG.size(); i++) {
        if (MG[vertex][i] == 0 && MH[vertex][i] == 1 && coloreo[i] != 0) {
            bool flagAux = true;
            for (int ady: adyVertex) {
                if (coloreo[i] == coloreo[ady]) {
                    flagAux = false;
                }
            }
            if (flagAux) {
                color = coloreo[i];
                break;
            }
        }
    }
    return color;
}

/*
Calcula el impacto de un coloreo
*/
int impacto(vector<int> coloreo) {
    int impacto = 0;
    for (int i = 0; i < MH.size(); ++i) {
        for (int j = 0; j < i; ++j) {
            if (MH[i][j]) {
                impacto += (coloreo[i] == coloreo[j]);
            }
        }
    }
    return impacto;
}

/*
Crea la vecindad, y a medida que la crea elige la instancia válida de mayor impacto.
*/
tuple<int, vector<int>> crearVecindad(tuple<int, vector<int>> solucionActual,
                                      queue<tuple<int, vector<int>>> &listaTabu, bool flagMemoria, int &impactoActual) {

    tuple<int, vector<int>> mejor;
    int impactoMejor = 0;
    vector<int> coloreoAux = get<1>(solucionActual);
    for (int k = 0; k < MG.size() * 3; ++k) { // Cambiamos el color de N*5 vértices. BANANA
        int i = rand() % int(MG.size()); //random j uniforme entre 1 y n
        int j = rand() % int(MG.size()); //random i uniforme entre 1 y n
        coloreoAux[i] = j;
        tuple<int, vector<int>> aux = make_tuple(i, coloreoAux);
        int impactoAux = impacto(coloreoAux);
        if (valida(aux, listaTabu, flagMemoria) && impactoAux > impactoMejor) {
            mejor = aux;
            impactoMejor = impactoAux;
        }
        coloreoAux = get<1>(solucionActual);
    }
    impactoActual = impactoMejor; // "Devuelvo el impacto de la mejor instancia por referencia
    return mejor;
}

/*
Checkeamos que la solución aux sea válida y no pertenezca a la lista tabu.
*/
bool valida(tuple<int, vector<int>> const &aux, queue<tuple<int, vector<int>>> &listaTabu, bool flagMemoria) {
    bool res = true;

    set<int> adyacentes = ady(MG, get<0>(aux));
    for (int ady: adyacentes) { //Checkeamos que el coloreo sea valido
        if (get<1>(aux)[ady] == get<1>(aux)[get<0>(aux)]) {
            return false;
        }
    }

    if (flagMemoria) { // Memoria basada en ultimas soluciones exploradas.
        for (int i = 0; i < listaTabu.size(); ++i) {
            tuple<int, vector<int>> tabu = listaTabu.front();
            listaTabu.pop();
            if (get<1>(tabu) == get<1>(aux)) {
                return false;
            }
            listaTabu.push(tabu);
        }
    } else { // Memoria basada en estructura (vertices)
        for (int i = 0; i < listaTabu.size(); ++i) {
            tuple<int, vector<int>> tabu = listaTabu.front();
            listaTabu.pop();
            if (get<0>(tabu) == get<0>(aux)) {
                return false;
            }
            listaTabu.push(tabu);
        }
    }
    return res;
}