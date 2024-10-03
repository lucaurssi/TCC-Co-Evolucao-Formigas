#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#include "libs/drawings.h"
#include "libs/ants.h"
#include <iostream>

using namespace std;

#define windowWidth 900
#define windowHeight 900
#define DEBUG 1

bool PLAY;
Ant Antony;
unsigned char color[3];


void interface(){
    unsigned char color[3];

    setColor(color, WHITE); 
    retangle(0, 0, 2, 2, color); // background

    
    
    draw_bottom_menu(PLAY);

    setColor(color, BLUE);
    draw_nest(-0.5, -0.5, color);
    setColor(color, RED);
    draw_nest( 0.5, 0.5, color);
    
    setColor(color, BLACK);

    
    
    draw_ant(Antony);
    




    
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
        move_ant(&Antony, 0.005);
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

    setColor(color, BLACK); 
    Antony = create_ant(0, 0, color);
  
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
