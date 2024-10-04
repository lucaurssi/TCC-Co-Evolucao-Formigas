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

    A.radius = 0.02;
    A.x = x;
    A.y = y;
    A.theta = ((rand()%20)-10)/10.0; // moving direction
    A.r = color[0];
    A.g = color[1] + 50;
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

void move_ant(Ant *ant, float distance){
	
	ant->theta+= ((rand()%11)-5)/100.0 ; // update theta

	// move towards theta
	ant->x = ant->x + distance*cos(ant->theta);
	ant->y = ant->y + distance*sin(ant->theta);

	// end of screen
	ant->x = ant->x>1 ? -1 : ant->x;
	ant->y = ant->y>1 ? -1 : ant->y;
	ant->x = ant->x<-1 ? 1 : ant->x;
	ant->y = ant->y<-1 ? 1 : ant->y;

}

std::vector<Ant> create_swarm(float x, float y, unsigned char*color, bool species, int amount){
    std::vector<Ant> Ants;    
    Ant A;    
    for(int i=0; i<amount; i++){
        A = create_ant(x, y, color, species);
        Ants.push_back(A); 
    }

    return Ants;
}










