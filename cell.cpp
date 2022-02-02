#include <ctime>
#include<iostream>
#include <cstdlib>
#include "cell.h"
#include <vector>
cell::cell()
{
    red = 1.0f;
    green = 1.0f;
    blue = 1.0f;

    is_taken = false;
    is_border = false;          //is border or the board, never can be taken

    did_anything = false;       //true if cell fought, traveled or bred in current turn

    strength = 0.0f;            //increases by 2% of enemies strength after won fight

    reproduction_prio = 0;   //any
    fight_prio = 0;          //+value
    travel_prio = 0;         //is ok

    age = 0;                    //increases once every turn or twice while fighting
    max_age = 0;                //is constant

    mutation_chance = 0.1f;       //0.0f - 1.0f       mutates by 1%
}
cell empty_cell;
void cell::multiply(cell* partner, cell* target)
{
    int mutation_index = rand() % 5;
    //std::cout << "I bred" << '\n';
    did_anything = true;
    partner -> did_anything = true;
    target -> is_taken = true;

    if(rand() % 2)
        target -> strength = strength;
    else
        target -> strength = partner -> strength;

    if(rand() % 2)
        target -> max_age = max_age;
    else
        target -> max_age = partner -> max_age;

    if(rand()%1000000 <= 1000000 * (mutation_chance + partner -> mutation_chance) / 2)
    {
        switch(mutation_index)
        {
        case 0:
        target -> strength += target -> strength * (rand() % 10 - 5) / 100;
        break;

        case 1:
        target -> max_age += target -> max_age * (rand() % 10 - 5) / 100;
        break;

        case 2:
        target -> reproduction_prio += target -> reproduction_prio * (rand() % 5 - 3) / 100;
        break;

        case 3:
        target -> fight_prio += target -> fight_prio * (rand() % 10 - 5) / 100;
        break;

        case 4:
        target -> travel_prio += target -> travel_prio * (rand() % 10 - 5) / 100;
        break;
        }
        mutation_chance += mutation_chance * (rand() % 10) / 100;
    }
    target -> reproduction_prio = (reproduction_prio + partner -> reproduction_prio) / 2;
    target -> fight_prio = (fight_prio + partner -> fight_prio) / 2;
    target -> travel_prio = (travel_prio + partner -> travel_prio) / 2;

    target -> age = 0;

    target -> did_anything = true;

    target -> red = red;
    target -> green = green;
    target -> blue = blue;
}
void cell::fight(cell* enemy)
{
    //std::cout << "I fought" << '\n';
    cell empty_cell;
    if(enemy -> strength > strength)
    {
        enemy -> strength += strength * 0.05f;
        enemy -> age++;
        *this = empty_cell;
    }
    else if(enemy -> strength < strength)
    {
        strength += enemy -> strength * 0.05f;
        *enemy = *this;
        *this = empty_cell;
        age ++;
    }
    else
    {
        *this = empty_cell;
        *enemy = empty_cell;
    }


}
void cell::travel(cell* target )
{
    *target = *this;
    *this = empty_cell;
}

void cell::take_action(cell* neighbours[8])
{
    if(did_anything)
        return;
    else
        did_anything = true;
    std::vector<int>enemies;
    std::vector<int>allies;
    std::vector<int>none;
    int rand_num;
    for(int i = 0; i < 8; i++)
    {
        if(!neighbours[i]->is_taken)
        {
            none.push_back(i);
            //std::cout << "none ";
        }

        else if(neighbours[i]->red == red && neighbours[i]->green == green && neighbours[i]->blue == blue)
        {
            allies.push_back(i);
            //std::cout << "ally ";
        }

        else if(!neighbours[i]->is_border)
        {
            enemies.push_back(i);
            //std::cout << "enemy ";
        }

    }
    //std::cout << '\n';
    if(!none.empty())
    {
        if(!allies.empty())
        {
            if(!enemies.empty())
            {
                rand_num = rand() % (reproduction_prio + fight_prio + travel_prio);
                if(rand_num > 0 && rand_num <= travel_prio)
                    travel(neighbours[none[rand() % none.size()]]);
                else if((rand_num > travel_prio && rand_num <= (fight_prio + travel_prio)))
                    fight(neighbours[enemies[rand() % enemies.size()]]);
                else
                    multiply(neighbours[allies[rand() % allies.size()]], neighbours[none[rand() % none.size()]]);
            }
            else
            {
                rand_num = rand() % (reproduction_prio + travel_prio) + 1;
                if(rand_num > 0 && rand_num <= travel_prio)
                    travel(neighbours[none[rand() % none.size()]]);
                else
                {
                    multiply(neighbours[allies[rand() % allies.size()]], neighbours[none[rand() % none.size()]]);
                }

            }
        }
        else if(!enemies.empty())
        {
            rand_num = rand() % (fight_prio + travel_prio) + 1;
            if(rand_num > 0 && rand_num <= travel_prio)
                travel(neighbours[none[rand() % none.size()]]);
            else
                fight(neighbours[enemies[rand() % enemies.size()]]);
        }
        else
            travel(neighbours[none[rand() % none.size()]]);
    }
    else if(!enemies.empty())
        fight(neighbours[enemies[rand() % enemies.size()]]);
}
