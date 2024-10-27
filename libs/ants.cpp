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
#include <iostream>

bool food_map[900][900];
    
Ant create_ant(int x, int y, unsigned char *color, int home_sick){
    Ant A;  

    A.radius = 0.01;
    A.x = x;
    A.y = y;
    A.theta = rand()%8; // moving direction
    A.initial_theta = A.theta;
    A.r = color[0] ;
    A.g = color[1] + 150; // change in color that make it easier to see the ants on the nest
    A.b = color[2];
    A.found_food = false;
    A.intruder_detected = false;
    A.home_sick = home_sick;
    A.lost =0;

    return A; 
}

// -1 - 1 to 0-900
int convert_range2(float x){
    int output = (x+1)*450; 
    if(output > 899) output=899;
    else if(output < 0) output=0;

    return output;
}

void move_ant(Ant *ant, bool ant_pos[900][900]){
 
    // remove ant from previous location
    ant_pos[ant->x][ant->y] = false; // location for ant vision
    
	// move towards theta
    switch(ant->theta){
        case 0: 
            ant->x++;
            break;
        case 1:
            ant->x++;
            ant->y--;
            break;
        case 2:
            ant->y--;
            break;
        case 3:
            ant->y--;
            ant->x--;
            break;
        case 4:
            ant->x--;
            break;
        case 5:
            ant->x--;
            ant->y++;
            break;
        case 6:
            ant->y++;
            break;
        case 7:
            ant->y++;
            ant->x++;
            break;     
        default:
            std::cout << "fuck "<< ant->theta << '\n';   
    }


	// end of screen
    if(ant->x < 0){ // top wall /\        /
        ant->x = 0;
        if(ant->theta == 1)ant->theta = 7;
        else if(ant->theta == 2)ant->theta = 6;
        else ant->theta = 5;
    
    }else if(ant->x > 899){ // bottom wall \/
        ant->x = 899;
        if(ant->theta == 5)ant->theta = 3;
        else if(ant->theta == 6)ant->theta = 2;
        else ant->theta = 1;
    }
	if(ant->y > 899){ // right wall  -->
        ant->y = 899;
        if(ant->theta == 1)ant->theta = 3;
        else if(ant->theta == 0)ant->theta = 4;
        else ant->theta = 5;
    
    }else if(ant->y < 0){ // left wall <--
        ant->y = 0;
        if(ant->theta == 3)ant->theta = 1;
        else if(ant->theta == 4)ant->theta = 0;
        else ant->theta = 7;
    }
    
   
    // add ant to new location
    ant_pos[ant->x][ant->y] = true; 
    
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

Colony create_colony(float x, float y, unsigned char*color, int amount){
    int ant_x = convert_range2(x);
    int ant_y = convert_range2(y);
    
    Colony swarm;
    Ant A;    
    swarm.home_sick_max = 300;

    for(int i=0; i<amount; i++){ // creates the ants of the colony 
        A = create_ant(ant_x, ant_y, color, swarm.home_sick_max);
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
    swarm.decay_timer_max = 15; // timer to reduce the pheromones on the map
    swarm.decay_timer = swarm.decay_timer_max;
    swarm.decay_amount = 1; // amount of pheromones reduced per cicle

    swarm.nest_x=x;
    swarm.nest_y=y;
    swarm.ants_amount = amount;
    swarm.food_found_amount = 0;
    
    swarm.alarm_phero_amount = 50;
    swarm.food_phero_amount = 100;
    swarm.path_phero_amount = 50;

    return swarm;
}

void reset_colony(Colony *colony){
    int x = convert_range2(colony->nest_x);
    int y = convert_range2(colony->nest_y);

    for(int i=0; i<colony->ants_amount; i++){
        colony->ants[i].theta = colony->ants[i].initial_theta;
        colony->ants[i].x = x;
        colony->ants[i].y = y;
        colony->ants[i].found_food = false;
        colony->ants[i].intruder_detected = false;
    }
    
    colony->food_found_amount = 0;

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
bool find_pheromone(int x, int y, int *theta, unsigned char pheromones[900][900][3], int type){
 
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

    int sumA=0, sumB=0, sumC=0; // sum of pheromones in it's path
    
    if(!(x+3 < 900 && x-3>0 && y+3 < 900 && y-3>0)) return false; // he do be blind near border.
    
    switch(*theta){
        // -->
        case 0: 
            sumA =                              pheromones[x+2][y-3][type] + pheromones[x+3][y-3][type] +
                   pheromones[x+1][y-2][type] + pheromones[x+2][y-2][type] + pheromones[x+3][y-2][type] +
                   pheromones[x+1][y-1][type] + pheromones[x+2][y-1][type] + pheromones[x+3][y-1][type] ;

            sumC = pheromones[x+1][y][type]   + pheromones[x+2][y][type]   + pheromones[x+3][y][type]   ;
            
            sumB = pheromones[x+1][y+1][type] + pheromones[x+2][y+1][type] + pheromones[x+3][y+1][type] +
                   pheromones[x+1][y+2][type] + pheromones[x+2][y+2][type] + pheromones[x+3][y+2][type] +
                                                pheromones[x+2][y+3][type] + pheromones[x+3][y+3][type] ;
            break;       
         // <--
        case 4:
            sumB = pheromones[x-3][y-3][type] + pheromones[x-2][y-3][type]                              +
                   pheromones[x-3][y-2][type] + pheromones[x-2][y-2][type] + pheromones[x-1][y-2][type] +
                   pheromones[x-3][y-1][type] + pheromones[x-2][y-1][type] + pheromones[x-1][y-1][type] ;

            sumC = pheromones[x-3][y][type]   + pheromones[x-2][y][type]   + pheromones[x-1][y][type]   ;
            
            sumA = pheromones[x-3][y+1][type] + pheromones[x-2][y+1][type] + pheromones[x-1][y+1][type] +
                   pheromones[x-3][y+2][type] + pheromones[x-2][y+2][type] + pheromones[x-1][y+2][type] +
                   pheromones[x-3][y+3][type] + pheromones[x-2][y+3][type]                              ;
            break;       
         // ^
        case 2: 
            sumA = pheromones[x-3][y-3][type] + pheromones[x-2][y-3][type] + pheromones[x-1][y-3][type] +
                   pheromones[x-3][y-2][type] + pheromones[x-2][y-2][type] + pheromones[x-1][y-2][type] +
                                              + pheromones[x-2][y-1][type] + pheromones[x-1][y-1][type] ;

            sumC = pheromones[x][y-1][type]   + pheromones[x][y-2][type]   + pheromones[x][y-3][type]   ;
            
            sumB = pheromones[x+1][y-3][type] + pheromones[x+2][y-3][type] + pheromones[x+3][y-3][type] +
                   pheromones[x+1][y-2][type] + pheromones[x+2][y-2][type] + pheromones[x+3][y-2][type] +
                   pheromones[x+1][y-1][type] + pheromones[x+2][y-1][type]                              ;
            break;
        // v
        case 6:
            sumA = pheromones[x+1][y+1][type] + pheromones[x+2][y+1][type]                              +
                   pheromones[x+1][y+2][type] + pheromones[x+2][y+2][type] + pheromones[x+3][y+2][type] +
                   pheromones[x+1][y+3][type] + pheromones[x+2][y+3][type] + pheromones[x+3][y+3][type] ;

            sumC = pheromones[x][y+1][type]   + pheromones[x][y+2][type]   + pheromones[x][y+3][type]   ;
            
            sumB =                              pheromones[x-2][y+1][type] + pheromones[x-1][y+1][type] +
                   pheromones[x-3][y+2][type] + pheromones[x-2][y+2][type] + pheromones[x-1][y+2][type] +
                   pheromones[x-3][y+3][type] + pheromones[x-2][y+3][type] + pheromones[x-1][y+3][type] ;
            break;
        // A: ^  B: -->
        case 1: 
            sumA = pheromones[x-1][y-3][type] + pheromones[x][y-3][type]   + pheromones[x+1][y-3][type] +
                   pheromones[x-1][y-2][type] + pheromones[x][y-2][type]   + pheromones[x+1][y-2][type] +
                                                pheromones[x][y-1][type]                                ;

            sumC = pheromones[x+1][y-1][type] + pheromones[x+2][y-2][type]                              ;
            
            sumB =                              pheromones[x+2][y-1][type] + pheromones[x+3][y-1][type] +
                   pheromones[x+1][y][type]   + pheromones[x+2][y][type]   + pheromones[x+3][y][type]   +
                                                pheromones[x+2][y+1][type] + pheromones[x+3][y+1][type] ;
            break;        
        // A: <-- B: ^
        case 3:
            sumA = pheromones[x-3][y-1][type] + pheromones[x-2][y-1][type]                              +
                   pheromones[x-3][y][type]   + pheromones[x-2][y][type]   + pheromones[x-1][y][type]   +
                   pheromones[x-3][y+1][type] + pheromones[x-2][y+1][type]                              ;

            sumC = pheromones[x-1][y-1][type] + pheromones[x-2][y-2][type]                              ;
            
            sumB = pheromones[x-1][y-3][type] + pheromones[x][y-3][type]   + pheromones[x+1][y-3][type] +
                   pheromones[x-1][y-2][type] + pheromones[x][y-2][type]   + pheromones[x+1][y-2][type] +
                                                pheromones[x][y-1][type]                                ;
            break;
        // A: v  B: <--
        case 5:
            sumA =                              pheromones[x][y+1][type]                                +
                   pheromones[x-1][y+2][type] + pheromones[x][y+2][type]   + pheromones[x+1][y+2][type] +
                   pheromones[x-1][y+3][type] + pheromones[x][y+3][type]   + pheromones[x+1][y+3][type] ;

            sumC = pheromones[x-1][y+1][type] + pheromones[x-2][y+2][type]                              ;
            
            sumB = pheromones[x-3][y-1][type] + pheromones[x-2][y-1][type]                              +
                   pheromones[x-3][y][type]   + pheromones[x-2][y][type]   + pheromones[x-1][y][type]   +
                   pheromones[x-3][y+1][type] + pheromones[x-2][y+1][type]                              ;
            break;
        // A: --> B: v
        case 7:
            sumA =                              pheromones[x+2][y-1][type] + pheromones[x+3][y-1][type] +
                   pheromones[x+1][y][type]   + pheromones[x+2][y][type]   + pheromones[x+3][y][type]   +
                                                pheromones[x+2][y+1][type] + pheromones[x+3][y+1][type] ;

            sumC = pheromones[x+1][y+1][type] + pheromones[x+2][y+2][type]                              ;
            
            sumB =                              pheromones[x][y+1][type]                                +
                   pheromones[x-1][y+2][type] + pheromones[x][y+2][type]   + pheromones[x+1][y+2][type] +
                   pheromones[x-1][y+3][type] + pheromones[x][y+3][type]   + pheromones[x+1][y+3][type] ;
            break;
    }

    if(sumC>sumA && sumC>sumB) ; // center is better path, keep current theta
    else if(sumA > sumB) *theta = (*theta+1)%8; // left
    else if(sumB > sumA) *theta = (*theta+7)%8; // right

    else if(sumA==0 && sumB==0){ // if no path, random theta, return lost
        int randd = rand()%100;
        if(randd < 2) *theta = (*theta+1)%8; // left (10% chance)
        else if(randd < 4) *theta = (*theta+7)%8; // right (10% chance)
        // else keep current theta // (80% chance)
        return true; // lost    
    }
    
    return false; // found a path
}

// try to follow a path, uses 'find_pheromone' if lost
bool follow_pheromone(int x, int y, int *theta, unsigned char pheromones[900][900][3], int type){

    if(pheromones[x][y][type] == 0) // not in a pheromone path of 'type' 
        return find_pheromone(x, y, theta, pheromones, type); 
    
    /*
        G H A    . A B    A B C
        F X B    . X C    H X D    ...
        E D C    . . .    G F E

        -->       ^ -->      ^

        3 2 1
        4 X 0  *theta
        5 6 7
    */

    int a,b,c,d,e,f,g,h;
    
    switch(*theta){
        case 0: // -->         
            a = pheromones[x+1][y-1][type];
            b = pheromones[x+1][y][type];
            c = pheromones[x+1][y+1][type];
            d = pheromones[x][y+1][type];
            e = pheromones[x-1][y+1][type];
            f = pheromones[x-1][y][type];
            g = pheromones[x-1][y-1][type];
            h = pheromones[x][y-1][type];
            break;
        case 1: // ^ -->         
            b = pheromones[x+1][y-1][type];
            c = pheromones[x+1][y][type];
            d = pheromones[x+1][y+1][type];
            e = pheromones[x][y+1][type];
            f = pheromones[x-1][y+1][type];
            g = pheromones[x-1][y][type];
            h = pheromones[x-1][y-1][type];
            a = pheromones[x][y-1][type];
            break;
        case 2: // ^         
            c = pheromones[x+1][y-1][type];
            d = pheromones[x+1][y][type];
            e = pheromones[x+1][y+1][type];
            f = pheromones[x][y+1][type];
            g = pheromones[x-1][y+1][type];
            h = pheromones[x-1][y][type];
            a = pheromones[x-1][y-1][type];
            b = pheromones[x][y-1][type];
            break;
        case 3: // <-- ^         
            d = pheromones[x+1][y-1][type];
            e = pheromones[x+1][y][type];
            f = pheromones[x+1][y+1][type];
            g = pheromones[x][y+1][type];
            h = pheromones[x-1][y+1][type];
            a = pheromones[x-1][y][type];
            b = pheromones[x-1][y-1][type];
            c = pheromones[x][y-1][type];
            break;
        case 4: // <--       
            e = pheromones[x+1][y-1][type];
            f = pheromones[x+1][y][type];
            g = pheromones[x+1][y+1][type];
            h = pheromones[x][y+1][type];
            a = pheromones[x-1][y+1][type];
            b = pheromones[x-1][y][type];
            c = pheromones[x-1][y-1][type];
            d = pheromones[x][y-1][type];
            break;
        case 5: // <-- V         
            f = pheromones[x+1][y-1][type];
            g = pheromones[x+1][y][type];
            h = pheromones[x+1][y+1][type];
            a = pheromones[x][y+1][type];
            b = pheromones[x-1][y+1][type];
            c = pheromones[x-1][y][type];
            d = pheromones[x-1][y-1][type];
            e = pheromones[x][y-1][type];
            break;
        case 6: // V         
            g = pheromones[x+1][y-1][type];
            h = pheromones[x+1][y][type];
            a = pheromones[x+1][y+1][type];
            b = pheromones[x][y+1][type];
            c = pheromones[x-1][y+1][type];
            d = pheromones[x-1][y][type];
            e = pheromones[x-1][y-1][type];
            f = pheromones[x][y-1][type];
            break;
        case 7: // V -->         
            h = pheromones[x+1][y-1][type];
            a = pheromones[x+1][y][type];
            b = pheromones[x+1][y+1][type];
            c = pheromones[x][y+1][type];
            d = pheromones[x-1][y+1][type];
            e = pheromones[x-1][y][type];
            f = pheromones[x-1][y-1][type];
            g = pheromones[x][y-1][type];
            break;
    }

    

    

    if(b != 0) return false; // pheromones foward (B), keep going

    else if(a==0 && c==0){ // no pheromone on A or C
        
        if(d==0 && h==0){ // no pheromone on D or H
            
            if(f==0 && g==0 && e==0) // no pheromone around
                return find_pheromone(x,y,theta, pheromones, type); 

            else if(g > f && g > e) *theta = (*theta + 3)%8; // G has a path
            else if(e > f) *theta = (*theta + 5)%8; // E has a path
            else if(find_pheromone(x,y,theta, pheromones, type)) // if no other path is found further ahead, goes back to F
                *theta = (*theta + 4)%8; // F has a path 
        }
        else if(h > d) // turn hard left (H)
            *theta = (*theta+2)%8;
        else      // turn hard right (D)
            *theta = (*theta+6)%8;
    }
    else if(a > c) // turn left (A)
        *theta = (*theta+1)%8;
    else      // turn right (C)
        *theta = (*theta+7)%8;


    return false; // not lost

}

void release_pheromone(unsigned char pheromones[900][900][3], int x, int y, int type, int amount){
    if(pheromones[x][y][type]+amount > 255) // alarm pheromone
        pheromones[x][y][type] = 255;
    else     
        pheromones[x][y][type] += amount;
}


void ant_behaviour(Colony *colony, Ant *ant, unsigned char pheromones[900][900][3], bool enemy_location[900][900]){
    int x = ant->x;
    int y = ant->y; 
    int nest_x = convert_range2(colony->nest_x);
    int nest_y = convert_range2(colony->nest_y);
    
    if(ant->home_sick == 0){ // time to go home;
        ant->home_sick = -1;
        ant->theta = (ant->theta + 4)%8 ; // turn 180°
    }

    // ant at home 
    if(x > nest_x-33 && x < nest_x+33 && y > nest_y-33 && y < nest_y+33 ){
        if(ant->found_food){
            ant->theta = (ant->theta + 4)%8 ; // turn 180°
            colony->food_found_amount++;        
        }
        ant->found_food = false;
        ant->intruder_detected = false;
        ant->home_sick = colony->home_sick_max; // refresh 
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
        release_pheromone(pheromones, x, y, 2, colony->alarm_phero_amount); // alarm pheromone
    if(ant->found_food)
        release_pheromone(pheromones, x, y, 1, colony->food_phero_amount); // food pheromone
    if(ant->home_sick > 0){ // exploring
        release_pheromone(pheromones, x, y, 0, colony->path_phero_amount); // path pheromone
        
        // explore while not home_sick, follows food pheromone is found
        follow_pheromone(x, y, &ant->theta, pheromones, 1); 
        ant->home_sick--;

    }else // go home, but stop pheromones if lost
        if(follow_pheromone(x, y, &ant->theta, pheromones, 0)){ // following path pheromone, true means lost
            ant->lost++;
            if(ant->lost > 100){ // lost the path for a while
                ant->found_food = false;
                ant->intruder_detected = false;           
            }
        }else
            ant->lost = 0;// is in or near a path a path
        
}


void process_colony(Colony *colony, bool enemy_location[900][900]){
    update_pheromones(colony);
    
    #pragma omp parallel for
    for(int i=0; i<colony->ants_amount; i++){
        ant_behaviour(colony, &colony->ants[i], colony->pheromones, enemy_location);
        move_ant(&colony->ants[i], colony->ant_position);
    }
}






