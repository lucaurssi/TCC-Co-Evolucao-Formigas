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

bool PLAY, simulate;
Ant Antony;
unsigned char color[3];
vector<Ant> blue_ants, red_ants;

void interface(){
    unsigned char color[3];

    setColor(color, WHITE); 
    retangle(0, 0, 2, 2, color); // background

    draw_bottom_menu(PLAY, simulate);

    setColor(color, BLUE);
    draw_nest(-0.5, -0.5, color);
    setColor(color, RED);
    draw_nest( 0.5, 0.5, color);
    
    setColor(color, BLACK);

    
    for(int i=0; i<20; i++){
        draw_ant(blue_ants[i]);
        draw_ant(red_ants[i]);             
    }
    




    
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
        for(int i=0; i<20; i++){
            move_ant(&blue_ants[i], 0.003);
            move_ant(&red_ants[i], 0.003);             
        }
    }
    
    return;
}

void button_click(int button, int state,int x, int y){
		
	if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN){
		if(DEBUG) 
			cout << "mouse: "<< x << ' '<< y << ", ";
        
        if(x > 15 && x < 40 && y > 860 && y < 890){ // play / stop button
            PLAY = !PLAY; if(DEBUG) cout << "PLAY/STOP pressed";   
        
        }else if(x > 60 && x<90 && y > 860 && y < 890){ // reset button
            if(DEBUG) cout << "RESET pressed"; // reset
        }else if(x > 110&& x < 135&&y > 860&& y < 890){
            simulate = !simulate; if(DEBUG) cout << "simulate pressed";
        }
    
        if(DEBUG) cout << '\n';
    }
    return;
}

void timer(int){
	processInterface();
	glutPostRedisplay();
	glutTimerFunc(1000/30, timer, 0);
}




int main(int argc, char** argv){
    PLAY = false;
    simulate = true;
    
    setColor(color, BLUE);
    blue_ants = create_swarm(-0.5, -0.5, color, 1, 20);
    setColor(color, RED);
    red_ants = create_swarm(0.5, 0.5, color, 0, 20);

  
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
