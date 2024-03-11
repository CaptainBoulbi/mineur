#include "raylib.h"

static inline int min(int a, int b) {
    return a < b ? a : b;
}

int screen_width = 500;
int screen_height = 550;

Rectangle menu;
int grid_len = 0;
int nb_cell = 16;

Rectangle grid = {0};

Image mine_image_orig = {0};
Texture mine_texture = {0};

void screen_resize_handle(void) {
    screen_width = GetScreenWidth();
    screen_height = GetScreenHeight();

    menu.width = screen_width;
    grid_len = min(screen_width, screen_height - menu.height);
    grid = (Rectangle){(screen_width - grid_len) / 2,
                        screen_height - grid_len,
                        grid_len, grid_len};

    Image mine_copy = ImageCopy(mine_image_orig);
    ImageResize(&mine_copy, grid.width/nb_cell, grid.height/nb_cell);
    mine_texture = LoadTextureFromImage(mine_copy);
}

int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    menu = (Rectangle){0, 0, screen_width, screen_height - screen_width};
    mine_image_orig = LoadImage("ressources/mine.png");

    InitWindow(screen_width, screen_height, "mineur");

    screen_resize_handle();

    while (!WindowShouldClose()) {
        if (IsWindowResized())
            screen_resize_handle();

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawRectangleRec(menu, RED);
            DrawRectangleRec(grid, GRAY);

            for (int x=0; x<nb_cell; x++) {
                for (int y=0; y<nb_cell; y++) {
                    DrawTexture(mine_texture,
                                grid.x + grid.width/nb_cell * x,
                                grid.y + grid.height/nb_cell * y,
                                WHITE);
                }
            }
        }
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
