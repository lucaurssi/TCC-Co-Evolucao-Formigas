#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif
#include <math.h>
#include <string>
#include <iostream>
#include "drawings.h"

#define FONT GLUT_BITMAP_TIMES_ROMAN_24

using namespace std;



void setColor(unsigned char* color, unsigned char R, unsigned char G, unsigned char B){
	color[0] = R;
	color[1] = G;
	color[2] = B;
	return; 
}

void retangle(float x, float y, float w, float h, unsigned char*color){
  glColor3ub(color[0], color[1], color[2]); // define color
  glBegin(GL_POLYGON);// intiate the polygon

  glVertex2d( x-w/2, y-h/2);
  glVertex2d( x-w/2, y+h/2);
  glVertex2d( x+w/2, y+h/2);
  glVertex2d( x+w/2, y-h/2);

  glEnd();// end polygon
}


void triangle(float x, float y, float h, float b, unsigned char*color){
  glColor3ub(color[0], color[1], color[2]);
  glBegin(GL_POLYGON);

  glVertex2d( x-b/2, y-h/2);
  glVertex2d( x+b/2, y-h/2);
  glVertex2d( x, y+h/2);

  glEnd();
}


void circle(float x, float y, float radius, unsigned char*color){
  glColor3ub(color[0], color[1], color[2]);
  glBegin(GL_POLYGON);

  for (int i = 0; i < 360; i+=5) 
    glVertex2d( radius*cos(i/180.0*M_PI) + x, radius*sin(i/180.0*M_PI) + y);

  glEnd();
}


void RenderString(float x, float y, string string){  
	
	glColor3ub(0, 0, 0); // black
	glRasterPos2f(x, y); 
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[0]); // write one character
	
	for(int i=1; i<(int)string.length(); i++){
		x = (x*900)+ 2*glutBitmapWidth(FONT, string[i-1]); // move x by the width of the character
		x = x/900;
		glRasterPos2f(x, y);
		glutBitmapCharacter(FONT, string[i]);
	}
}


void draw_bottom_menu(bool PLAY){
	unsigned char color[3];
	
	// draw a light_grey line in the bottom of the screen
	setColor(color, LIGHT_GREY);
	retangle(0, -1, 2, 0.2, color); 
	
    setColor(color, BLACK);
    if(!PLAY){ // play button
        glColor3ub(color[0], color[1], color[2]);
        glBegin(GL_POLYGON);

        glVertex2d( -0.95, -0.98);
        glVertex2d( -0.95, -0.92);
        glVertex2d( -0.9, -0.95);

        glEnd();
    }else{ // stop button
	    retangle(-0.95, -0.95, 0.016, 0.06, color); 
	    retangle(-0.92, -0.95, 0.016, 0.06, color); 
    }
    // reset button
    setColor(color, RED);
	retangle(-0.83, -0.95, 0.05, 0.05, color); 

	
}

void draw_nest(float x, float y, unsigned char*color){
    retangle(x, y, 0.2, 0.2, color);
}


















