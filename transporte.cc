#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <chrono>
using namespace std;
using namespace chrono;

struct Pedido {
    int salida;
    int llegada;
    int pasajeros;
};

struct Nodo {
    int c;
    int cEstimado;
    int U;
    bool esIzq; // en la teoría dice que hace falta para saber si se ha aceptado o cancelado el último pedido
    Nodo *izq;
    Nodo *der;

    bool operator<(const Nodo& n) const {
        return cEstimado < n.cEstimado;
    }
};

Nodo* crearNodoRaiz() {
    Nodo *nodo = new Nodo;
    nodo->izq = NULL;
    nodo->der = NULL;
    nodo->esIzq = false;
    return nodo;
}

Nodo* crearHijos(Nodo* nodo) {
    Nodo *izquierdo = new Nodo;
    izquierdo->izq = NULL;
    izquierdo->der = NULL;
    izquierdo->esIzq = true;

    Nodo *derecho = new Nodo;
    derecho->izq = NULL;
    derecho->der = NULL;
    derecho->esIzq = false;

    nodo->izq = izquierdo;
    nodo->der = derecho;
}

int comprobarPedidos(int n, int m, int p, vector<Pedido>& pedidos) {
    priority_queue<Nodo*> nodosVivos;
    Nodo* raiz = crearNodoRaiz();
    int U = 0;
    nodosVivos.push(raiz);
    while (!nodosVivos.empty()) {
        Nodo* nodoE = nodosVivos.top();
        nodosVivos.pop();
        crearHijos(nodoE);
    }
}

int main(int argc, char *argv[]) {
    ifstream f(argv[1]);
    if (f.is_open()) {
        ofstream g(argv[2]);
        if (g.is_open()) {
            time_point<high_resolution_clock> start, end;
            int n, m, p;
            int i = 1;
            while (f >> n >> m >> p) {
                if (n == 0 && m == 0 && p == 0) {
                    break;
                }
                if (m > 7) {
                    cerr << "Instancia " << i << ": m debe ser como máximo 7" << endl;
                    continue;
                }
                if (p > 22) {
                    cerr << "Instancia " << i << ": p debe ser como máximo 22" << endl;
                    continue;
                }
                vector<Pedido> pedidos(p);
                for (int j = 0; j < p; i++) {
                    int salida, llegada, pasajeros;
                    f >> salida >> llegada >> pasajeros;
                    if (salida >= m) {
                        cerr << "Instancia " << i << ", pedido " << j << ": la estación de salida "
                             << "debe ser menor que m" << endl;
                        continue;
                    }
                    if (llegada <= salida) {
                        cerr << "Instancia " << i << ", pedido " << j << ": la estación de llegada "
                             << "debe ser posterior a la de salida" << endl;
                        continue;
                    }
                    pedidos[j].salida = salida;
                    pedidos[j].llegada = llegada;
                    pedidos[j].pasajeros = pasajeros;
                }
                start = high_resolution_clock::now();
                // Procesar pedidos
                // g << comprobarPedidos(n, m, p, pedidos) << " "
                end = high_resolution_clock::now();
                auto tiempo = duration_cast<nanoseconds>(end - start).count() / 1E6;
                // g << tiempo << endl;
            }
        }
    }
    return 0;
}
