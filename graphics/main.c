#include "raylib.h"

int main()
{
    InitWindow(800, 600, "My First Graphic Program");

    Vector2 shape[] = {
        {10, 10},
        {100, 80},
        {200, 200},
        {200, 350},
        {90, 0}
    };

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawLineStrip(shape, 5, RED);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}