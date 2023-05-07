#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
using namespace std;
using namespace chrono;

class Pedido {
public:
    int salida;
    int llegada;
    int pasajeros;

    Pedido(int s, int l, int p) {
        salida = s;
        llegada = l;
        pasajeros = p;
    }
};

int main(int argc, char* argv[]) {
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
                    if (salida >= llegada) {
                        cerr << "Instancia " << i << ", pedido " << j << ": la estación de salida "
                             << "debe ser anterior a la de llegada" << endl;
                        continue;
                    }
                    if (llegada > m) {
                        cerr << "Instancia " << i << ", pedido " << j << ": la estación de llegada "
                             << "debe ser menor o igual que m" << endl;
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
                auto tiempo = duration_cast<nanoseconds>(end - start).count()/1E6;
                // g << tiempo << endl;
            }
        }
    }
    return 0;
}
