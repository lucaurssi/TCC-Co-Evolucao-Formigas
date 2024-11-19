/*
 TCC - Coevolucao entre duas Especies de Formigas Artificiais Competindo por Recursos
 Author - Luca Gomes Urssi
 Advisor - Eduardo do Valle Simoes
 
 This file simulates two colonies of ants using the ants.h library and evolutionary systems theory without graphics
*/

#include "libs/ants.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <omp.h>

using namespace std;

#define COLONY_SIZE 50	
#define DEBUG 0

Colony blue_ants, red_ants;

// A cadidate is a group of variables that will be tested
typedef struct _cadidate{
    unsigned short int decay, decay_amount, sick;
    unsigned char alarm, food, path;
	int soldiers_amount, home_sick_max;
    int original_generation;
    double time;
    float food_avg;
}Candidate;

// One generation is composed of a group of candidates
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


int simulate(Colony* blue, Colony* red, Food* food, int time_limit){
	int count = 0;

	double t2 = omp_get_wtime();
	
	while(blue->food_found_amount < 100 && red->food_found_amount < 100){

		if(count++ > time_limit) // time_limit is the number of cicles
			break;	  // this is to prevent an ineficient individual from taking too much time
		 
		process_colony(blue, red->ant_position, food);
		process_colony(red, blue->ant_position, food);
		process_food(food);
		
	}
	if(DEBUG) cout << "time: "<<omp_get_wtime() - t2 << "s ";
	return count;
}


// simulate every candidate of the generation ten times each, recording the average time and food collected
void simulate_generation(Colony* blue, Colony* red, Food* food, Gen *color_generation, bool color, int num_per_gen){
	cout << " * Simulating generation * \n";

    for(int i=0; i<num_per_gen; i++){
        double time = 0;	
        int food_avg = 0;
		
        if(color) edit_colony(blue, color_generation->candidates[i]);
        else edit_colony(red, color_generation->candidates[i]); 
		
		cout << "Candidate " << i+1 << "; ";

        for(int j=0; j<10; j++){ // simulate 10x
            reset_colony(blue); // reset colonies
            reset_colony(red);
            food->amount = 0;
            process_food(food);// reset food spot

            time += simulate(blue, red, food, 350000);
            if(color) food_avg += blue->food_found_amount;
           	else food_avg += red->food_found_amount;           		
        }
        color_generation->candidates[i].time = time/10.0;
        color_generation->candidates[i].food_avg = food_avg/10.0;
		
		cout << "Average cicles: " << color_generation->candidates[i].time << " Average food: " << food_avg/10.0 << '\n';
    }
}


/*
	this function generates random parameters and simulates than
	if the time it takes to simulate is lower than "time_limit" cicles
	and the colected amount of food is above 70
	the candidate is added to the vector
*/
void create_generation(Colony* A, Colony* B, Food* food, Gen *color_generation, int current_generation, int num_per_gen, int time_limit){
	int i=1;
	cout << "Generating colonies candidates... \n" ;
	
	while((int)color_generation->candidates.size() < num_per_gen){
		Candidate ind = generate_random(current_generation); // generate random parameters
		edit_colony(A, ind); // apply parameters

		reset_colony(A); // reset colonies
		reset_colony(B);
		
		food->amount = 0;
		process_food(food);// reset food spot
		
		double time = simulate(A, B, food, time_limit); // allow up to -- cicles of simulation

		if(A->food_found_amount > 70) { // requires the colony to gather at least 71 points food to qualify
			color_generation->candidates.push_back(ind);
			cout << "Simulation "<< i << " success. Points: " << A->food_found_amount << " Cicles: " << time << '\n'; 		
		}else
			cout << "Simulation "<< i << " failed.  Points: " << A->food_found_amount << " Cicles: " << time << '\n';
		
		
		i++;
	}

	cout << "Generation created with " << num_per_gen << " candidates \n\n";
}


int find_best(Gen color_generation){

	float best_food = 0;
	double time = 999.0;
	int best = 999;

	for(int i=0; i<(int)color_generation.candidates.size(); i++){
		if(color_generation.candidates[i].food_avg > best_food){ // seek the most food gathered
			best_food = color_generation.candidates[i].food_avg;
			best = i;
			time = color_generation.candidates[i].time;
		}else if(color_generation.candidates[i].food_avg == best_food && color_generation.candidates[i].time < time){
			best = i; // if the same amount of food, look at the time it took to simulate
			time = color_generation.candidates[i].time;
		}
	}
	cout << " Best canditate: " << best+1 << '\n';
	return best;
}

/*
 this function writes on a text file the current best canditate of the generation, 
 the text file is created inside the respective colored folder 
*/
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

void breeding(Gen *gen, int best, int generation){
	cout << "Breeding new generation\n";
	
	// all members breed with the best of the generation
	for(int i=0; i<(int)gen->candidates.size(); i++){
		
		gen->candidates[i].decay = (gen->candidates[i].decay + gen->candidates[best].decay) / 2;
		gen->candidates[i].decay_amount = (gen->candidates[i].decay_amount + gen->candidates[best].decay_amount) / 2;
		gen->candidates[i].sick = (gen->candidates[i].sick + gen->candidates[best].sick) / 2;

		gen->candidates[i].alarm = (gen->candidates[i].alarm + gen->candidates[best].alarm) / 2;
		gen->candidates[i].food = (gen->candidates[i].food + gen->candidates[best].food) / 2;
		gen->candidates[i].path = (gen->candidates[i].path + gen->candidates[best].path) / 2;

		gen->candidates[i].soldiers_amount = (gen->candidates[i].soldiers_amount + gen->candidates[best].soldiers_amount) / 2;
		gen->candidates[i].home_sick_max = (gen->candidates[i].home_sick_max + gen->candidates[best].home_sick_max) / 2;

		gen->candidates[i].original_generation = generation;
	}
	
	int mutation;
	
	// mutation
	for(int i=0; i<(int)gen->candidates.size(); i++){
		if(i == best) continue; // keep best as is
		
		mutation = gen->candidates[i].decay + (rand()%3) -1; // -1,  0, +1
		if(mutation > 0 && mutation < 256)
			gen->candidates[i].decay = mutation;
			
		mutation = gen->candidates[i].decay_amount + (rand()%3) -1; // -1,  0, +1
		if(mutation > 0 && mutation < 50)
			gen->candidates[i].decay_amount = mutation;
			
		mutation = gen->candidates[i].sick + (rand()%11) -5; // -5 ... 0 ... +5
		if(mutation > 100 && mutation < 2000)
			gen->candidates[i].sick = mutation;
			
		mutation = gen->candidates[i].alarm + (rand()%3) -1; // -1,  0, +1
		if(mutation > 5 && mutation < 256)
			gen->candidates[i].alarm = mutation;
			
		mutation = gen->candidates[i].food + (rand()%3) -1; // -1,  0, +1
		if(mutation > 5 && mutation < 256)
			gen->candidates[i].food = mutation;
			
		mutation = gen->candidates[i].path + (rand()%3) -1; // -1,  0, +1
		if(mutation > 5 && mutation < 256)
			gen->candidates[i].path = mutation;
		
		mutation = gen->candidates[i].soldiers_amount + (rand()%3) -1; // -1,  0, +1
		if(mutation > 0 && mutation < COLONY_SIZE)
			gen->candidates[i].soldiers_amount = mutation;
		
		mutation = gen->candidates[i].home_sick_max + (rand()%11) -5; // -5 ... 0 ... +5
		if(mutation > 0 && mutation < 2000)
			gen->candidates[i].home_sick_max = mutation;
	}
    return;
}


int main(/*int argc, char** argv*/){

    // ----- setup ----- //
    
    
    Gen blue_generation, red_generation; // generation of colonies
    Food food;
    int current_generation_number = 1;

    blue_ants = create_colony(-0.5, -0.5, COLONY_SIZE, 10);
    red_ants = create_colony(0.5, 0.5, COLONY_SIZE, 10);

    create_food_map(&food);

    int candidates_per_generation = 10;
	cout << " *Generation 1*\n";
    
    
// ---- 1° blue generation ---- //

	cout << "Blue Setup:\n";	
	
    create_generation(&blue_ants, &red_ants, &food, &blue_generation, current_generation_number, candidates_per_generation, 175000);
    
    simulate_generation(&blue_ants, &red_ants, &food, &blue_generation, true, candidates_per_generation); // true means blue

	int blue_best = find_best(blue_generation);
	edit_colony(&blue_ants, blue_generation.candidates[blue_best]); // set the current best for blue ants
	
	cout << "Best of blue, Cicles: " << blue_generation.candidates[blue_best].time << " Food:" << blue_generation.candidates[blue_best].food_avg << '\n';

	// create a file with this geration best's values
	document_generation_best("blue", blue_generation.candidates[blue_best], current_generation_number);
	
    
// ---- 1° red generation ---- //

	cout << "Red Setup:\n";
	// the red generation goes against the best of the blue
    create_generation(&red_ants, &blue_ants, &food, &red_generation, current_generation_number, candidates_per_generation, 175000);
    
    simulate_generation(&blue_ants, &red_ants, &food, &red_generation, false, candidates_per_generation); // false means red

	int red_best = find_best(red_generation);
	edit_colony(&red_ants, red_generation.candidates[red_best]);

	
	cout << "Best of red, Cicles: " << red_generation.candidates[red_best].time<< " Food:" << red_generation.candidates[red_best].food_avg << '\n';

	document_generation_best("red", red_generation.candidates[red_best], current_generation_number);



 // --- other generations --- //
 
	cout << "\n * 1° generation completed, starting continuous breeding and simulating loop * \n\n";

 	int k=1;
 	while(k < 10){ // k generations
 		k++;
 		current_generation_number++;
 		
		cout << "\n *Generation "<< current_generation_number << "*\n"; 
		cout << "blue:\n";
		
 		breeding(&blue_generation, blue_best, current_generation_number);
 		
 		simulate_generation(&blue_ants, &red_ants, &food, &blue_generation, true, candidates_per_generation); // true means blue
		blue_best = find_best(blue_generation);
		edit_colony(&blue_ants, blue_generation.candidates[blue_best]); 
		document_generation_best("blue", blue_generation.candidates[blue_best], current_generation_number);
	 	
	 	cout << "red:\n";
	 
 		breeding(&red_generation, red_best, current_generation_number);
 		
		simulate_generation(&blue_ants, &red_ants, &food, &red_generation, false, candidates_per_generation); // false means red
		red_best = find_best(red_generation);
		edit_colony(&red_ants, red_generation.candidates[red_best]);
		document_generation_best("red", red_generation.candidates[red_best], current_generation_number);
 	
 	}
 
 
    return 0;
}















