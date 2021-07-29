#include "auxiliares.h"
#include "main.h"

MatrizAdy MG;
MatrizAdy MH;

/****************** heur 1 *****************/

vector<int> heuristicaUno() {

    vector<int> coloreo(MG.size(), 0);
    set<int> conjRep = representantes(MG); // Conjunto con un vértice de cada comp. conexa
    for (int rep : conjRep) {
        heuristicaUnoAux(rep, coloreo);
    }
    return coloreo;
}

void heuristicaUnoAux(int rep, vector<int> &coloreo) {
    int n = MG.size() - 1; // Cantidad de vértices de G
    coloreo[rep] = MG.size(); // Pinto al representante de la comp. conexa
    queue<int> cola;
    cola.push(rep);
    while (!cola.empty()) {
        int v = cola.front();
        cola.pop();
        for (int w : ady(MG, v)) {
            if (!coloreo[w]) {
                int colorPos = colorPosible(w, coloreo);
                if (!(ady(MH, w).empty()) && (colorPos != -1)) {
                    coloreo[w] = colorPos;
                } else {
                    coloreo[w] = n;
                    n -= 1;
                }
                cola.push(w);
            }
        }
    }
}

/***************** heur 2 ******************/

vector<int> heuristicaDos() {
    vector<int> coloreo;
    coloreo.assign(MG.size(), 0); //Inicializo la solución
    MatrizAdy M_resta = restarMatrices();
    set<int> representantesDeComponentes = representantes(M_resta); //Me quedo con un vértice de cada comp. conexa
    int color = 1;
    for (int rep : representantesDeComponentes) { //Pinto cada comp. conexa que quedo luego de hacer (X_{h})-(X_{g})
        pintarComponente(rep, M_resta, color, coloreo);
        color++;
    }
    arreglarColoreo(coloreo, color); //Arreglo el coloreo
    return coloreo;
}

/***************** Tabu Search ******************/

vector<int> tabuSearch(bool flagMemoria, bool flagHeuristica, int iterMax) {
    int sizeMaximo;
    if (flagMemoria) { // Memoria basada en ultimas soluciones exploradas.
        sizeMaximo = MG.size(); //BANANA
    } else { // Memoria basada en estructura (vertices)
        sizeMaximo = MG.size() / 2;
    }

    vector<int> coloreoMax;
    if (flagHeuristica) { //Inicializamos la solucion inicial con H1
        coloreoMax = heuristicaUno();
    } else { //Inicializamos la solucion inicial con H2
        coloreoMax = heuristicaDos();
    }
    int impactoMax = impacto(coloreoMax);

    tuple<int, vector<int>> solucionActual = make_tuple(-1, coloreoMax);
    int cantIt = 0;
    queue<tuple<int, vector<int>>> listaTabu;
    int impactoActual;

    while (cantIt < iterMax) {
        solucionActual = crearVecindad(solucionActual, listaTabu, flagMemoria, impactoActual);
        listaTabu.push(solucionActual);
        if (impactoActual > impactoMax) {
            impactoMax = impactoActual;
            coloreoMax = get<1>(solucionActual);
        }
        if (listaTabu.size() == sizeMaximo) {
            listaTabu.pop();
        }
        cantIt++;
    }
    return coloreoMax;
}

/* main */

int main(int argc, char **argv) {

    int n, m_G, m_H;
    vector<int> coloreo;
    int imp;

    // Leemos el parametro que indica el algoritmo a ejecutar.
    map<string, string> algoritmos_implementados = {
            {"H1",        "Heuristica 1"},
            {"H2",        "Heuristica 2"},
            {"TB-H1-MSE-MBI", "Tabu Search Heuristica 1 Memoria SE Muy Bajas Iteraciones"},
            {"TB-H1-ME-MBI", "Tabu Search Heuristica 1 Memoria Estruc Muy Bajas Iteraciones"},
            {"TB-H2-MSE-MBI", "Tabu Search Heuristica 2 Memoria SE Muy Bajas Iteraciones"},
            {"TB-H2-ME-MBI", "Tabu Search Heuristica 2 Memoria Estruc Muy Bajas Iteraciones"},
            {"TB-H1-MSE-BI", "Tabu Search Heuristica 1 Memoria SE Bajas Iteraciones"},
            {"TB-H1-ME-BI",  "Tabu Search Heuristica 1 Memoria Estruc Bajas Iteraciones"},
            {"TB-H2-MSE-BI", "Tabu Search Heuristica 2 Memoria SE Bajas Iteraciones"},
            {"TB-H2-ME-BI",  "Tabu Search Heuristica 2 Memoria Estruc Bajas Iteraciones"},
            {"TB-H1-MSE-MI", "Tabu Search Heuristica 1 Memoria SE Medias Iteraciones"},
            {"TB-H1-ME-MI",  "Tabu Search Heuristica 1 Memoria Estruc Medias Iteraciones"},
            {"TB-H2-MSE-MI", "Tabu Search Heuristica 2 Memoria SE Medias Iteraciones"},
            {"TB-H2-ME-MI",  "Tabu Search Heuristica 2 Memoria Estruc Medias Iteraciones"},
            {"TB-H1-MSE-AI", "Tabu Search Heuristica 1 Memoria SE Altas Iteraciones"},
            {"TB-H1-ME-AI",  "Tabu Search Heuristica 1 Memoria Estruc Altas Iteraciones"},
            {"TB-H2-MSE-AI", "Tabu Search Heuristica 2 Memoria SE Altas Iteraciones"},
            {"TB-H2-ME-AI",  "Tabu Search Heuristica 2 Memoria Estruc Altas Iteraciones"},
    };

    // Verificar que el algoritmo pedido exista.
    if (argc < 2 or algoritmos_implementados.find(argv[1]) == algoritmos_implementados.end()) {
        cerr << "Algoritmo no encontrado: " << argv[1] << endl;
        cerr << "Los algoritmos existentes son: " << endl;
        for (auto &alg_desc: algoritmos_implementados)
            cerr << "\t- " << alg_desc.first << ": " << alg_desc.second << endl;
        return 0;
    }
    string algoritmo = argv[1];

    // Leemos el input.
    cin >> n >> m_G >> m_H;
    vector<int> aux(n, 0);
    MG.assign(n, aux);
    MH.assign(n, aux);

    int v;
    int w;
    for (int i = 0; i < m_G; i++) {
        cin >> v;        // Primer coordenada de la arista
        cin >> w;        // Segunda coordenada de la arista
        MG[v - 1][w - 1] = 1;
        MG[w - 1][v - 1] = 1;
    }
    for (int i = 0; i < m_H; i++) {
        cin >> v;        // Primer coordenada de la arista
        cin >> w;        // Segunda coordenada de la arista
        MH[v - 1][w - 1] = 1;
        MH[w - 1][v - 1] = 1;
    }

    // Ejecutamos el algoritmo y obtenemos su tiempo de ejecución.
    auto start = chrono::steady_clock::now();
    if (algoritmo == "H1") {
        coloreo = heuristicaUno();

    } else if (algoritmo == "H2") {
        coloreo = heuristicaDos();

    } else if (algoritmo == "TB-H1-MSE-MBI") {
        coloreo = tabuSearch(true, true, 50);

    } else if (algoritmo == "TB-H1-ME-MBI") {
        coloreo = tabuSearch(false, true, 50);

    } else if (algoritmo == "TB-H2-MSE-MBI") {
        coloreo = tabuSearch(true, false, 50);

    } else if (algoritmo == "TB-H2-ME-MBI") {
        coloreo = tabuSearch(false, false, 50);
    } else if (algoritmo == "TB-H1-MSE-BI") {
        coloreo = tabuSearch(true, true, 500);

    } else if (algoritmo == "TB-H1-ME-BI") {
        coloreo = tabuSearch(false, true, 500);

    } else if (algoritmo == "TB-H2-MSE-BI") {
        coloreo = tabuSearch(true, false, 500);

    } else if (algoritmo == "TB-H2-ME-BI") {
        coloreo = tabuSearch(false, false, 500);
    } else if (algoritmo == "TB-H1-MSE-MI") {
        coloreo = tabuSearch(true, true, 1000);

    } else if (algoritmo == "TB-H1-ME-MI") {
        coloreo = tabuSearch(false, true, 1000);

    } else if (algoritmo == "TB-H2-MSE-MI") {
        coloreo = tabuSearch(true, false, 1000);

    } else if (algoritmo == "TB-H2-ME-MI") {
        coloreo = tabuSearch(false, false, 1000);
    } else if (algoritmo == "TB-H1-MSE-AI") {
        coloreo = tabuSearch(true, true, 2500);

    } else if (algoritmo == "TB-H1-ME-AI") {
        coloreo = tabuSearch(false, true, 2500);

    } else if (algoritmo == "TB-H2-MSE-AI") {
        coloreo = tabuSearch(true, false, 2500);

    } else if (algoritmo == "TB-H2-ME-AI") {
        coloreo = tabuSearch(false, false, 2500);
    }

    auto end = chrono::steady_clock::now();
    double total_time = chrono::duration<double, milli>(end - start).count();

    // Imprimimos el tiempo de ejecución por stderr.
    clog << total_time << endl;

    // Imprimimos el resultado por stdout.
    imp = impacto(coloreo);
    cout << imp << endl;
    for (int color: coloreo) {
        cout << color << "\t";
    }
    return 0;
}