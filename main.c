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

typedef enum GameState {
    PLAYING,
    WIN,
    LOSE,
} GameState;

GameState game_state = PLAYING;

int game_cap = 16*30;
char game[16][30] = {0};
char discover[16][30] = {0};
char zero[16][30] = {0};

typedef enum GameType {
    BEGINNER,
    INTERMEDIATE,
    EXPERT,
} GameType;

GameType current_game_type = BEGINNER;

typedef struct Vec2i {
    int x;
    int y;
} Vec2i;

Vec2i game_size;
int nb_bomb;
int nb_bomb_pad = 9;
char *nb_bomb_text;

int screen_width = 500;
int screen_height = 550;

Rectangle menu;
int grid_len = 0;

Rectangle grid = {0};

const Color text_color = RED;

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

Texture camera_texture;
Texture lose_texture;
Texture win_texture;
Texture playing_texture;
Texture fixed_tile_texture;
Texture double_tile_texture;
Texture triple_tile_texture;

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

void fill_game(void)
{
    switch (current_game_type) {
    case BEGINNER:
        game_size.x = 9;
        game_size.y = 9;
        nb_bomb = 10;
        nb_bomb_pad = 13;
        nb_bomb_text = "10";
        break;
    case INTERMEDIATE:
        game_size.x = 16;
        game_size.y = 16;
        nb_bomb = 40;
        nb_bomb_pad = 9;
        nb_bomb_text = "40";
        break;
    case EXPERT:
        game_size.x = 16;
        game_size.y = 30;
        nb_bomb = 99;
        nb_bomb_pad = 9;
        nb_bomb_text = "99";
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

void discover_empty_cell(int x, int y)
{
    int count = 0;
    for (int i=-1; i<=1; i++) {
        if (x+i < 0 || x+i > game_size.x)
            continue;
        for (int j=-1; j<=1; j++) {
            if (y+j < 0 || y+j > game_size.y)
                continue;
            if (game[y+j][x+i] == '0' && discover[y+j][x+i])
                count++;
        }
    }
    if (count > 0)
        discover[y][x] = 1;
}

void zero_click(int x, int y)
{
    if (x < 0 || x >= game_size.x ||
        y < 0 || y >= game_size.y)
        return;

    if (zero[y][x])
        return;

    discover[y][x] = 1;
    zero[y][x] = 1;

    if (game[y][x] != '0')
        return;

    for (int i=-1; i<=1; i++) {
        if (x+i < 0 || x+i >= game_size.x)
            continue;
        for (int j=-1; j<=1; j++) {
            if (y+j < 0 || y+j >= game_size.y)
                continue;
            zero_click(x+i, y+j);
        }
    }
}

int collision(Vec2i coord, Texture text, int x, int y)
{
    return
        x > coord.x && x < coord.x + text.width &&
        y > coord.y && y < coord.y + text.height;
}

void reload_game(void)
{
    game_state = PLAYING;
    memset(game, 0, game_cap);
    memset(discover, 0, game_cap);
    memset(zero, 0, game_cap);
    fill_game();
    screen_resize_handle();
}

int main(void)
{
    srand(time(NULL));

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    // SetTraceLogLevel(LOG_ERROR);

    InitWindow(screen_width, screen_height, "mineur");
    SetTargetFPS(60);

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

    camera_texture = LoadTexture("ressources/camera.png");
    lose_texture = LoadTexture("ressources/lose.png");
    win_texture = LoadTexture("ressources/win.png");
    playing_texture = LoadTexture("ressources/playing.png");
    fixed_tile_texture = LoadTexture("ressources/fixed_tile.png");

    screen_resize_handle();

    fill_game();

    int take_screenshot = 0;
    while (!WindowShouldClose()) {
        if (IsWindowResized())
            screen_resize_handle();

        if (take_screenshot) {
            TakeScreenshot("minesweeper-screenshot.png");
            take_screenshot = 0;
        }

        BeginDrawing();
        {
            ClearBackground((Color) {
                .r = 0x8E, .g = 0x8E, .b = 0x8E, .a = 255
            });


            if (IsKeyPressed(KEY_A)) {
                CloseWindow();
                exit(0);
            }

            if (IsKeyPressed(KEY_S)) {
                take_screenshot = 1;
            }

            int resize = 0;
            if (IsKeyPressed(KEY_I)) {
                current_game_type = INTERMEDIATE;
                resize = 1;
            }
            if (IsKeyPressed(KEY_B)) {
                current_game_type = BEGINNER;
                resize = 1;
            }
            if (IsKeyPressed(KEY_E)) {
                current_game_type = EXPERT;
                resize = 1;
            }
            if (resize || IsKeyPressed(KEY_R)) {
                reload_game();
                resize = 0;
            }

            Color menu_color;
            Texture *menu_smiley;
            switch (game_state) {
            case LOSE:
                menu_color = RED;
                menu_smiley = &lose_texture;
                break;
            case WIN:
                menu_color = GREEN;
                menu_smiley = &win_texture;
                break;
            case PLAYING:
                menu_color = YELLOW;
                menu_smiley = &playing_texture;
                break;
            }

            DrawRectangleRec(menu, menu_color);

            Vec2i smiley_coord = {menu.width/2 - menu_smiley->width/2, 0};
            DrawTexture( *menu_smiley, smiley_coord.x, smiley_coord.y, WHITE);

            Vec2i camera_coord = {
                menu.width/2 - menu_smiley->width/2 - camera_texture.width -10,
                menu.height/2 - camera_texture.height/2,
            };
            DrawTexture(
                camera_texture,
                camera_coord.x, camera_coord.y,
                WHITE
            );
            DrawTexture(
                fixed_tile_texture,
                menu.width - fixed_tile_texture.width,
                0,
                WHITE
            );
            DrawText(nb_bomb_text,
                menu.width - fixed_tile_texture.width + nb_bomb_pad,
                13, 30, text_color
            );

            int diff_pad = 10;
            Vec2i diff_button[3] = {0};
            char *diff_name[3] = {"BEGINNER", "INTERMEDIATE", "EXPERT"};
            for (int i=0; i<3; i++) {
                diff_button[i] = (Vec2i) {
                    fixed_tile_texture.width*i + diff_pad*i, 0
                };
                DrawTexture(
                    fixed_tile_texture, // TODO: double_fixed_tile_texture
                    diff_button[i].x, diff_button[i].y,
                    WHITE
                );
                DrawText(diff_name[i],
                    diff_button[i].x + 9, diff_button[i].y + 15,
                    20, text_color
                );
            }

            DrawRectangleRec(grid, (Color) {
                .r = 0xC6, .g = 0xC6, .b = 0xC6, .a = 255
            });

            int timer_pad = 10;
            int timer_mid = (screen_width * 3) / 4;
            DrawTexture(
                fixed_tile_texture, // TODO: double_fixed_tile_texture
                timer_mid - timer_pad - fixed_tile_texture.width, 0,
                WHITE
            );
            DrawTexture(
                fixed_tile_texture, // TODO: double_fixed_tile_texture
                timer_mid + timer_pad, 0,
                WHITE
            );

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
                        grid.x + grid.width/game_size.x * x,
                        grid.y + grid.height/game_size.y * y,
                        WHITE
                    );
                }
            }

            int mouse_x = GetMouseX();
            int mouse_y = GetMouseY();
            int grid_x = (int) grid.x;
            int grid_y = (int) grid.y;
            int mouse_in_grid = 0;
            int mouse_in_menu = 0;

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (grid_x < mouse_x && mouse_x < grid_x + grid.width &&
                    grid_y < mouse_y && mouse_y < grid_y + grid.height)
                {
                    mouse_in_grid = 1;
                } else if (0 < mouse_x && mouse_x < menu.width &&
                           0 < mouse_y && mouse_y < menu.height)
                {
                    mouse_in_menu = 1;
                }
            }

            if (mouse_in_grid && game_state == PLAYING) {
                mouse_x = ((float)(mouse_x - grid_x) / grid_len) * game_size.x;
                mouse_y = ((float)(mouse_y - grid_y) / grid_len) * game_size.y;

                discover[mouse_y][mouse_x] = 1;

                zero_click(mouse_x, mouse_y);

                int count_undiscovered_cell = 0;
                for (int x=0; x<game_size.x; x++) {
                    for (int y=0; y<game_size.y; y++) {
                        if (!discover[y][x])
                            count_undiscovered_cell++;
                    }
                }

                if (game_state != LOSE && count_undiscovered_cell <= nb_bomb)
                    game_state = WIN;

                if (game[mouse_y][mouse_x] == 'X') {
                    memset(discover, 1, game_cap);
                    game_state = LOSE;
                }
            } else if (mouse_in_menu) {
                if (collision(smiley_coord, playing_texture,
                              mouse_x, mouse_y))
                {
                    reload_game();
                }
                if (collision(camera_coord, camera_texture,
                              mouse_x, mouse_y))
                {
                    take_screenshot = 1;
                }
                if (collision(diff_button[0], fixed_tile_texture,
                              mouse_x, mouse_y))
                {
                    current_game_type = BEGINNER;
                    reload_game();
                }
                if (collision(diff_button[1], fixed_tile_texture,
                              mouse_x, mouse_y))
                {
                    current_game_type = INTERMEDIATE;
                    reload_game();
                }
                if (collision(diff_button[2], fixed_tile_texture,
                              mouse_x, mouse_y))
                {
                    current_game_type = EXPERT;
                    reload_game();
                }
            }
        }
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
