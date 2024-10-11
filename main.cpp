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

#include "libs/drawings.h"
#include "libs/ants.h"

#include <vector>
#include <iostream>

using namespace std;

#define windowWidth 900
#define windowHeight 900
#define DEBUG 1
#define COLONY_SIZE 100

unsigned char color[3];
bool PLAY, simulate;

/*  3 types of pheromones; exploration, food & intruder. 
    ants look for food with 'exploration' pheromone.
    when food is found they follow the path back to the nest while releasing 'food' pheromone.
    at any point, if a hostile being is sighted the ant start releasing 'intruder' pheromone and goes home.

    there are two colonies and it was choosen that the ants don't share pheromones.
*/
unsigned char blue_pheromones[windowWidth][windowHeight][3];
unsigned char red_pheromones[windowWidth][windowHeight][3];

//Ant Antony; // Antony is the ant that helped debugging this code.
vector<Ant> blue_ants, red_ants;



void interface(){
    unsigned char color[3];

    setColor(color, BLACK); 
    retangle(0, 0, 2, 2, color); // background


    setColor(color, BLUE);
    draw_nest(-0.5, -0.5, color);
    setColor(color, RED);
    draw_nest( 0.5, 0.5, color);
    
    try{
    draw_pheromones(blue_pheromones, red_pheromones, true, false);
    }
    catch(int er){
        cout << "something fucky happened "<< er <<'\n';    
    }
        
    //draw_ant(Antony);

    for(int i=0; i<COLONY_SIZE; i++){
        draw_ant(blue_ants[i]);
        draw_ant(red_ants[i]);             
    }

    
    draw_bottom_menu(PLAY, simulate); // menu
    
    glutSwapBuffers();	
    return;
}


void processInterface(){
    // checks for changes in window size and reset it
    // does not change fullscreen
    int w = glutGet(GLUT_WINDOW_WIDTH); 
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    if(w!=windowWidth || h!=windowHeight){
        if(DEBUG)cout << "Screen size diferent, resizing: "<< w << ' ' << h << '\n';
        glutReshapeWindow(windowWidth, windowHeight);
    }
    
    if(PLAY){
        //move_ant(&Antony, 0.003);
        
        for(int i=0; i<COLONY_SIZE; i++){
            move_ant(&blue_ants[i], 0.003, blue_pheromones);
            move_ant(&red_ants[i], 0.003, red_pheromones);             
        }
    }
    
    return;
}


void button_click(int button, int state,int x, int y){
	if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN){
		if(DEBUG) 
			cout << "[DEBUG] left_mouse_click: "<< x << ' '<< y;
        
        if(x > 15 && x < 40 && y > 860 && y < 890){ // play / stop button
            PLAY = !PLAY; if(DEBUG) cout << " - PLAY/STOP pressed";   
        
        }else if(x > 60 && x<90 && y > 860 && y < 890){ // reset button
            if(DEBUG) cout << " - RESET pressed";
            PLAY = false;

            blue_ants = reset_colony(blue_ants, COLONY_SIZE, -0.5, -0.5);
            red_ants = reset_colony(red_ants, COLONY_SIZE, 0.5, 0.5);

            for(int i=0; i<windowWidth; i++)
                for(int j=0; j<windowHeight; j++)
                    for(int k=0; k<3; k++){
                        blue_pheromones[i][j][k]=0;            
                        red_pheromones[i][j][k]=0;
                    }
            
        }else if(x > 110&& x < 135&&y > 860&& y < 890){
            simulate = !simulate; if(DEBUG) cout << " - simulate pressed";
        }
    
        if(DEBUG) cout<<'\n';
    }

    return;
}


void timer(int){
	processInterface(); // calculations

	glutPostRedisplay(); // redraw frame

    glutTimerFunc(1000/30, timer, 0);
}



int main(int argc, char** argv){
    PLAY = false;
    simulate = true;

    if(DEBUG) cout << "DEBUG mode is ON.\n";

    // setting pheromones to zero on the entire map
    for(int i=0; i<windowWidth; i++)
        for(int j=0; j<windowHeight; j++)
            for(int k=0; k<3; k++){
                blue_pheromones[i][j][k]=0;            
                red_pheromones[i][j][k]=0;
            }
    
    //setColor(color, BLACK);
    //Antony = create_ant(0,0,color, 1);
    
    setColor(color, BLUE);
    blue_ants = create_colony(-0.5, -0.5, color, 1, COLONY_SIZE);
    setColor(color, RED);
    red_ants = create_colony(0.5, 0.5, color, 0, COLONY_SIZE);

  
    //----- Create Window -----//
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("TCC-Co-Evolucao-Formigas");
    glClearColor(1.0, 1.0, 1.0, 1.0);// clear screen

    glutDisplayFunc(interface);// defining drawing function

    glutTimerFunc(0, timer, 0); // clock, processing function here
	glutMouseFunc(button_click);// mouse click handler

    glutMainLoop();
    return 0;
}
