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

void check_box(float x, float y, bool state, string name){
    unsigned char color[3];

    setColor(color, GREY);    
    circle(x, y, 0.03, color);
    setColor(color, BLUE);     
    if(state) circle(x, y, 0.02, color);

    RenderString(x+0.05, y-0.015, name);
    

}

void draw_bottom_menu(bool PLAY, bool Graphics, bool b_phero, bool r_phero){
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

    check_box(-0.73, -0.95, Graphics, "Graphics");

    check_box(-0.4, -0.95, b_phero, "Blue Pheromones");
    check_box(0.1, -0.95, r_phero, "Red Pheromones");
	
}


void draw_nest(float x, float y, unsigned char R, unsigned char G, unsigned char B){
    unsigned char color[3];
    setColor(color, R, G, B);
    retangle(x, y, 0.2, 0.2, color);
}

// converts from range 0-900 to range -1 - 1
float convert_range(int x){
    return (x/450.0)-1;
}

// doesn't work if the screen change it's size 
void draw_pheromones(unsigned char b_phero[900][900][3], unsigned char r_phero[900][900][3], bool draw_blue, bool draw_red){
    unsigned char color[3];    
    
    unsigned char r,g,b;
    
    for(int i=0; i<900; i++)
        for(int j=0; j<900; j++){
            r=0; g=0; b=0;            
            if(draw_blue){ // draw blue pheromone
                r = b_phero[i][j][0];
                g = b_phero[i][j][1];
                b = b_phero[i][j][2];
                if(draw_red){ // blue & red
                    r = (r + r_phero[i][j][2] > 255) ? (255) : (r + r_phero[i][j][2]);
                    g = (g + r_phero[i][j][1] > 255) ? (255) : (g + r_phero[i][j][1]);
                    b = (b + r_phero[i][j][0] > 255) ? (255) : (b + r_phero[i][j][0]);                
                }           
            }
            else if(draw_red){ // draw red but not blue
                r = r_phero[i][j][2];
                g = r_phero[i][j][1];
                b = r_phero[i][j][0];
            }                

            if(!(r==0 && g==0 && b==0)){
                    setColor(color, r,g,b);
                    retangle( convert_range(i), convert_range(j), 0.003, 0.003, color);// draw pixel 
                } 
        }

    return;
}
















