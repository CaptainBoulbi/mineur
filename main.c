#include "raylib.h"

static inline int min(int a, int b) {
    return a < b ? a : b;
}
static inline int max(int a, int b) {
    return a > b ? a : b;
}

int screen_width = 500;
int screen_height = 550;

Rectangle menu;
int grid_len = 0;
int nb_cell = 16;

Rectangle grid = {0};

Image tile_image_orig = {0};
Texture tile_texture = {0};
Image tile_hover_image_orig = {0};
Texture tile_hover_texture = {0};
Image mine_image_orig = {0};
Texture mine_texture = {0};

void screen_resize_handle(void) {
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
    ImageResize(&tile_copy, grid.width/nb_cell, grid.height/nb_cell);
    tile_texture = LoadTextureFromImage(tile_copy);

    Image tile_hover_copy = ImageCopy(tile_hover_image_orig);
    ImageResize(&tile_hover_copy, grid.width/nb_cell, grid.height/nb_cell);
    tile_hover_texture = LoadTextureFromImage(tile_hover_copy);

    Image mine_copy = ImageCopy(mine_image_orig);
    ImageResize(&mine_copy, grid.width/nb_cell, grid.height/nb_cell);
    mine_texture = LoadTextureFromImage(mine_copy);
}

int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    menu = (Rectangle){0, 0, screen_width, screen_height - screen_width};
    tile_image_orig = LoadImage("ressources/tile.png");
    tile_hover_image_orig = LoadImage("ressources/tile_hover.png");
    mine_image_orig = LoadImage("ressources/mine.png");

    InitWindow(screen_width, screen_height, "mineur");
    SetTargetFPS(60);

    screen_resize_handle();

    while (!WindowShouldClose()) {
        if (IsWindowResized())
            screen_resize_handle();

        BeginDrawing();
        {
            ClearBackground(BLACK);

            DrawRectangleRec(menu, RED);
            DrawRectangleRec(grid, (Color) {
                .r = 0xFF, .g = 0xDA, .b = 0x2E, .a = 255
            });

            for (int x=0; x<nb_cell; x++) {
                for (int y=0; y<nb_cell; y++) {
                    DrawTexture(
                        tile_texture,
                        grid.x + grid.width/nb_cell * x,
                        grid.y + grid.height/nb_cell * y,
                        WHITE
                    );
                }
            }

            int mouse_x = GetMouseX();
            int mouse_y = GetMouseY();
            int grid_x = (int) grid.x;
            int grid_y = (int) grid.y;

            mouse_x = max(mouse_x, grid_x);
            mouse_x = min(mouse_x, grid_x + grid.width - tile_texture.width);
            mouse_y = max(mouse_y, grid_y);
            mouse_y = min(mouse_y, grid_y + grid.height - tile_texture.height);

            mouse_x -= (mouse_x - grid_x) % tile_texture.width;
            mouse_y -= (mouse_y - grid_y) % tile_texture.height;

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
