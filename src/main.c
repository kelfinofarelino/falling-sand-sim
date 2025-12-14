#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> // Gunakan <windows.h> dan Sleep() jika di Windows

// --- KONFIGURASI ---
#define HEIGHT 25
#define WIDTH 60
#define FPS 30
#define FRAME_DELAY (1000000 / FPS)

// Tipe Partikel
enum ParticleType {
    EMPTY = 0,
    WALL = 1,
    SAND = 2
};

// Grid dunia
int grid[HEIGHT][WIDTH];

// Buffer untuk rendering cepat (mengurangi syscall printf berulang)
char render_buffer[HEIGHT * (WIDTH + 1) * 20]; // Alokasi size lebih untuk ANSI codes

// --- FUNGSI UTAMA ---

void clear_screen() {
    printf("\033[H"); // Pindahkan kursor ke pojok kiri atas (Home), tidak clear seluruhnya agar smooth
}

void init_world() {
    memset(grid, EMPTY, sizeof(grid));
    
    // Buat lantai/wadah di bawah
    for (int x = 0; x < WIDTH; x++) grid[HEIGHT-1][x] = WALL;
    
    // Buat beberapa rintangan di tengah (platform)
    for (int x = 20; x < 40; x++) grid[15][x] = WALL;
    for (int x = 10; x < 25; x++) grid[8][x] = WALL;
}

// Logika inti: Update Fisika
void update_physics() {
    // PENTING: Loop dari BAWAH ke ATAS (y--)
    // Agar partikel yang jatuh tidak diproses ulang di posisi barunya dalam frame yang sama
    for (int y = HEIGHT - 2; y >= 0; y--) {
        // Acak arah iterasi horizontal agar tumpukan pasir terlihat natural (tidak miring ke satu sisi)
        // Kita bisa iterasi biasa, tapi untuk efek 'longsor' yang realistik, kita cek kiri/kanan secara dinamis
        
        for (int x = 0; x < WIDTH; x++) {
            if (grid[y][x] == SAND) {
                // 1. Cek bawah langsung
                if (grid[y+1][x] == EMPTY) {
                    grid[y+1][x] = SAND;
                    grid[y][x] = EMPTY;
                }
                // 2. Fisika Longsor: Cek bawah-kiri (Diagonal)
                else if (x > 0 && grid[y+1][x-1] == EMPTY) {
                    grid[y+1][x-1] = SAND;
                    grid[y][x] = EMPTY;
                }
                // 3. Fisika Longsor: Cek bawah-kanan (Diagonal)
                else if (x < WIDTH - 1 && grid[y+1][x+1] == EMPTY) {
                    grid[y+1][x+1] = SAND;
                    grid[y][x] = EMPTY;
                }
                // Jika semua penuh, pasir diam (stacking)
            }
        }
    }
}

// Spawn pasir baru di titik random di atas
void spawn_sand() {
    int spawn_x = (rand() % (WIDTH - 4)) + 2; // Random posisi X
    if (grid[0][spawn_x] == EMPTY) {
        grid[0][spawn_x] = SAND;
    }
    // Spawn lebih banyak pasir biar cepat menumpuk
    if (grid[0][spawn_x+1] == EMPTY) grid[0][spawn_x+1] = SAND;
}

// Rendering Efisien dengan Single Buffer
void draw_world() {
    char *p = render_buffer;
    
    // Header
    p += sprintf(p, "FALLING SAND SIMULATION (Ctrl+C to Quit)\n");
    p += sprintf(p, "========================================\n");

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (grid[y][x] == WALL) {
                // Abu-abu untuk tembok
                p += sprintf(p, "\033[1;30m#\033[0m"); 
            } else if (grid[y][x] == SAND) {
                // Kuning terang untuk pasir
                p += sprintf(p, "\033[1;33m.\033[0m");
            } else {
                p += sprintf(p, " ");
            }
        }
        p += sprintf(p, "\n"); // Newline per baris
    }
    
    // Cetak seluruh frame sekaligus (Sangat cepat!)
    printf("%s", render_buffer);
}

int main() {
    srand(time(NULL));
    init_world();
    
    // Clear layar sekali di awal dengan 'J' untuk menghapus residu scroll
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