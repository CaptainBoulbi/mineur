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
char discover[16][30] = {0};

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
int grid_len = 0;

Rectangle grid = {0};

#define INIT_TEXTURE(img, tex) \
    Image img = {0}; \
    Texture tex = {0}

INIT_TEXTURE(tile_image_orig, tile_texture);
INIT_TEXTURE(tile_hover_image_orig, tile_hover_texture);
INIT_TEXTURE(mine_image_orig, mine_texture);
INIT_TEXTURE(t1_image_orig, t1_texture);
INIT_TEXTURE(t2_image_orig, t2_texture);
INIT_TEXTURE(t3_image_orig, t3_texture);
INIT_TEXTURE(t4_image_orig, t4_texture);
INIT_TEXTURE(t5_image_orig, t5_texture);
INIT_TEXTURE(t6_image_orig, t6_texture);
INIT_TEXTURE(t7_image_orig, t7_texture);
INIT_TEXTURE(t8_image_orig, t8_texture);

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

#define RESIZE_TEXTURE(name, copy, orig) \
    do { \
        Image copy = ImageCopy(orig); \
        ImageResize(&(copy), grid.width/game_size.x, grid.height/game_size.y);\
        name = LoadTextureFromImage(copy); \
    } while (0)

    RESIZE_TEXTURE(tile_texture, tile_copy, tile_image_orig);
    RESIZE_TEXTURE(tile_hover_texture, tile_hover_copy, tile_hover_image_orig);
    RESIZE_TEXTURE(mine_texture, mine_copy, mine_image_orig);
    RESIZE_TEXTURE(t1_texture, t1_copy, t1_image_orig);
    RESIZE_TEXTURE(t2_texture, t2_copy, t2_image_orig);
    RESIZE_TEXTURE(t3_texture, t3_copy, t3_image_orig);
    RESIZE_TEXTURE(t4_texture, t4_copy, t4_image_orig);
    RESIZE_TEXTURE(t5_texture, t5_copy, t5_image_orig);
    RESIZE_TEXTURE(t6_texture, t6_copy, t6_image_orig);
    RESIZE_TEXTURE(t7_texture, t7_copy, t7_image_orig);
    RESIZE_TEXTURE(t8_texture, t8_copy, t8_image_orig);
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

void fill_game(GameType type)
{
    memset(game, 0, 16*30);
    memset(discover, 0, 16*30);

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

int main(void)
{
    srand(time(NULL));

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    menu = (Rectangle){0, 0, screen_width, screen_height - screen_width};
    tile_image_orig = LoadImage("ressources/tile.png");
    tile_hover_image_orig = LoadImage("ressources/tile_hover.png");
    mine_image_orig = LoadImage("ressources/mine.png");
    mine_image_orig = LoadImage("ressources/mine.png");
    t1_image_orig = LoadImage("ressources/1.png");
    t2_image_orig = LoadImage("ressources/2.png");
    t3_image_orig = LoadImage("ressources/3.png");
    t4_image_orig = LoadImage("ressources/4.png");
    t5_image_orig = LoadImage("ressources/5.png");
    t6_image_orig = LoadImage("ressources/6.png");
    t7_image_orig = LoadImage("ressources/7.png");
    t8_image_orig = LoadImage("ressources/8.png");

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
            if (IsKeyPressed(KEY_P)) {
                for (int x=0; x<game_size.x; x++) {
                    for (int y=0; y<game_size.y; y++) {
                        printf("%c", game[y][x]);
                    }
                    printf("\n");
                }
            }

            DrawRectangleRec(menu, RED);
            DrawRectangleRec(grid, (Color) {
                .r = 255, .g = 240, .b = 172, .a = 255
            });

            for (int x=0; x<game_size.x; x++) {
                for (int y=0; y<game_size.y; y++) {
                    Texture *tex;
                    if (discover[y][x]) {
                        switch (game[y][x]) {
                            default:
                            case 'X': tex = &mine_texture; break;
                            case '0': continue;
                            case '1': tex = &t1_texture; break;
                            case '2': tex = &t2_texture; break;
                            case '3': tex = &t3_texture; break;
                            case '4': tex = &t4_texture; break;
                            case '5': tex = &t5_texture; break;
                            case '6': tex = &t6_texture; break;
                            case '7': tex = &t7_texture; break;
                            case '8': tex = &t8_texture; break;
                        }
                    } else {
                        tex = &tile_texture;
                    }

                    DrawTexture(
                        *tex,
                        grid.x + grid.width/ game_size.x * x,
                        grid.y + grid.height/game_size.y * y,
                        WHITE
                    );
                }
            }

            int mouse_x = GetMouseX();
            int mouse_y = GetMouseY();
            int grid_x = (int) grid.x;
            int grid_y = (int) grid.y;

            // boundary
            mouse_x = max(mouse_x, grid_x);
            mouse_x = min(mouse_x, grid_x + grid.width - (grid_len/game_size.x));
            mouse_y = max(mouse_y, grid_y);
            mouse_y = min(mouse_y, grid_y + grid.height - (grid_len/game_size.y));

            mouse_x = ((float) (mouse_x - grid_x) / grid_len) * game_size.x;
            mouse_y = ((float) (mouse_y - grid_y) / grid_len) * game_size.y;

            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                discover[mouse_y][mouse_x] = 1;
            }
        }
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
