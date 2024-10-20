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
    A.have_food = false;
    A.intruder_detected = false;

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
    
   
    
}


void update_pheromones(Colony * colony){
    
    colony->decay_timer -= 1;
    if(colony->decay_timer > 0) return; // not ready to decay
    
    colony->decay_timer = colony->decay_timer_max;
    
    for(int i=0; i<900; i++)
        for(int j=0; j<900; j++)
            for(int k=0; k<3; k++)
                if(colony->pheromones[i][j][k] > 0) colony->pheromones[i][j][k] -= colony->decay_amount;
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
    swarm.decay_timer_max = 10; // timer to reduce the pheromones on the map
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
        colony->ants[i].have_food = false;
        colony->ants[i].intruder_detected = false;
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


bool search_for_enemies(int x, int y, bool enemy_location[900][900]){
    

    if(x+2 < 900 && x-2>0 && y+2 < 900 && y-2>0) // not near borders
        for(int i=x-2; i<x+3; i++) 
            for(int j=y-2; j<y+3; j++) // check for enemy ant 2 pixels away
                if(enemy_location[i][j])
                    return true;
    return false;
}

void follow_pheromone(int x, int y, float *theta, unsigned char pheromones[900][900][3], int type){
/*
    X - ant
    A - left sensor
    B - right sensor

                      1,9625-1,1775
 2.7475-1.9625    A A . B B
           . B B  A A . B B  A A . 1,1775-0.3925
           A . B      X      A . B
           A A X             X B B
                               
         B B                   A A
         B B                   A A  
         . . X               X . .  0.3925-(-0,3925)
         A A                   B B
         A A                   B B
        (-2.7475)-2.7475        
                    ...
*/    

    int sumA=0, sumB=0; // sum of pheromones in it's path
    
    if(!(x+2 < 900 && x-2>0 && y+2 < 900 && y-2>0)) return; // he do be blind near border.

    if(*theta >= -0.3925 && *theta <= 0.3925 ){ // -->
        sumA += pheromones[x+2][y-2][type] + pheromones[x+1][y-2][type] + pheromones[x+1][y-1][type] + pheromones[x+2][y-1][type];
        sumB += pheromones[x+2][y+2][type] + pheromones[x+1][y+2][type] + pheromones[x+1][y+1][type] + pheromones[x+2][y+1][type]; 

    }else if(*theta <= (-2.7475) && *theta >= 2.7475 ){ // <--
        sumA += pheromones[x-2][y+2][type] + pheromones[x-1][y+2][type] + pheromones[x-1][y+1][type] + pheromones[x-2][y+1][type];
        sumB += pheromones[x-2][y-2][type] + pheromones[x-1][y-2][type] + pheromones[x-1][y-1][type] + pheromones[x-2][y-1][type]; 

    }else if(*theta <= 1.9625 && *theta >= 1.1775 ){ // ^
        sumA += pheromones[x-2][y-2][type] + pheromones[x-1][y-2][type] + pheromones[x-1][y-1][type] + pheromones[x-2][y-1][type];
        sumB += pheromones[x+2][y-2][type] + pheromones[x+1][y-2][type] + pheromones[x+1][y-1][type] + pheromones[x+2][y-1][type]; 

    }else if(*theta >= -1.9625 && *theta <= -1.1775 ){ // v
        sumA += pheromones[x+2][y+2][type] + pheromones[x+1][y+2][type] + pheromones[x+1][y+1][type] + pheromones[x+2][y+1][type];
        sumB += pheromones[x-2][y+2][type] + pheromones[x-1][y+2][type] + pheromones[x-1][y+1][type] + pheromones[x-2][y+1][type]; 

    }

    if(sumA > sumB) *theta += 0.04;
    else if(sumB < sumA) *theta -= 0.04;
    else if(sumA==0 && sumB==0) *theta+= ((rand()%10)-5)/100.0 ; // no pheromone, lost/exploring


}


void ant_behaviour(Ant *ant, unsigned char pheromones[900][900][3], bool enemy_location[900][900]){
    int x = convert_range2(ant->x);
    int y = convert_range2(ant->y); 

    if(!ant->intruder_detected && search_for_enemies(x, y, enemy_location)){
        ant->intruder_detected = true;
        ant->theta += 3.14; // turn 180°
    }

    if(ant->intruder_detected){
        if(pheromones[x][y][0]+50 > 255) // alarm pheromone
            pheromones[x][y][0] = 255;
        else     
            pheromones[x][y][0] += 50;

        follow_pheromone(x, y, &ant->theta, pheromones, 2);   
        return;         
    }
    else{
        
        if(pheromones[x][y][2]+50 > 255)
            pheromones[x][y][2] = 255;
        else     
            pheromones[x][y][2] += 50;

        follow_pheromone(x, y, &ant->theta, pheromones, 2);  

    }
    // add color to pheromone on the ant location
    

    // check current behaviour
    // if (intruder || check_intruder())
        // { go_home, alarm_phero || got_home, reset ; return;}
    // if (found_food)
        //{ go_home, food_phero || got_home, release food, follow_food_phero; return;}
    // else
        // explore: check_food_phero, follow || random_move, path_phero
    
}


void process_colony(Colony *colony, bool enemy_location[900][900]){
    update_pheromones(colony);
        
    for(int i=0; i<colony->ants_amount; i++){
        ant_behaviour(&colony->ants[i], colony->pheromones, enemy_location);
        move_ant(&colony->ants[i], 0.003, colony->ant_position);
    }
}






