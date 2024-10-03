

#ifndef ANTS
#define ANTS
typedef struct _ant{
    float radius, x, y, theta;
    unsigned char r,g,b;
}Ant;
#endif


Ant create_ant(float x, float y, unsigned char *color);
void draw_ant(Ant ant);
void move_ant(Ant *ant, float distance);

