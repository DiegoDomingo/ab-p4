/*
* AUTORES:     Diego Domingo Ralla (818637)
*              Simón Alonso Gutiérrez (821038)
* ASIGNATURA:  Algoritmia Básica
* FECHA:       28 mayo 2023
* FICHERO:     transporte-ilp.cc
* DESCRIPCIÓN: Programa que resuelve con un algoritmo de programación lineal
*              enterea (ILP) el problema de cálculo del máximo ingreso en 
*              función de los pedidos aceptados para la línea ferroviaria
*              Taskent-Samarcanda
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <ilcplex/ilocplex.h>
using namespace std;
using namespace chrono;

ILOSTLBEGIN

struct Pedido {
    int salida;
    int llegada;
    int pasajeros;
};

// Resuelve el problema de programación lineal entera (ILP)
// Pre:  pedidos = [p_1, ..., p_p]
//       ∀i ∈ [1, p], p_i.salida >= 0 ∧ p_i.salida < p_i.llegada ∧ 
//       p_i.llegada <= m
// Post: devuelve el máximo ingreso total
int resolverILP(int n, int m, int p, vector<Pedido>& pedidos) {
    IloEnv env;
    try {
        IloModel model(env);

        // Variable del problema: x_i ∈ {0, 1} y representa si se acepta el pedido i o no
        IloBoolVarArray x(env, p);

        // Función objetivo: maximizar el ingreso
        IloExpr obj(env);
        for (int i = 0; i < p; i++) {
            obj += x[i] * pedidos[i].pasajeros * (pedidos[i].llegada - pedidos[i].salida);
        }
        model.add(IloMaximize(env, obj));
        obj.end();

        // Restricciones: para cada estación entre la estación de salida y de llegada de cada
        // pedido, la suma de los pasajeros de los pedidos aceptados debe ser menor o igual que la
        // capacidad del tren
        for (int j = 0; j < m; j++) {
            IloExpr constraint(env);
            for (int i = 0; i < p; i++) {
                if (pedidos[i].salida <= j && j < pedidos[i].llegada) {
                    constraint += x[i] * pedidos[i].pasajeros;
                }
            }
            model.add(constraint <= n);
            constraint.end();
        }

        IloCplex cplex(model);
        cplex.solve();
        int res = cplex.getObjValue();

        env.end();
        return res;
    }
    catch (IloException& e) {
        cerr << "Error: " << e << endl;
        env.end();
        return -1;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Uso: transporte <pruebas.txt> <resultados.txt>" << endl;
        return -1;
    }
    
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
                }
                start = high_resolution_clock::now();
                g << resolverILP(n, m, p, pedidos) << " ";
                end = high_resolution_clock::now();
                auto tiempo = duration_cast<nanoseconds>(end - start).count() / 1E6;
                g << tiempo << endl;
            }
        }
    }
    return 0;
}
