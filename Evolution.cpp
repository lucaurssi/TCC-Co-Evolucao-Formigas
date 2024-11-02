/*
 TCC - Coevolucao entre duas Especies de Formigas Artificiais Competindo por Recursos
 Author - Luca Gomes Urssi
 Advisor - Eduardo do Valle Simoes
*/

#include "libs/ants.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <omp.h>

using namespace std;

#define COLONY_SIZE 50	

Colony blue_ants, red_ants;

typedef struct _cadidate{
    unsigned short int decay, decay_amount, sick;
    unsigned char alarm, food, path;
	int soldiers_amount, home_sick_max;
    int original_generation;
    double time;
}Candidate;

typedef struct _gen{
	vector<Candidate> candidates;
}Gen;

void edit_colony(Colony *colony, Candidate ind){

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

Candidate generate_random(int generation){
    Candidate ind;    
    ind.decay = 10 + rand()%200;        // 1-200
    ind.decay_amount = 1 + rand()%20;   // 1-20
    ind.sick = 450 + rand()%1051;       // 450 - 1500

    ind.alarm = 10 + rand()%236;
    ind.food = 10 + rand()%236; // 10 - 255
    ind.path = 10 + rand()%236;

	ind.soldiers_amount = rand()%50; // 0-49
	ind.home_sick_max = 51 + rand()%1500;	// 50 - 1500

    ind.original_generation = generation;

    return ind;
}

double simulate(Colony* blue, Colony* red, Food* food, int time_limit){
	unsigned char count = 0;
	double t2 = omp_get_wtime();

	while(blue->food_found_amount < 100 && red->food_found_amount < 100){

		if(count == 255){ // every 255 cicles, check if the simulation passed one minute and stop it
			count = 0;	  // this is to prevent an ineficient individual from taking too much time
			if(omp_get_wtime() - t2 > time_limit) break;          
		}

		process_colony(blue, red->ant_position, food);
		process_colony(red, blue->ant_position, food);
		process_food(food);
		count++;
	}

	return omp_get_wtime() - t2;
}

void simulate_generation(Colony* blue, Colony* red, Food* food, Gen *color_generation, bool color, int num_per_gen){
	cout << " * Starting simulation of the current generation. * \n";

    for(int i=0; i<num_per_gen; i++){
        int time = 0;
		
        if(color) edit_colony(blue, color_generation->candidates[i]);
        else edit_colony(red, color_generation->candidates[i]); 
		
		cout << "Simulating " << i+1 << ": ";

        for(int j=0; j<10; j++){ // simulate 10x
            reset_colony(blue); // reset colonies
            reset_colony(red);
            food->amount = 0;
            process_food(food);// reset food spot

            time += simulate(blue, red, food, 120);
        }
        color_generation->candidates[i].time = time/10.0;
		
		cout << "Average time: " << color_generation->candidates[i].time << " Total time: " << time << '\n';
    }
}

/*
	this function generates random parameters and simulates than
	if the time it takes to simulate is lower than 60 seconds
	and the colected amount of food is above 80
	the candidate is added to the vector
*/
void create_generation(Colony* A, Colony* B, Food* food, Gen *color_generation, int current_generation, int num_per_gen){
	int i=1;
	cout << "Generating colonies candidates... \n" ;
	
	while((int)color_generation->candidates.size() < num_per_gen || i > num_per_gen*5){
		Candidate ind = generate_random(current_generation); // generate random parameters
		edit_colony(A, ind); // apply parameters

		reset_colony(A); // reset colonies
		reset_colony(B);
		
		food->amount = 0;
		process_food(food);// reset food spot
		
		double time = simulate(A, B, food, 60); // allow up to 60s of simulation

		if(A->food_found_amount > 80) { // requires the colony to gather at least 81 points food to qualify
			color_generation->candidates.push_back(ind);
			cout << "Simulation "<< i << " success. Points: " << A->food_found_amount << " Time: " << time << '\n'; 		
		}else
			cout << "Simulation "<< i << " failed.  Points: " << A->food_found_amount << " Time: " << time << '\n';
		
		
		i++;
	}

	cout << "Generation created with " << num_per_gen << " candidates \n\n";
}


int find_best(Gen color_generation){

	double best_time = 999;
	int best = 999;

	for(int i=0; i<(int)color_generation.candidates.size(); i++){
		if(color_generation.candidates[i].time < best_time){
			best_time = color_generation.candidates[i].time;
			best = i;
		}
	}

	return best;
}

void document_generation_best(string color, Candidate A, int generation){

    ofstream file;
    string file_name;
    
	file_name = color + "/gen_" + to_string(generation)+".ant";
	file.open(file_name);
	
	file << to_string(A.decay) << ' ' << to_string(A.decay_amount) << ' ' << to_string(A.sick) << ' ';
    file << to_string(A.alarm) << ' ' << to_string(A.food) << ' ' << to_string(A.path) << ' ';
	file << to_string(A.soldiers_amount) << ' ' << to_string(A.home_sick_max) << ' ';
    file << to_string(A.original_generation) << ' ';
    file << to_string(A.time);
	
	file.close();

}

int main(/*int argc, char** argv*/){

    // ----- setup ----- //
    
    Gen blue_generation, red_generation; // generation of colonies
    Food food;
    int current_generation_number = 1;

    blue_ants = create_colony(-0.5, -0.5, COLONY_SIZE, 10);
    red_ants = create_colony(0.5, 0.5, COLONY_SIZE, 10);

    create_food_map(&food);

    int candidates_per_generation = 1;

    
// ---- 1° blue generation ---- //
	cout << "Blue Setup:\n";	
	
    create_generation(&blue_ants, &red_ants, &food, &blue_generation, current_generation_number, candidates_per_generation);
    
    simulate_generation(&blue_ants, &red_ants, &food, &blue_generation, true, candidates_per_generation); // true means blue

	int best = find_best(blue_generation);
	edit_colony(&blue_ants, blue_generation.candidates[best]); // set the current best for blue ants
	
	cout << "Best of blue : " << blue_generation.candidates[best].time << "s\n";

	// create a file with this geration best's values
	document_generation_best("blue", blue_generation.candidates[best], current_generation_number);
	
    
// ---- 1° red generation ---- //
	cout << "Red Setup:\n";
	// the red generation goes against the best of the blue
    create_generation(&red_ants, &blue_ants, &food, &red_generation, current_generation_number, candidates_per_generation);
    
    simulate_generation(&blue_ants, &red_ants, &food, &red_generation, false, candidates_per_generation); // false means red

	best = find_best(red_generation);
	edit_colony(&red_ants, red_generation.candidates[best]);

	
	cout << "Best of red : " << red_generation.candidates[best].time << "s\n";

	document_generation_best("red", red_generation.candidates[best], current_generation_number);

 
 	while(true){
 		current_generation_number++;
 		
 		// blue
 		//breed(blue);
 		
 		simulate_generation(&blue_ants, &red_ants, &food, &blue_generation, true, candidates_per_generation); // true means blue
		best = find_best(blue_generation);
		edit_colony(&blue_ants, blue_generation.candidates[best]); 
		document_generation_best("blue", blue_generation.candidates[best], current_generation_number);
	 	
	 	
	 	//red
	 	//breed(red);
	 	
		simulate_generation(&blue_ants, &red_ants, &food, &red_generation, false, candidates_per_generation); // false means red
		best = find_best(red_generation);
		edit_colony(&red_ants, red_generation.candidates[best]);
		document_generation_best("red", red_generation.candidates[best], current_generation_number);
 	
 	}
 
 
    return 0;
}















