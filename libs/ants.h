/*
 TCC - Coevolucao entre duas Especies de Formigas Artificiais Competindo por Recursos
 Author - Luca Gomes Urssi
 Advisor - Eduardo do Valle Simoes
*/

#include <vector>

#ifndef ANT
#define ANT
typedef struct _ant{
    float radius, x, y, theta, initial_theta;
    unsigned char r,g,b;
    bool species;
    bool found_food, intruder_detected;
    int home_sick; 
    int lost;
    
}Ant;
#endif

#ifndef COLONY
#define COLONY
typedef struct _colony{
    std::vector<Ant> ants;
    bool ant_position[900][900];
    unsigned char pheromones[900][900][3];
    bool draw_phero;
    unsigned short int decay_timer, decay_timer_max;
    unsigned char decay_amount;
    float nest_x, nest_y;
    int ants_amount;
}Colony;
#endif

//Ant create_ant();
//int convert_range2(float x)
//void move_ant();
//void update_pheromones(Colony *colony);
//void ant_behaviour();

void draw_ant(Ant ant);

Colony create_colony(float x, float y, unsigned char*color, bool species, int amount);
void reset_colony(Colony *colony);
void process_colony(Colony *colony, bool enemy_location[900][900]);
void create_food_map();



