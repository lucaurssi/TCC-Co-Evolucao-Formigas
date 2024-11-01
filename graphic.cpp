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
#define COLONY_SIZE 50

/*
    TO DO list:
    - better ant vision ? shorter is better ? (follow / find)

    - soldier ant 

    - proper evolution, leave it running overnight
*/

unsigned char color[3];
bool PLAY, Graphics;


Colony blue_ants, red_ants;    
Food food;


void interface(){
    unsigned char color[3];

    setColor(color, BLACK); 
    retangle(0, 0, 2, 2, color); // background

    if(Graphics){

        draw_pheromones(blue_ants.pheromones, red_ants.pheromones, blue_ants.draw_phero, red_ants.draw_phero);
        
        draw_nest(-0.5, -0.5, BLUE);
        draw_nest( 0.5, 0.5, RED);

        setColor(color, GREEN);
        retangle(convert_range(food.x), convert_range(food.y), 0.04, 0.04, color); // Food spot
        

        setColor(color, LIGHT_BLUE);

        for(int i=0; i<blue_ants.soldiers_amount; i++)
            if(blue_ants.ants[i].alive)
                retangle(convert_range(blue_ants.ants[i].x), convert_range(blue_ants.ants[i].y), 0.03, 0.03, color);

        for(int i=blue_ants.soldiers_amount; i<blue_ants.ants_amount; i++)
            if(blue_ants.ants[i].alive)
                triangle(convert_range(blue_ants.ants[i].x), convert_range(blue_ants.ants[i].y), 0.02, 0.02, color);
        

        
        setColor(color, LIGHT_RED);
        
        for(int i=0; i<red_ants.soldiers_amount; i++)
            if(red_ants.ants[i].alive)
                retangle(convert_range(red_ants.ants[i].x), convert_range(red_ants.ants[i].y), 0.03, 0.03, color);

        for(int i=red_ants.soldiers_amount; i<red_ants.ants_amount; i++)
            if(red_ants.ants[i].alive)
                triangle(convert_range(red_ants.ants[i].x), convert_range(red_ants.ants[i].y), 0.02, 0.02, color);           
        
    }
    
    draw_bottom_menu(PLAY, Graphics,  blue_ants.draw_phero, red_ants.draw_phero);
    
    glutSwapBuffers();	
    return;
}


void processInterface(){
    // checks for changes in window size and reset it
    // does not change fullscreen [ TODO: detect and close fullscreen ]
    int w = glutGet(GLUT_WINDOW_WIDTH); 
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    if(w!=900 || h!=900){
        if(DEBUG)cout << "Screen size diferent, resizing: "<< w << ' ' << h << '\n';
        glutReshapeWindow(900, 900);
    }
    
    if(PLAY){        
        process_colony(&blue_ants, red_ants.ant_position, &food);
        process_colony(&red_ants, blue_ants.ant_position, &food);
        process_food(&food);
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
            reset_colony(&blue_ants);
            reset_colony(&red_ants);
            
        }else if(x > 110&& x < 135&&y > 860&& y < 890){
            Graphics = !Graphics; if(DEBUG) cout << " - Graphics pressed";

        }else if(x > 255&& x < 285&&y > 860&& y < 890){
            blue_ants.draw_phero = !blue_ants.draw_phero; if(DEBUG) cout << " - Blue pheromones pressed";

        }else if(x > 480&& x < 510&&y > 860&& y < 890){
            red_ants.draw_phero = !red_ants.draw_phero; if(DEBUG) cout << " - Red pheromones pressed";
        }
    
        if(DEBUG) cout<<'\n';
    }

    return;
}


void timer(int){
	processInterface(); // calculate things every frame

	glutPostRedisplay(); // call "interface" function again

    
    glutTimerFunc(1000/60, timer, 0); // call "timer" function again after 1000/30 miliseconds
}



int main(int argc, char** argv){
    
    if(DEBUG) cout << "DEBUG mode is ON.\n";

    PLAY = false; // the simulation is paused by default
    Graphics = true; // visual simulation
    
    // --- creating ant colony --- //
    blue_ants = create_colony(-0.5, -0.5, COLONY_SIZE, 10);
    red_ants = create_colony(0.5, 0.5, COLONY_SIZE, 10);

    create_food_map(&food);
  
    //----- Create Window -----//
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("TCC-Co-Evolucao-Formigas");

    glClearColor(1.0, 1.0, 1.0, 1.0);// clear screen

    glutDisplayFunc(interface);// defining drawing function as interface()

    glutTimerFunc(0, timer, 0); // clock, processing function here

	glutMouseFunc(button_click);// mouse click handler

    glutMainLoop();
    return 0;
}