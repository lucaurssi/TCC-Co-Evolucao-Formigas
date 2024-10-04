#include <string>
#include <vector>

#define WHITE 		255,255,255
#define BLACK 		0,0,0
#define GREEN 		25,255,25
#define RED	  		255,0,0
#define BLUE  		25,25,255
#define GREY  		125,125,125
#define LIGHT_GREY 	200,200,200



// muda a cor na variavel
void setColor(unsigned char* color, unsigned char R, unsigned char G, unsigned char B);

// Funções para desenhar polígonos
void retangle(float x, float y, float w, float h, unsigned char*color);
void triangle(float x, float y, float h, float b, unsigned char*color);
void circle(float x, float y, float radius, unsigned char*color);


void RenderString(float x, float y, std::string string);
void draw_bottom_menu(bool PLAY, bool simulate);

void draw_nest(float x, float y, unsigned char*color);
