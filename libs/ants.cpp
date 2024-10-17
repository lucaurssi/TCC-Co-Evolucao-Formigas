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
    return (x+1)*450;
}

void move_ant(Ant *ant, float distance, unsigned char pheromones[900][900][3]){
	
	ant->theta+= ((rand()%11)-5)/100.0 ; // update theta
    
    // this keeps theta value between -3.14 and 3.14, which is the used directions in our functions
    if(ant->theta > 3.14) ant->theta = ant->theta - 6.28;
    if(ant->theta < -3.14) ant->theta = ant->theta + 6.28;

	// move towards theta
	ant->x = ant->x + distance*cos(ant->theta);
	ant->y = ant->y + distance*sin(ant->theta);

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
    
    // coordinates for pheromone handler
    int x = convert_range2(ant->x), y = convert_range2(ant->y);
    if(x > 899) x=899;
    else if(x < 0) x=0;
    
    if(y > 899) y=899;
    else if(y < 0) y=0;

    if(pheromones[x][y][2]+50 > 255)
        pheromones[x][y][2] = 255;
    else     
        pheromones[x][y][2] += 50;
    
}

std::vector<Ant> create_colony(float x, float y, unsigned char*color, bool species, int amount){
    std::vector<Ant> Ants;    
    Ant A;    
    for(int i=0; i<amount; i++){
        A = create_ant(x, y, color, species);
        Ants.push_back(A); 
    }

    return Ants;
}

std::vector<Ant> reset_colony(std::vector<Ant> colony, int size, float x, float y){
    for(int i=0; i<size; i++){
        colony[i].theta = colony[i].initial_theta;
        colony[i].x = x;
        colony[i].y = y;
    }
    return colony;
}








