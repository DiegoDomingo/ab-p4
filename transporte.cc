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
    int beneficio;
};

struct Nodo {
    vector<int> solucion;
    int nivel;
    int costeEstimado;

    bool operator<(const Nodo& n) const {
        return costeEstimado < n.costeEstimado;
    }
};

int calcularIngreso(vector<Pedido>& pedidos, vector<int>& solucion) {
    int ingreso = 0.0;
    for (int i = 0; i < pedidos.size(); i++) {
        ingreso += solucion[i] * pedidos[i].beneficio;
    }
    return ingreso;
}

int calcularCosteEstimado(vector<Pedido>& pedidos, vector<int>& solucion, int k) {
    int coste = 0;
    for (int i = 0; i < k; i++) {
        coste += solucion[i] * pedidos[i].beneficio;
    }
    for (int i = k; i < pedidos.size(); i++) {
        coste += pedidos[i].beneficio;
    }
    return -coste;
}

int calcularCotaSuperior(vector<Pedido>& pedidos, vector<int>& solucion, int k) {
    int coste = 0;
    for (int i = 0; i < k; i++) {
        coste += solucion[i] * pedidos[i].beneficio;
    }
    return -coste;
}

bool esSolucion(int n, vector<Pedido>& pedidos, vector<int>& solucion, int k) {
    vector<int> pasajerosEstacion(pedidos.size(), 0);
    for (int i = 0; i < k; i++) {
        if (solucion[i] == 1) {
            for (int j = pedidos[i].salida; j < pedidos[i].llegada; j++) {
                pasajerosEstacion[j] += pedidos[i].pasajeros;
                if (pasajerosEstacion[j] > n) {
                    return false;
                }
            }
        }
    }
    return true;
}

Nodo ramPodaMinCoste(int n, int m, int p, vector<Pedido>& pedidos) {
    priority_queue<Nodo> nodosVivos;
    Nodo raiz;
    Nodo mejorSolucion;
    raiz.solucion = vector<int>(p, 0);
    raiz.nivel = 0;
    raiz.costeEstimado = calcularCosteEstimado(pedidos, raiz.solucion, raiz.nivel);
    nodosVivos.push(raiz);
    int U = calcularCotaSuperior(pedidos, raiz.solucion, raiz.nivel);
    while (!nodosVivos.empty()) {
        Nodo nodoE = nodosVivos.top();
        nodosVivos.pop();
        if (nodoE.nivel == p) {
            continue;
        }
        for (int i = 1; i >= 0; i--) {
            Nodo hijo = nodoE;
            hijo.solucion[hijo.nivel++] = i;
            hijo.costeEstimado = calcularCosteEstimado(pedidos, hijo.solucion, hijo.nivel);
            if (esSolucion(n, pedidos, hijo.solucion, hijo.nivel) && hijo.costeEstimado <= U) {
                nodosVivos.push(hijo);
                if (hijo.costeEstimado < U) {
                    U = calcularCotaSuperior(pedidos, hijo.solucion, hijo.nivel);
                    mejorSolucion = hijo;
                }
            }
        }
    }
    return mejorSolucion;
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
                for (int j = 0; j < p; j++) {
                    int salida, llegada, pasajeros;
                    f >> salida >> llegada >> pasajeros;
                    if (salida >= m) {
                        cerr << "Instancia " << i << ", pedido " << j + 1 << ": la estación de "
                             << "salida debe ser menor que m" << endl;
                        continue;
                    }
                    if (llegada <= salida) {
                        cerr << "Instancia " << i << ", pedido " << j + 1 << ": la estación de "
                             << "llegada debe ser posterior a la de salida" << endl;
                        continue;
                    }
                    pedidos[j].salida = salida;
                    pedidos[j].llegada = llegada;
                    pedidos[j].pasajeros = pasajeros;
                    pedidos[j].beneficio = pasajeros * (llegada - salida);
                }
                start = high_resolution_clock::now();
                // Procesar pedidos
                Nodo nodo = ramPodaMinCoste(n, m, p, pedidos);
                g << calcularIngreso(pedidos, nodo.solucion) << " ";
                end = high_resolution_clock::now();
                auto tiempo = duration_cast<nanoseconds>(end - start).count() / 1E6;
                g << tiempo << endl;
            }
        }
    }
    return 0;
}
