#include <iostream>
#include "cell.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <ctime>
#include <cmath>

GLFWwindow* window;

const double frame_length = 0.05; //frame length in seconds

const int board_size = 250;
const float drawn_board_size = 1.8f;

const int min_strenght = 5, max_strenght = 50; //bounds for first gen randomizer
const int min_max_age = 10, max_max_age = 100; //

clock_t time1 = clock();
clock_t time2 = time1;
double time_counter = frame_length * CLOCKS_PER_SEC;

const float drawn_cell_size = drawn_board_size / board_size;
const int min_colony_size = board_size / 20;
const int max_colony_size = board_size / 5;


const int amount_of_colonies = 8;

int color_step = cbrt(amount_of_colonies) + 1;

cell board[board_size + 2][board_size + 2];

cell empty_c;

void draw_square(float x, float y, float a, float r, float g, float b)
{
    glBegin(GL_QUADS);
    glColor3f(r, g, b);
    glVertex2f(x-a/2, y-a/2);
    glVertex2f(x+a/2, y-a/2);
    glVertex2f(x+a/2, y+a/2);
    glVertex2f(x-a/2, y+a/2);
    glEnd();
}
void draw_board()
{
    for(int i = 1; i <= board_size; i ++)
        for(int j = 1; j <= board_size; j ++)
        {

            draw_square((j - board_size/2.0f) * drawn_cell_size, (i - board_size/2) * drawn_cell_size, drawn_cell_size, board[i][j].red, board[i][j].green, board[i][j].blue);
        }

}
void make_colony(int colony_counter)
{
    int colony_size = rand() % (max_colony_size - min_colony_size + 1) + min_colony_size;
    int x = rand() % (board_size - colony_size);
    int y = rand() % (board_size - colony_size);

    int should_overwrite = 0;

    float r = (float)(colony_counter % color_step) / color_step * 0.75f + 0.25f;
    float g = (float)(colony_counter/color_step % color_step) / color_step * 0.75f + 0.25f;
    float b = (float)(colony_counter/color_step/color_step % color_step ) / color_step * 0.75f + 0.25f;

    float strenght = rand() % (max_strenght - min_strenght + 1) + min_strenght;

    float fight_odds = rand() % (100 - 5) + 5;
    float travel_odds = rand() % (100 - 5) + 5;
    float reproduction_odds = rand() % (100 - 5) + 5;

    int max_age = rand() % (max_max_age - min_max_age + 1) + min_max_age;

    for(int i = y; i <= colony_size + y; i++)
        for(int j = x; j <= colony_size + x; j++)
        {
            if(board[i][j].is_taken)
                should_overwrite = (should_overwrite + ((i + j)% (int)(sqrt(amount_of_colonies)+1))) % (int)(sqrt(amount_of_colonies) + 1);
            if((board[i][j].is_taken && should_overwrite == 0) || !board[i][j].is_taken)
            {
                board[i][j].red = r;
                board[i][j].green = g;
                board[i][j].blue = b;
                board[i][j].is_taken = true;
                board[i][j].fight_prio = 3 * fight_odds;
                board[i][j].travel_prio = travel_odds;
                board[i][j].reproduction_prio = reproduction_odds;
                board[i][j].max_age = max_age;
                board[i][j].strength = strenght;
            }

        }

}
void loop_func()
{
    cell* tab[8];
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    draw_board();
    glfwSwapBuffers(window);
    glfwPollEvents();

    time2 = clock();
    time_counter += (double)(time2-time1);
    time1 = time2;

    if(time_counter > frame_length * CLOCKS_PER_SEC)
    {
        time_counter -= frame_length * CLOCKS_PER_SEC;
        for(int i = 1; i <= board_size; i ++)
            for(int j = 1; j <= board_size; j ++)
            {
                tab[0] = &board[i+1][j];
                tab[1] = &board[i+1][j+1];
                tab[2] = &board[i][j+1];
                tab[3] = &board[i-1][j+1];
                tab[4] = &board[i-1][j];
                tab[5] = &board[i-1][j-1];
                tab[6] = &board[i][j-1];
                tab[7] = &board[i+1][j-1];
                if(board[i][j].is_taken)
                {
                    //std::cout << board[i][j].age << ' ' << board[i][j].max_age << '\n';
                    board[i][j].take_action(tab);
                    board[i][j].age += 1;
                }

                if(board[i][j].age >= board[i][j].max_age && board[i][j].is_taken)
                {
                    //std::cout<<"I died of aging" << '\n';
                    board[i][j] = empty_c;
                }
            }
        for(int i = 1; i <= board_size; i ++)
            for(int j = 1; j <= board_size; j ++)
                board[i][j].did_anything = false;
    }
}
int main()
{

    const int windowSize = 960;

    std::srand(time(NULL));

	if (!glfwInit())
	{
        return -1;
	}

    glfwInitHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

	window = glfwCreateWindow(windowSize, windowSize, "some cells lol", NULL, NULL);
	glfwSetWindowPos(window, (glfwGetVideoMode(glfwGetPrimaryMonitor())->width - windowSize)/2, (glfwGetVideoMode(glfwGetPrimaryMonitor())->height - windowSize)/2);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

	glfwSwapInterval(1);
	glfwMakeContextCurrent(window);

    for(int i = 0; i < board_size + 2; i++) //draws border around board so no cells can go outside of it
    {
        board[i][0].is_border = true;
        board[i][board_size + 1].is_border = true;
        board[0][i].is_border = true;
        board[board_size + 1][i].is_border = true;

        board[i][0].is_taken = true;
        board[i][board_size + 1].is_taken = true;
        board[0][i].is_taken = true;
        board[board_size + 1][i].is_taken = true;
    }


    for(int i = 0; i < amount_of_colonies; i++)
        make_colony(i);



    while (!glfwWindowShouldClose(window))
    {
            loop_func();
    }

	glfwTerminate();








	return 0;
}
