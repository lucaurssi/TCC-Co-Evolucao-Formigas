/*
 TCC - Coevolucao entre duas Especies de Formigas Artificiais Competindo por Recursos
 Author - Luca Gomes Urssi
 Advisor - Eduardo do Valle Simoes
*/

#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif
#include "drawings.h"
#include "ants.h"

#include <math.h>
#include <vector>
#include <omp.h>

bool food_map[900][900];
    
Ant create_ant(float x, float y, unsigned char *color, bool species, int home_sick){
    Ant A;  

    A.radius = 0.01;
    A.x = x;
    A.y = y;
    A.theta = ((rand()%628)-314)/100.0; // moving direction
    A.initial_theta = A.theta;
    A.r = color[0] ;
    A.g = color[1] + 150; // change in color that make it easier to see the ants on the nest
    A.b = color[2];
    A.species = species;
    A.found_food = false;
    A.intruder_detected = false;
    A.home_sick = home_sick;
    A.lost =0;

    return A; 
}

void draw_ant(Ant ant){	

	unsigned char color[3]; 
	setColor(color, ant.r, ant.g, ant.b);

	float radius = ant.radius;
	float x = ant.x;
	float y = ant.y;

	triangle(x, y, radius*2, radius*2, color);	
	return;
}

// -1 - 1 to 0-900
int convert_range2(float x){
    int output = (x+1)*450; 
    if(output > 899) output=899;
    else if(output < 0) output=0;

    return output;
}

void move_ant(Ant *ant, float distance, bool ant_pos[900][900]){
    int x,y;	
    
    // this keeps theta value between -3.14 and 3.14, which is the used directions in our functions
    if(ant->theta > 3.1415) ant->theta = ant->theta - 6.283;
    if(ant->theta < -3.1415) ant->theta = ant->theta + 6.283;
    
    // remove ant from previous location
    x = convert_range2(ant->x);
    y = convert_range2(ant->y);
    ant_pos[x][y] = false; // location for ant vision
    
	// move towards theta
	ant->x = ant->x + distance*cos(ant->theta);
	ant->y = ant->y + distance*sin(ant->theta);

    // add ant to new location
    x = convert_range2(ant->x);
    y = convert_range2(ant->y);
    ant_pos[x][y] = true; 

	// end of screen
    if(ant->x > 1){ // top wall /\        /
        ant->x = 1;
        if(ant->theta >0) ant->theta = ant->theta + 1.57;
        else if(ant->theta <0) ant->theta = ant->theta - 1.57;
        else ant->theta = 3.14;
    
    }else if(ant->x < -1){ // bottom wall \/
        ant->x = -1;
        if(ant->theta >0) ant->theta = ant->theta - 1.57;
        else if(ant->theta <0) ant->theta = ant->theta + 1.57;
        else ant->theta = 0;
    }
	if(ant->y > 1){ // right wall  -->
        ant->y = 1;
        ant->theta = -ant->theta;
    
    }else if(ant->y < -1){ // left wall <--
        ant->y = -1;
        ant->theta = -ant->theta;
    }
    
   
    
}


void update_pheromones(Colony * colony){
    int nest_x = convert_range2(colony->nest_x),
        nest_y = convert_range2(colony->nest_y);

    if(colony->pheromones[nest_x][nest_y][0] < 225){ // colony is always full phero
        for(int i=nest_x-33; i<nest_x+33; i++)
            for(int j=nest_y-33; j<nest_y+33; j++)
                colony->pheromones[i][j][0] = 255;  
        
        for(int i=290; i<310; i++) // food spot phero
            for(int j=440; j<460; j++)
                colony->pheromones[i][j][1] = 255;

        for(int i=440; i<460; i++)
            for(int j=440; j<460; j++)
                colony->pheromones[i][j][1] = 255;

        for(int i=590; i<610; i++)
            for(int j=440; j<460; j++)
                colony->pheromones[i][j][1] = 255;
    }              


    colony->decay_timer -= 1;
    if(colony->decay_timer > 0) return; // not ready to decay
    
    colony->decay_timer = colony->decay_timer_max;
    
    #pragma omp parallel for
    for(int i=0; i<900; i++)
        for(int j=0; j<900; j++)
            for(int k=0; k<3; k++)
                if(colony->pheromones[i][j][k] > 0) colony->pheromones[i][j][k] -= colony->decay_amount;
}

Colony create_colony(float x, float y, unsigned char*color, bool species, int amount){
    Colony swarm;
    Ant A;    
    swarm.home_sick_max = 1000;

    for(int i=0; i<amount; i++){ // creates the ants of the colony 
        A = create_ant(x, y, color, species, swarm.home_sick_max);
        swarm.ants.push_back(A); 
    }
    
    #pragma omp parallel for
    for(int i=0; i<900; i++)
        for(int j=0; j<900; j++){
            swarm.ant_position[i][j] = false; // map is empty of ants
            for(int k=0; k<3; k++){
                swarm.pheromones[i][j][k]=0; // map is clear of pheromones
            }
        }
    
    swarm.draw_phero = false; // this option is off by default
    swarm.decay_timer_max = 100; // timer to reduce the pheromones on the map
    swarm.decay_timer = swarm.decay_timer_max;
    swarm.decay_amount = 1; // amount of pheromones reduced per cicle

    swarm.nest_x=x;
    swarm.nest_y=y;
    swarm.ants_amount = amount;

    return swarm;
}

void reset_colony(Colony *colony){
    for(int i=0; i<colony->ants_amount; i++){
        colony->ants[i].theta = colony->ants[i].initial_theta;
        colony->ants[i].x = colony->nest_x;
        colony->ants[i].y = colony->nest_y;
        colony->ants[i].found_food = false;
        colony->ants[i].intruder_detected = false;
    }

    #pragma omp parallel for
    for(int i=0; i<900; i++)
        for(int j=0; j<900; j++){
            colony->ant_position[i][j] = false; // map is empty of ants
            for(int k=0; k<3; k++){
                colony->pheromones[i][j][k]=0; // map is clear of pheromones
            }
        }
    return;
}


bool search_for_enemies(int x, int y, bool enemy_location[900][900]){

    if(x+2 < 900 && x-2>0 && y+2 < 900 && y-2>0) // not near borders
        for(int i=x-2; i<x+3; i++) 
            for(int j=y-2; j<y+3; j++) // check for enemy ant 2 pixels away
                if(enemy_location[i][j])
                    return true;
    return false;
}

void food_pocket(int x, int y){
    for(int i=x-10; i<x+10; i++)
        for(int j=y-10; j<y+10; j++)
            food_map[i][j] = true;
}

void create_food_map(){
    #pragma omp parallel for
    for(int i=0; i<900; i++) 
        for(int j=0; j<900; j++) 
            food_map[i][j] = false;
    
    food_pocket(450, 450);
    food_pocket(300, 450);
    food_pocket(600, 450);
}

bool check_food_nearby(int x, int y){

    if(x+2 < 900 && x-2>0 && y+2 < 900 && y-2>0) // not near borders
        for(int i=x-2; i<x+3; i++) 
            for(int j=y-2; j<y+3; j++) // check for food 2 pixels away
                if(food_map[i][j])
                    return true;
    return false;
}


// this function changes the direction of the ant based on it's behaviour
bool follow_pheromone(int x, int y, float *theta, unsigned char pheromones[900][900][3], int type){
 
// pheromone 'type': 0 - path   1 - food   2 - alarm

/*
    #ants have two sensors, they turn to the direction with more pheromone

    X - ant
    A - left sensor
    B - right sensor

    #theta is the direction the ant is facing
    
                        1,9625-1,1775
2.7475-1.9625       A A A . B B B  
             B B B  A A A . B B B  A A A
           . B B B    A A . B B    A A A .  1,1775-0.3925
         A A . B          X          A . B B
         A A A X                     X B B B
         A A                             B B
      
         B B                            A A
       B B B                          A A A
       B B B                          A A A
       . . . X                      X . . . 0.3925-(-0,3925)
       A A A                          B B B
       A A A                          B B B
       A A                              B B
        (-2.7475)-2.7475
                            ...
*/    

    int sumA=0, sumB=0; // sum of pheromones in it's path
    
    if(!(x+3 < 900 && x-3>0 && y+3 < 900 && y-3>0)) return false; // he do be blind near border.
    
    // -->
    if(*theta >= -0.3925 && *theta <= 0.3925 ){ 
        sumA =                              pheromones[x+2][y-3][type] + pheromones[x+3][y-3][type] +
               pheromones[x+1][y-2][type] + pheromones[x+2][y-2][type] + pheromones[x+3][y-2][type] +
               pheromones[x+1][y-1][type] + pheromones[x+2][y-1][type] + pheromones[x+3][y-1][type] ;
        
        sumB = pheromones[x+1][y+1][type] + pheromones[x+2][y+1][type] + pheromones[x+3][y+1][type] +
               pheromones[x+1][y+2][type] + pheromones[x+2][y+2][type] + pheromones[x+3][y+2][type] +
                                            pheromones[x+2][y+3][type] + pheromones[x+3][y+3][type] ;
    // <--
    }else if(*theta <= (-2.7475) && *theta >= 2.7475 ){ 
        sumB = pheromones[x-3][y-3][type] + pheromones[x-2][y-3][type]                              +
               pheromones[x-3][y-2][type] + pheromones[x-2][y-2][type] + pheromones[x-1][y-2][type] +
               pheromones[x-3][y-1][type] + pheromones[x-2][y-1][type] + pheromones[x-1][y-1][type] ;
        
        sumA = pheromones[x-3][y+1][type] + pheromones[x-2][y+1][type] + pheromones[x-1][y+1][type] +
               pheromones[x-3][y+2][type] + pheromones[x-2][y+2][type] + pheromones[x-1][y+2][type] +
               pheromones[x-3][y+3][type] + pheromones[x-2][y+3][type]                              ;
    // ^
    }else if(*theta <= 1.9625 && *theta >= 1.1775 ){ 
        sumA = pheromones[x-3][y-3][type] + pheromones[x-2][y-3][type] + pheromones[x-1][y-3][type] +
               pheromones[x-3][y-2][type] + pheromones[x-2][y-2][type] + pheromones[x-1][y-2][type] +
                                          + pheromones[x-2][y-1][type] + pheromones[x-1][y-1][type] ;
        
        sumB = pheromones[x+1][y-3][type] + pheromones[x+2][y-3][type] + pheromones[x+3][y-3][type] +
               pheromones[x+1][y-2][type] + pheromones[x+2][y-2][type] + pheromones[x+3][y-2][type] +
               pheromones[x+1][y-1][type] + pheromones[x+2][y-1][type]                              ;
    // v
    }else if(*theta >= -1.9625 && *theta <= -1.1775 ){ 
        sumA = pheromones[x+1][y+1][type] + pheromones[x+2][y+1][type]                              +
               pheromones[x+1][y+2][type] + pheromones[x+2][y+2][type] + pheromones[x+3][y+2][type] +
               pheromones[x+1][y+3][type] + pheromones[x+2][y+3][type] + pheromones[x+3][y+3][type] ;
        
        sumB =                              pheromones[x-2][y+1][type] + pheromones[x-1][y+1][type] +
               pheromones[x-3][y+2][type] + pheromones[x-2][y+2][type] + pheromones[x-1][y+2][type] +
               pheromones[x-3][y+3][type] + pheromones[x-2][y+3][type] + pheromones[x-1][y+3][type] ;
    // A: ^  B: -->
    }else if(*theta >= 0.3925 && *theta <= 1.1775 ){ 
        sumA = pheromones[x-1][y-3][type] + pheromones[x][y-3][type]   + pheromones[x+1][y-3][type] +
               pheromones[x-1][y-2][type] + pheromones[x][y-2][type]   + pheromones[x+1][y-2][type] +
                                            pheromones[x][y-1][type]                                ;
        
        sumB =                              pheromones[x+2][y-1][type] + pheromones[x+3][y-1][type] +
               pheromones[x+1][y][type]   + pheromones[x+2][y][type]   + pheromones[x+3][y][type]   +
                                            pheromones[x+2][y+1][type] + pheromones[x+3][y+1][type] ;
    // A: <-- B: ^
    }else if(*theta >= 1.9625 && *theta <= 2.7475 ){ 
        sumA = pheromones[x-3][y-1][type] + pheromones[x-2][y-1][type]                              +
               pheromones[x-3][y][type]   + pheromones[x-2][y][type]   + pheromones[x-1][y][type]   +
               pheromones[x-3][y+1][type] + pheromones[x-2][y+1][type]                              ;
        
        sumB = pheromones[x-1][y-3][type] + pheromones[x][y-3][type]   + pheromones[x+1][y-3][type] +
               pheromones[x-1][y-2][type] + pheromones[x][y-2][type]   + pheromones[x+1][y-2][type] +
                                            pheromones[x][y-1][type]                                ;
    // A: v  B: <--
    }else if(*theta <= -1.9625 && *theta >= -2.7475 ){ 
        sumA =                              pheromones[x][y+1][type]                                +
               pheromones[x-1][y+2][type] + pheromones[x][y+2][type]   + pheromones[x+1][y+2][type] +
               pheromones[x-1][y+3][type] + pheromones[x][y+3][type]   + pheromones[x+1][y+3][type] ;
        
        sumB = pheromones[x-3][y-1][type] + pheromones[x-2][y-1][type]                              +
               pheromones[x-3][y][type]   + pheromones[x-2][y][type]   + pheromones[x-1][y][type]   +
               pheromones[x-3][y+1][type] + pheromones[x-2][y+1][type]                              ;
    // A: --> B: v
    }else if(*theta <= -0.3925 && *theta >= -1.1775 ){ 
        sumA =                              pheromones[x+2][y-1][type] + pheromones[x+3][y-1][type] +
               pheromones[x+1][y][type]   + pheromones[x+2][y][type]   + pheromones[x+3][y][type]   +
                                            pheromones[x+2][y+1][type] + pheromones[x+3][y+1][type] ;
        
        sumB =                              pheromones[x][y+1][type]                                +
               pheromones[x-1][y+2][type] + pheromones[x][y+2][type]   + pheromones[x+1][y+2][type] +
               pheromones[x-1][y+3][type] + pheromones[x][y+3][type]   + pheromones[x+1][y+3][type] ;
    }
    
    if(sumA > sumB) *theta += 0.04; // left
    else if(sumB > sumA) *theta -= 0.04; // right
    else if(sumA==0 && sumB==0){ 

        //TODO: if lost search further nearby

        *theta+= ((rand()%11)-5)/100.0 ; // no pheromone
        return true; // lost    
    }
    
    return false; // following a path
}

void release_pheromone(unsigned char pheromones[900][900][3], int x, int y, int type, int amount){
    if(pheromones[x][y][type]+amount > 255) // alarm pheromone
        pheromones[x][y][type] = 255;
    else     
        pheromones[x][y][type] += amount;
}


void ant_behaviour(Colony *colony, Ant *ant, unsigned char pheromones[900][900][3], bool enemy_location[900][900]){
    int x = convert_range2(ant->x);
    int y = convert_range2(ant->y); 
    int nest_x = convert_range2(colony->nest_x);
    int nest_y = convert_range2(colony->nest_y);
    
    if(ant->home_sick == 0){ // time to go home;
        ant->home_sick = -1;
        ant->theta += 3.14; // turn 180°
    }

    // ant at home 
    if(x > nest_x-33 && x < nest_x+33 && y > nest_y-33 && y < nest_y+33 ){
        ant->found_food = false;
        ant->intruder_detected = false;
        ant->home_sick = 1000;
        ant->lost = 0;  
    }

    // check nearby for intruders
    if(!ant->intruder_detected && search_for_enemies(x, y, enemy_location)){
        ant->intruder_detected = true;
        if(ant->home_sick > 0) ant->home_sick = 0;
        ant->lost = 0;// even if lost it can try a bit
    }
    // check if found a food source
    if(!ant->found_food && check_food_nearby(x, y)){
        ant->found_food = true;
        if(ant->home_sick > 0) ant->home_sick = 0;
        ant->lost = 0; // even if lost it can try a bit
    }

    // release pheromones
    if(ant->intruder_detected)
        release_pheromone(pheromones, x, y, 2, 50); // alarm pheromone
    if(ant->found_food)
        release_pheromone(pheromones, x, y, 1, 50); // food pheromone
    if(ant->home_sick > 0){
        release_pheromone(pheromones, x, y, 0, 50); // path pheromone
        
        // explore while not home_sick
        ant->theta += ((rand()%11)-5)/100.0 ; // random direction
        ant->home_sick--;

    }else // go home, but stop pheromones if lost
        if(follow_pheromone(x, y, &ant->theta, pheromones, 0)){
            ant->lost++;
            if(ant->lost > 100){ // lost the path for a while
                ant->found_food = false;
                ant->intruder_detected = false;           
            }
        }else
            ant->lost = 0;// is in a path
    
    
    
    
    
}


void process_colony(Colony *colony, bool enemy_location[900][900]){
    update_pheromones(colony);
    
    #pragma omp parallel for
    for(int i=0; i<colony->ants_amount; i++){
        ant_behaviour(colony, &colony->ants[i], colony->pheromones, enemy_location);
        move_ant(&colony->ants[i], 0.003, colony->ant_position);
    }
}






