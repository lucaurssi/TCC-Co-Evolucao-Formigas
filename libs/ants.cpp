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


Ant create_ant(float x, float y, unsigned char *color, bool species){
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

void move_ant(Ant *ant, float distance, unsigned char pheromones[900][900][3], bool ant_pos[900][900]){
    int x,y;	
    
	ant->theta+= ((rand()%11)-5)/100.0 ; // update theta
    
    // this keeps theta value between -3.14 and 3.14, which is the used directions in our functions
    if(ant->theta > 3.14) ant->theta = ant->theta - 6.28;
    if(ant->theta < -3.14) ant->theta = ant->theta + 6.28;
    
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
    
    // add color to pheromone on the ant location
    if(pheromones[x][y][2]+50 > 255)
        pheromones[x][y][2] = 255;
    else     
        pheromones[x][y][2] += 50;
    
}


void update_pheromones(unsigned char phero[900][900][3], unsigned short int * decay_timer, unsigned short int decay_timer_max, unsigned char decay_amount){
    
    *decay_timer -= 1;
    if(*decay_timer > 0) return; // not ready to decay
    
    *decay_timer = decay_timer_max;
    
    for(int i=0; i<900; i++)
        for(int j=0; j<900; j++)
            for(int k=0; k<3; k++)
                if(phero[i][j][k] > 0)phero[i][j][k] -= decay_amount;
}

Colony create_colony(float x, float y, unsigned char*color, bool species, int amount){
    Colony swarm;
    Ant A;    
    for(int i=0; i<amount; i++){ // creates the ants of the colony 
        A = create_ant(x, y, color, species);
        swarm.ants.push_back(A); 
    }

    for(int i=0; i<900; i++)
        for(int j=0; j<900; j++){
            swarm.ant_position[i][j] = false; // map is empty of ants
            for(int k=0; k<3; k++){
                swarm.pheromones[i][j][k]=0; // map is clear of pheromones
            }
        }
    
    swarm.draw_phero = false; // this option is off by default
    swarm.decay_timer_max = 5; // timer to reduce the pheromones on the map
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
    }

    for(int i=0; i<900; i++)
        for(int j=0; j<900; j++){
            colony->ant_position[i][j] = false; // map is empty of ants
            for(int k=0; k<3; k++){
                colony->pheromones[i][j][k]=0; // map is clear of pheromones
            }
        }
    
    return;
}

void process_colony(Colony *colony){
    update_pheromones(colony->pheromones, &colony->decay_timer, colony->decay_timer_max, colony->decay_amount);
        
    for(int i=0; i<colony->ants_amount; i++)
        move_ant(&colony->ants[i], 0.003, colony->pheromones, colony->ant_position);
     
}

/*
void ant_behaviour(Ant ant, unsigned char pheromones[900][900][3], bool ant_pos[900][900]){
    
}
*/







