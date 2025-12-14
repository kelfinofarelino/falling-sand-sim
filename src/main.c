#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#define HEIGHT 25
#define WIDTH 60
#define FPS 30
#define FRAME_DELAY (1000000 / FPS)

enum ParticleType {
    EMPTY = 0,
    WALL = 1,
    SAND = 2
};

int grid[HEIGHT][WIDTH];

char render_buffer[HEIGHT * (WIDTH + 1) * 20];


void clear_screen() {
    printf("\033[H");
}

void init_world() {
    memset(grid, EMPTY, sizeof(grid));
    
    for (int x = 0; x < WIDTH; x++) grid[HEIGHT-1][x] = WALL;
    
    for (int x = 20; x < 40; x++) grid[15][x] = WALL;
    for (int x = 10; x < 25; x++) grid[8][x] = WALL;
}

void update_physics() {
    for (int y = HEIGHT - 2; y >= 0; y--) {
        
        for (int x = 0; x < WIDTH; x++) {
            if (grid[y][x] == SAND) {
                if (grid[y+1][x] == EMPTY) {
                    grid[y+1][x] = SAND;
                    grid[y][x] = EMPTY;
                }
                else if (x > 0 && grid[y+1][x-1] == EMPTY) {
                    grid[y+1][x-1] = SAND;
                    grid[y][x] = EMPTY;
                }
                else if (x < WIDTH - 1 && grid[y+1][x+1] == EMPTY) {
                    grid[y+1][x+1] = SAND;
                    grid[y][x] = EMPTY;
                }
            }
        }
    }
}

void spawn_sand() {
    int spawn_x = (rand() % (WIDTH - 4)) + 2;
    if (grid[0][spawn_x] == EMPTY) {
        grid[0][spawn_x] = SAND;
    }
    if (grid[0][spawn_x+1] == EMPTY) grid[0][spawn_x+1] = SAND;
}

void draw_world() {
    char *p = render_buffer;
    
    p += sprintf(p, "FALLING SAND SIMULATION (Ctrl+C to Quit)\n");
    p += sprintf(p, "========================================\n");

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (grid[y][x] == WALL) {
                p += sprintf(p, "\033[1;30m#\033[0m"); 
            } else if (grid[y][x] == SAND) {
                p += sprintf(p, "\033[1;33m.\033[0m");
            } else {
                p += sprintf(p, " ");
            }
        }
        p += sprintf(p, "\n");
    }
    
    printf("%s", render_buffer);
}

int main() {
    srand(time(NULL));
    init_world();
    
    printf("\033[H\033[J");

    while (1) {
        spawn_sand();      // 1. Input/Spawner
        update_physics();  // 2. Logic Process
        clear_screen();    // 3. Reset Cursor
        draw_world();      // 4. Render
        
        usleep(FRAME_DELAY); // Atur kecepatan
    }

    return 0;
}