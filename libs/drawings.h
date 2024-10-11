/*
 TCC - Coevolucao entre duas Especies de Formigas Artificiais Competindo por Recursos
 Author - Luca Gomes Urssi
 Advisor - Eduardo do Valle Simoes
*/

#include <string>
#include <vector>

#define WHITE 		255,255,255
#define BLACK 		0,0,0
#define GREEN 		25,255,25
#define RED	  		255,0,0
#define BLUE  		25,25,255
#define GREY  		125,125,125
#define LIGHT_GREY 	200,200,200



// change the color in 'color'    
void setColor(unsigned char* color, unsigned char R, unsigned char G, unsigned char B);

// basic drawing functions
void retangle(float x, float y, float w, float h, unsigned char*color);
void triangle(float x, float y, float h, float b, unsigned char*color);
void circle(float x, float y, float radius, unsigned char*color);

// text writing function
void RenderString(float x, float y, std::string string);

// this function draws the background and bottom menu
void draw_bottom_menu(bool PLAY, bool simulate);

// this function draws a colored square on the x,y position
// to do: make the nest look better
void draw_nest(float x, float y, unsigned char*color);

void draw_pheromones(unsigned char b_phero[900][900][3], unsigned char r_phero[900][900][3], bool draw_blue, bool draw_red);
