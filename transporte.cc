/*
* AUTORES:     Diego Domingo Ralla (818637)
*              Simón Alonso Gutiérrez (821038)
* ASIGNATURA:  Algoritmia Básica
* FECHA:       23 mayo 2023
* FICHERO:     transporte.cc
* DESCRIPCIÓN: Programa que resuelve con un algoritmo de ramificación y poda el 
*              problema de cálculo del máximo ingreso en función de los pedidos 
*              aceptados para la línea ferroviaria Taskent-Samarcanda
*/

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

// Nodo del árbol de búsqueda
struct Nodo {
    vector<int> solucion;
    int nivel;
    int costeEstimado;

    bool operator<(const Nodo& n) const {
        return costeEstimado < n.costeEstimado;
    }
};

// Calcula el ingreso obtenido de aceptar los pedidos indicados en el vector
// solucion
// Pre:  pedidos = [p_1, ..., p_p]
//       solucion = [x_1, ..., x_p], ∀i ∈ [1, p] : x_i ∈ {0, 1}
// Post: devuelve el ingreso obtenido
int calcularIngreso(vector<Pedido>& pedidos, vector<int>& solucion) {
    int ingreso = 0.0;
    for (int i = 0; i < pedidos.size(); i++) {
        ingreso += solucion[i] * pedidos[i].beneficio;
    }
    return ingreso;
}

// Calcula la función de estimación c gorro
// Pre:  pedidos = [p_1, ..., p_p]
//       solucion = [x_1, ..., x_p], ∀i ∈ [1, p] : x_i ∈ {0, 1}
//       k es un nivel en el árbol de búsqueda tal que 1 <= k <= p
// Post: devuelve el coste estimado
int calcularCosteEstimado(vector<Pedido>& pedidos, vector<int>& solucion, int k) {
    int coste = 0;
    // Hasta k se acumula el beneficio obtenido por los pedidos aceptados
    for (int i = 0; i < k; i++) {
        coste += solucion[i] * pedidos[i].beneficio;
    }
    // Se suma el beneficio obtenido si se aceptaran todos los pedidos restantes
    for (int i = k; i < pedidos.size(); i++) {
        coste += pedidos[i].beneficio;
    }
    return -coste; // Se devuelve en negativo porque es problema de mínimo
}

// Calcula la función de poda U
// Pre:  pedidos = [p_1, ..., p_p]
//       solucion = [x_1, ..., x_p], ∀i ∈ [1, p] : x_i ∈ {0, 1}
//       k es un nivel en el árbol de búsqueda tal que 1 <= k <= p
// Post: devuelve el valor de la cota superior
int calcularCotaSuperior(vector<Pedido>& pedidos, vector<int>& solucion, int k) {
    int coste = 0;
    // Se acumula el beneficio obtenido por los pedidos aceptados hasta el momento
    for (int i = 0; i < k; i++) {
        coste += solucion[i] * pedidos[i].beneficio;
    }
    return -coste; // Se devuelve en negativo porque es problema de mínimo
}

// Camprueba si un nodo es solución, es decir, si no se sobrepasa la capacidad
// total del tren para ninguna estación
// Pre:  pedidos = [p_1, ..., p_p]
//       solucion = [x_1, ..., x_p], ∀i ∈ [1, p] : x_i ∈ {0, 1}
//       k es un nivel en el árbol de búsqueda tal que 1 <= k <= p
// Post: devuelve true si el nodo es solución
bool esSolucion(int n, vector<Pedido>& pedidos, vector<int>& solucion, int k) {
    // Vector de pasajeros en el tren en cada estación
    vector<int> pasajerosEstacion(pedidos.size(), 0);
    // Se recorren todos los pedidos comprobados hasta el momento. Si el pedido se ha aceptado, se
    // acumula el número de pasajeros en el tren en cada estación por la que pasa el tren en ese
    // pedido. Si en algún momento se sobrepasa la capacidad del tren (n), se devuelve false
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

// Algoritmo de ramificación y poda
// Pre:  pedidos = [p_1, ..., p_p]
//       ∀i ∈ [1, p], p_i.salida >= 0 ∧ p_i.salida < p_i.llegada ∧ 
//       p_i.llegada <= m
// Post: devuelve un vector solución = [x_1, ..., x_p] donde 
//       ∀i ∈ [1, p] : x_i ∈ {0, 1} e indica si el pedido i se acepta o no
Nodo ramPodaMinCoste(int n, int m, int p, vector<Pedido>& pedidos) {
    priority_queue<Nodo> nodosVivos; // Cola con prioridades (se ordena por c gorro)
    Nodo raiz;
    Nodo mejorSolucion; // Nodo con la mejor solución encontrada hasta el momento
    raiz.solucion = vector<int>(p, 0); // Vector solución que indica si cada pedido se acepta o no
    raiz.nivel = 0;
    raiz.costeEstimado = calcularCosteEstimado(pedidos, raiz.solucion, raiz.nivel);
    nodosVivos.push(raiz);
    int U = calcularCotaSuperior(pedidos, raiz.solucion, raiz.nivel);
    while (!nodosVivos.empty()) {
        Nodo nodoE = nodosVivos.top();
        nodosVivos.pop();
        // Si ya se ha llegado al nivel p, hemos llegado a las hojas
        if (nodoE.nivel == p) {
            continue;
        }
        // Se expanden los hijos
        for (int i = 1; i >= 0; i--) {
            Nodo hijo = nodoE;
            hijo.solucion[hijo.nivel++] = i; // En función del hijo expandido se añade 1 o 0
            hijo.costeEstimado = calcularCosteEstimado(pedidos, hijo.solucion, hijo.nivel);
            // Si el nodo es solución factible y no se poda, se añade a la cola
            if (esSolucion(n, pedidos, hijo.solucion, hijo.nivel) && hijo.costeEstimado <= U) {
                nodosVivos.push(hijo);
                // Si c gorro es menor que U, se actualiza U y se guarda el nodo como mejor solución
                // encontrada hasta el momento
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
