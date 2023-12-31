#include <iostream>
#include <bits/stdc++.h>
#include "utils.h"
#include <math.h>
#include <limits.h>
#include <chrono>
#include <unistd.h>

// alpha = pheromones, beta = heuristic
using namespace std;

int alpha = 4; 
int beta = 3;

vector<string>inst;
int string_len;

double pheromone_factor_acum = 0.0;
double heuristic_factor_acum = 0.0;

double evaporation_rate = 0.758;
int ants = 5;
int max_time = 60;

struct Ant{
    int row;
    int col;
    string solucion;
    int cost;
};

vector<vector<double>> init_pheromones(int string_len) {
    vector<vector<double>> grid(4, vector<double>(string_len, 0.0));

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < string_len; j++) {
            grid[i][j] = 1.0;
        }
    }

    return grid;
}

double calculate_heuristic_next(const Ant ant, int fila) {
    // Crear una copia de la solución actual para evaluar las diferentes opciones
    string sol_adding_letter = ant.solucion;

    // Añadir la letra correspondiente según el valor de j
    switch (fila) {
        case 0:
            sol_adding_letter.push_back('A');
            break;
        case 1:
            sol_adding_letter.push_back('C');
            break;
        case 2:
            sol_adding_letter.push_back('T');
            break;
        case 3:
            sol_adding_letter.push_back('G');
            break;
        default:
            cerr << "Fila invalida" <<endl;
            EXIT_FAILURE;
    }

    int cost = 0;
    int str_len = sol_adding_letter.length();
    for(int i=0;i<inst.size();i++){
        for(int k=0;k<str_len;k++){
            if(inst[i][k] != sol_adding_letter[k])
                cost+=1;
        }
    }

    //Cuanto menor sea el costo, mayor debe ser su probabilidad 
    return 1.0 / (cost*cost);
}

double calculate_choosing_probability(const vector<vector<double>> grid, int i, int j, const Ant ant){
    //parte superior de la ecuacion de probabilidad
    double pheromone_factor = pow (grid[i][j], ::alpha);
    double heuristic_factor = pow (calculate_heuristic_next(ant,i), ::beta);  
    
    double probability = pheromone_factor * heuristic_factor / pheromone_factor_acum * heuristic_factor_acum; 
    return probability;
}


auto compareFunc = [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
    return a.second > b.second;
};


Ant ant_tour(vector<vector<double>> grid) {
    Ant ant;
    const vector<char> bases = {'A', 'C', 'T', 'G'};
    // Inicialización de la hormiga
    ant.row = rand() % 4;
    ant.col = 0;

    ant.solucion.push_back(bases.at(ant.row));
    int motifs_len = inst.at(0).length();

    for (size_t col = 1; col < motifs_len; col++) {
        ant.col = col;
        vector<pair<int,double>>probs;

        // Corregir el bucle y utilizar la variable col en lugar de j
        for (size_t row = 0; row < 4; row++) {
            pheromone_factor_acum += grid[row][col];
            heuristic_factor_acum += calculate_heuristic_next(ant, row);
        }

        pheromone_factor_acum = pow(pheromone_factor_acum, ::alpha);
        heuristic_factor_acum = pow(heuristic_factor_acum, ::beta);

        for (size_t row = 0; row < 4; row++) {
            double probability = calculate_choosing_probability(grid, row, col, ant); 
            probs.push_back (make_pair(row,probability));
        }

        std::sort(probs.begin(),probs.end(),compareFunc);
        pair<int,double>row_selected = probs.at(0); 
        ant.solucion.push_back(bases.at(row_selected.first));
        pheromone_factor_acum = 0.0;
        heuristic_factor_acum = 0.0;
    }
    ant.cost = hamming_cuadrado(ant.solucion,inst);
    return ant;
}

void update_pheromones(vector<vector<double>>& grid, const vector<Ant>ants, double evaporation_rate) {
    unordered_map<char, int> conversion_table = {{'A', 0}, {'C', 1}, {'T', 2}, {'G', 3}};

    // Evaporación de feromonas
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < string_len; j++) {
            grid[i][j] *= (1.0 - evaporation_rate);
        }
    }

    // Actualización de feromonas basada en las soluciones de las hormigas
    for (const Ant& ant : ants) {
        for (int col = 0; col < string_len; col++) {
            
            int row = conversion_table[ant.solucion[col]];
            grid[row][col] += 1.0 / static_cast<double>(ant.cost);
        }
    }
}

pair<int, string> ACO(vector<vector<double>>* grid) {
    pair<int,string>best_ant = make_pair(INT_MAX,"");
    vector<Ant> hormigas; //Necesario para actualizar feromonas al final del tour 
    for (int i = 0; i < ants; i++) {
        Ant ant = ant_tour(*grid); 
        //cout<<"Hormiga "<<i<<endl;
        //cout<<ant.solucion<<endl;
        //cout<<ant.cost<<endl;
        if(ant.cost < best_ant.first)
        {
            best_ant.first = ant.cost;
            best_ant.second = ant.solucion;
        }
        hormigas.push_back(ant);

    }
    update_pheromones(*grid, hormigas, evaporation_rate);
    return best_ant;
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    int m = atoi(argv[1]);
    int l = atoi(argv[2]);

    
    char directorio[10]; // Asegúrate de usar un tamaño adecuado para tu caso
    snprintf(directorio, sizeof(directorio), "%d %d", m, l);

    // Cambiar el directorio de trabajo
    if (chdir(directorio) != 0) {
        perror("chdir");
        return 1;
    }
    std::cout << "inst; " << " m; " << " l; " << " ACO; " << "ACOTIME" << endl;
    
    for (int i=0;i<100;i++){
    string instancia = "inst_"+ to_string(m)+"_"+to_string(l)+"_4_" + to_string(i) + ".txt";
    inst = lee_instancia(instancia);
    int string_len = inst.at(0).length();

    vector<vector<double>> grid = init_pheromones(string_len);
    int best_global = INT_MAX;
    
    double tiempo_greatest = 0;
    double tiempo_now = 0;
    std::chrono::time_point<std::chrono::system_clock> start_time = std::chrono::system_clock::now();
        pair<int,string>best_from_iteration;
        while (true){   
            
            best_from_iteration = ACO(&grid);
            if (best_from_iteration.first < best_global)
            {
                best_global = best_from_iteration.first;
                std::chrono::time_point<std::chrono::system_clock> end_time = std::chrono::system_clock::now();
                std::chrono::duration<double> interval = (end_time - start_time);
                tiempo_greatest = interval.count();

                if(tiempo_greatest >= max_time) break;
                
            }
            std::chrono::time_point<std::chrono::system_clock> end_time = std::chrono::system_clock::now();
            std::chrono::duration<double> interval2 = (end_time - start_time);
            tiempo_now = interval2.count();

            if(tiempo_now >= max_time) break;
        
        }
        printf("%d ;%d ;%d ;%d ;%.2f\n", i, m, l, best_from_iteration.first, tiempo_greatest);

    }

    
    return 0;
}