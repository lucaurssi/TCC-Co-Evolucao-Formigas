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
	int soldiers_amount, home_sick_max;
}A;

void edit_colony(Colony *colony, A ind){

    colony->decay_timer_max = ind.decay; // timer to reduce the pheromones on the map
    colony->decay_timer = colony->decay_timer_max;

    colony->decay_amount = ind.decay_amount; // amount of pheromones reduced per cicle

    colony->alarm_phero_amount = ind.alarm;
    colony->food_phero_amount = ind.food; // types of pheromones and their amount
    colony->path_phero_amount = ind.path;
    
    colony->home_sick_max = ind.sick; // ants go home when zero

	colony->soldiers_amount = ind.soldiers_amount;
	colony->home_sick_max = ind.home_sick_max;

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

	ind.soldiers_amount = rand()%50; // 0-49
	ind.home_sick_max = 51 + rand()%1500;	// 50 - 1500

    return ind;
}

double simulate(Colony* blue, Colony* red, Food* food){
	unsigned char count = 0;
	double t2 = omp_get_wtime();

	while(blue->food_found_amount < 100 && red->food_found_amount < 100){

		if(count == 255){ // every 255 cicles, check if the simulation passed one minute and stop it
			count = 0;	  // this is to prevent an ineficient individual from taking too much time
			if(omp_get_wtime() - t2 > 60) break;          
		}

		process_colony(blue, red->ant_position, food);
		process_colony(red, blue->ant_position, food);
		process_food(food);
		count++;
	}

	return omp_get_wtime() - t2;
}


int main(/*int argc, char** argv*/){

    // ----- setup ----- //
    
    double time;
    //vector<A> vec;
    A ind;
    Food food;

    blue_ants = create_colony(-0.5, -0.5, COLONY_SIZE, 10);
    red_ants = create_colony(0.5, 0.5, COLONY_SIZE, 10);

    create_food_map(&food);
    

    for(int i=0; i<100; i++){
        ind = generate_random_A(); // generate random parameters
        edit_colony(&blue_ants, ind); // apply parameters

        reset_colony(&blue_ants); // reset colonies
        reset_colony(&red_ants);
        
        food.amount = 0;
        process_food(&food);// reset food spot
        
		time = simulate(&blue_ants, &red_ants, &food);

        cout << "I: " << i << " b: " << blue_ants.food_found_amount << " r: " << red_ants.food_found_amount <<" time: " << time << '\n'; 
            
    }

	// TODO : Generations, selection of best, breeding, if best is best: next is genocide, keep best in file

 
    return 0;
}















