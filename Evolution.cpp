/*
 TCC - Coevolucao entre duas Especies de Formigas Artificiais Competindo por Recursos
 Author - Luca Gomes Urssi
 Advisor - Eduardo do Valle Simoes
*/

#include "libs/ants.h"

#include <vector>
#include <iostream>
#include <omp.h>

using namespace std;

#define COLONY_SIZE 50	

Colony blue_ants, red_ants;

typedef struct _a{
    unsigned short int decay, decay_amount, sick;
    unsigned char alarm, food, path;
}A;

void edit_colony(Colony *colony, A ind){

    colony->decay_timer_max = ind.decay; // timer to reduce the pheromones on the map
    colony->decay_timer = colony->decay_timer_max;

    colony->decay_amount = ind.decay_amount; // amount of pheromones reduced per cicle

    colony->alarm_phero_amount = ind.alarm;
    colony->food_phero_amount = ind.food; // types of pheromones and their amount
    colony->path_phero_amount = ind.path;
    
    colony->home_sick_max = ind.sick; // ants go home when zero

    return;
}

A generate_random_A(){
    A ind;    
    ind.decay = 1 + rand()%128;         // 1-128
    ind.decay_amount = 1 + rand()%10;   // 1-10
    ind.sick = 25 + rand()%976;         // 25 - 1000

    ind.alarm = 20 + rand()%236;
    ind.food = 20 + rand()%236; // 20 - 255
    ind.path = 20 + rand()%236;
    return ind;
}


int main(/*int argc, char** argv*/){

    // ----- setup ----- //
    
    double t1, t2;
    unsigned char color[3]={0,0,0};
    vector<A> vec;
    A ind;

    blue_ants = create_colony(-0.5, -0.5, color, COLONY_SIZE);
    red_ants = create_colony(0.5, 0.5, color, COLONY_SIZE);

    create_food_map();

    for(int i=0; i<100; i++){
        ind = generate_random_A();
        edit_colony(&blue_ants, ind);
        reset_colony(&blue_ants);
        reset_colony(&red_ants);

        t2 = omp_get_wtime();
        
        while(blue_ants.food_found_amount < 10 && red_ants.food_found_amount < 10){
            process_colony(&blue_ants, red_ants.ant_position);
            process_colony(&red_ants, blue_ants.ant_position);
            if(omp_get_wtime()-t2 > 2) break;
        }

        t1 = omp_get_wtime() - t2;

        if(t1 < 15 && blue_ants.food_found_amount >= 9){
            cout << "Indv: " << i << " blue: " << blue_ants.food_found_amount << " time: " << t1 << '\n'; 
            vec.push_back(ind);  
        }
         
    }

    for(int i=0; i<(int)vec.size(); i++){
        edit_colony(&blue_ants, vec[i]);
        reset_colony(&blue_ants);
        reset_colony(&red_ants);

        t2 = omp_get_wtime();
        
        while(blue_ants.food_found_amount < 20 && red_ants.food_found_amount < 20){
            process_colony(&blue_ants, red_ants.ant_position);
            process_colony(&red_ants, blue_ants.ant_position);
            if(omp_get_wtime()-t2 > 5) break;
        }

        t1 = omp_get_wtime() - t2;

        if(t1 < 60 && blue_ants.food_found_amount >= 20){
            cout << "Indv: " << i << " blue: " << blue_ants.food_found_amount << " time: " << t1 << '\n'; 
            cout << vec[i].decay << ' ' << vec[i].decay_amount << ' ' << vec[i].sick << ' ' << (int)vec[i].alarm << ' ' << (int)vec[i].food << ' ' << (int)vec[i].path << "\n\n";
            
        }
    }
    
    return 0;
}















