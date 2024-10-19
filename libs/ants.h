/*
 TCC - Coevolucao entre duas Especies de Formigas Artificiais Competindo por Recursos
 Author - Luca Gomes Urssi
 Advisor - Eduardo do Valle Simoes
*/

#include <vector>

#ifndef ANTS
#define ANTS
typedef struct _ant{
    float radius, x, y, theta, initial_theta;
    unsigned char r,g,b;
    bool species;
}Ant;
#endif


Ant create_ant(float x, float y, unsigned char *color, bool species);

void draw_ant(Ant ant);
void move_ant(Ant *ant, float distance, unsigned char pheromones[900][900][3], bool ant_pos[900][900]);

std::vector<Ant> create_colony(float x, float y, unsigned char*color, bool species, int amount);
std::vector<Ant> reset_colony(std::vector<Ant> colony, int size, float x, float y);

void update_pheromones(unsigned char phero[900][900][3], unsigned short int * decay_timer, unsigned short int decay_timer_max);

