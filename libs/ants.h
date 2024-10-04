#include <vector>

#ifndef ANTS
#define ANTS
typedef struct _ant{
    float radius, x, y, theta;
    unsigned char r,g,b;
    bool species;
}Ant;
#endif


Ant create_ant(float x, float y, unsigned char *color, bool species);
void draw_ant(Ant ant);
void move_ant(Ant *ant, float distance);
std::vector<Ant> create_swarm(float x, float y, unsigned char*color, bool species, int amount);

