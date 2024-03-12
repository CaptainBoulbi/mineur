#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "raylib.h"

#define ARRAT(arr, type, x, y) ((arr) + (y)*(type) + (x))

static inline int min(int a, int b) {
    return a < b ? a : b;
}
static inline int max(int a, int b) {
    return a > b ? a : b;
}

char game[16][30] = {0};

typedef enum GameType {
    BEGINNER,
    INTERMEDIATE,
    EXPERT,
} GameType;

typedef struct Vec2i {
    int x;
    int y;
} Vec2i;

Vec2i game_size;
int nb_bomb;

int screen_width = 500;
int screen_height = 550;

Rectangle menu;
Vec2i nb_cell = {9, 9}; // 9-9 16-16 16-30
int grid_len = 0;

Rectangle grid = {0};

Image tile_image_orig = {0};
Texture tile_texture = {0};
Image tile_hover_image_orig = {0};
Texture tile_hover_texture = {0};
Image mine_image_orig = {0};
Texture mine_texture = {0};

void screen_resize_handle(void)
{
    screen_width = GetScreenWidth();
    screen_height = GetScreenHeight();

    menu.width = screen_width;
    grid_len = min(screen_width, screen_height - menu.height);
    grid = (Rectangle){
        (screen_width - grid_len) / 2,
        screen_height - grid_len,
        grid_len, grid_len
    };

    Image tile_copy = ImageCopy(tile_image_orig);
    ImageResize(&tile_copy, grid.width/nb_cell.x, grid.height/nb_cell.y);
    tile_texture = LoadTextureFromImage(tile_copy);

    Image tile_hover_copy = ImageCopy(tile_hover_image_orig);
    ImageResize(&tile_hover_copy, grid.width/nb_cell.x, grid.height/nb_cell.y);
    tile_hover_texture = LoadTextureFromImage(tile_hover_copy);

    Image mine_copy = ImageCopy(mine_image_orig);
    ImageResize(&mine_copy, grid.width/nb_cell.x, grid.height/nb_cell.y);
    mine_texture = LoadTextureFromImage(mine_copy);
}

void fill_game(GameType type)
{
    memset(game, 0, 16*30);

    switch (type) {
    case BEGINNER:
        game_size.x = 9;
        game_size.y = 9;
        nb_bomb = 10;
        break;
    case INTERMEDIATE:
        game_size.x = 16;
        game_size.y = 16;
        nb_bomb = 40;
        break;
    case EXPERT:
        game_size.x = 16;
        game_size.y = 30;
        nb_bomb = 99;
        break;
    }

    for (int i=0; i<nb_bomb; i++) {
        int x = rand() % game_size.x;
        int y = rand() % game_size.y;

        if (game[y][x] == 'X')
            i--;
        else
            game[y][x] = 'X';
    }

    for (int x=0; x<game_size.x; x++) {
        for (int y=0; y<game_size.y; y++) {
            if (game[y][x] == 'X')
                continue;

            int count = count_bomb(x, y);
            game[y][x] = count + 48;
        }
    }
}

int count_bomb(int x, int y)
{
    int count = 0;
    for (int i=-1; i<=1; i++) {
        if (x+i < 0 || x+i > game_size.x)
            continue;
        for (int j=-1; j<=1; j++) {
            if (y+j < 0 || y+j > game_size.y)
                continue;
            if (i == 0 && j == 0)
                continue;
            if (game[y+j][x+i] == 'X')
                count++;
        }
    }
    return count;
}

int main(void)
{
    srand(time(NULL));

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    menu = (Rectangle){0, 0, screen_width, screen_height - screen_width};
    tile_image_orig = LoadImage("ressources/tile.png");
    tile_hover_image_orig = LoadImage("ressources/tile_hover.png");
    mine_image_orig = LoadImage("ressources/mine.png");

    InitWindow(screen_width, screen_height, "mineur");
    SetTargetFPS(60);

    screen_resize_handle();

    fill_game(BEGINNER);

    while (!WindowShouldClose()) {
        if (IsWindowResized())
            screen_resize_handle();

        BeginDrawing();
        {
            ClearBackground(BLACK);

            if (IsKeyPressed(KEY_R)) {
                fill_game(BEGINNER);
            }

            DrawRectangleRec(menu, RED);
            DrawRectangleRec(grid, (Color) {
                .r = 0xFF, .g = 0xDA, .b = 0x2E, .a = 255
            });

            for (int x=0; x<nb_cell.x; x++) {
                for (int y=0; y<nb_cell.y; y++) {
                    switch (game[y][x]) {
                    case '0':
                        DrawTexture(
                            tile_texture,
                            grid.x + grid.width/nb_cell.x * x,
                            grid.y + grid.height/nb_cell.y * y,
                            WHITE
                        );
                        break;
                    case 'X':
                        DrawTexture(
                            mine_texture,
                            grid.x + grid.width/nb_cell.x * x,
                            grid.y + grid.height/nb_cell.y * y,
                            WHITE
                        );
                        break;
                    default:
                        DrawTexture(
                            tile_hover_texture,
                            grid.x + grid.width/nb_cell.x * x,
                            grid.y + grid.height/nb_cell.y * y,
                            WHITE
                        );
                        break;
                    }
                }
            }

            int mouse_x = GetMouseX();
            int mouse_y = GetMouseY();
            int grid_x = (int) grid.x;
            int grid_y = (int) grid.y;

            // boundary
            mouse_x = max(mouse_x, grid_x);
            mouse_x = min(mouse_x, grid_x + grid.width - (grid_len/nb_cell.x));
            mouse_y = max(mouse_y, grid_y);
            mouse_y = min(mouse_y, grid_y + grid.height - (grid_len/nb_cell.y));

            // remove extra
            mouse_x -= (mouse_x - grid_x) % (grid_len/nb_cell.x);
            mouse_y -= (mouse_y - grid_y) % (grid_len/nb_cell.y);

            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                DrawTexture(mine_texture, mouse_x, mouse_y, WHITE);
            } else {
                DrawTexture(tile_hover_texture, mouse_x, mouse_y, WHITE);
            }
        }
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
