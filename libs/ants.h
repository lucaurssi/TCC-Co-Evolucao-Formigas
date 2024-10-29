/*
 TCC - Coevolucao entre duas Especies de Formigas Artificiais Competindo por Recursos
 Author - Luca Gomes Urssi
 Advisor - Eduardo do Valle Simoes
*/

#include <vector>

#ifndef ANT
#define ANT
typedef struct _ant{
    int  x, y, theta, initial_theta;
    bool found_food, intruder_detected;
    int home_sick, alarm_max; 
    int lost;
    bool alive;
    
}Ant;
#endif

#ifndef COLONY
#define COLONY
typedef struct _colony{
    std::vector<Ant> ants;
    char ant_position[900][900];
    unsigned char pheromones[900][900][3];
    bool draw_phero;
    float nest_x, nest_y;
    int ants_amount;
    int soldiers_amount;

    int home_sick_max;
    int food_found_amount;

    unsigned char alarm_phero_amount;
    unsigned char food_phero_amount;
    unsigned char path_phero_amount;

    unsigned short int decay_timer, decay_timer_max;
    unsigned char decay_amount;
}Colony;
#endif

#ifndef FOOD
#define FOOD
typedef struct _food{
    int x,y,amount;
}Food;
#endif


Colony create_colony(float x, float y, int amount, int soldiers_amount);
void reset_colony(Colony *colony);
void process_colony(Colony *colony, char enemy_location[900][900], Food* food);
void create_food_map(Food* food);
void process_food(Food *food);
void toggle_food(Food *food);


