#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <ilcplex/ilocplex.h>
using namespace chrono;

ILOSTLBEGIN

struct Pedido {
    int salida;
    int llegada;
    int pasajeros;
    int beneficio;
};

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
                // Resolver ILP
                end = high_resolution_clock::now();
                auto tiempo = duration_cast<nanoseconds>(end - start).count() / 1E6;
                g << tiempo << endl;
            }
        }
    }
    return 0;
}
