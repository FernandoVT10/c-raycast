#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "CCFuncs.h"
#include "raylib.h"
#include "raymath.h"

typedef struct {
    Vector2 origin;
    Vector2 dir;
    Vector2 endPoint; // used to draw it
} MyRay;

typedef struct {
    MyRay *items;
    size_t count;
    size_t capacity;
} MyRays;

void draw_ray(MyRay ray) {
    DrawLineEx(ray.origin, ray.endPoint, 2, YELLOW);
}

void draw_triangle(Vector2 p1, Vector2 p2, Vector2 p3) {
    DrawLineEx(p1, p2, 2, BLUE);
    DrawLineEx(p2, p3, 2, GREEN);
    DrawLineEx(p3, p1, 2, RED);
}

void draw_stats(float cos, float sin, float tan) {
    const char *cosText = TextFormat("cos = %f", cos);
    DrawText(cosText, 20, 20, 20, BLUE);

    const char *sinText = TextFormat("sin  = %f", sin);
    DrawText(sinText, 20, 40, 20, GREEN);

    const char *tanText = TextFormat("tan = %f", tan);
    DrawText(tanText, 20, 60, 20, RED);
}

int main(void) {
    InitWindow(1280, 720, "C Raycast");
    SetTargetFPS(60);

    Vector2 srcPos = {
        .x = GetScreenWidth() / 2,
        .y = GetScreenHeight() / 2,
    };

    MyRay ray = {
        .origin = srcPos,
    };

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        if(IsKeyPressed(KEY_F)) {
            ToggleFullscreen();
        }

        if(IsWindowResized()) {
            srcPos.x = GetScreenWidth() / 2;
            srcPos.y = GetScreenHeight() / 2;
            ray.origin = srcPos;
        }

        Vector2 mousePos = GetMousePosition();

        Vector2 p1 = srcPos;
        Vector2 p2 = {mousePos.x, srcPos.y};
        Vector2 p3 = mousePos;

        draw_triangle(p1, p2, p3);

        float adj = p2.x - p1.x;
        float opp = p3.y - p2.y;
        float hyp = hypotf(adj, opp);

        float cos = adj/hyp;
        float sin = opp/hyp;
        float tan = opp/adj;

        draw_stats(cos, sin, tan);

        ray.dir.x = cos;
        ray.dir.y = sin;

        // draw origin
        int srcRadius = 5;
        DrawCircle(srcPos.x, srcPos.y, srcRadius, WHITE);

        Vector2 lineStart = {800, 200};
        Vector2 lineEnd = {850, 250};
        DrawLineEx(lineStart, lineEnd, 2, WHITE);

        float dx = lineEnd.x - lineStart.x;
        float dy = lineEnd.y - lineStart.y;
        float originDX = (lineStart.x - ray.origin.x);

        float s = ray.dir.y * originDX - ray.dir.x * (lineStart.y - ray.origin.y);
        s /= ray.dir.x * dy - ray.dir.y * dx;

        float t = originDX + s * dx;
        t /= ray.dir.x;

        if(s >= 0 && s <= 1 && t > 0) {
            ray.endPoint.x = ray.origin.x + ray.dir.x * t;
            ray.endPoint.y = ray.origin.y + ray.dir.y * t;
        } else {
            float distance = 1000;
            ray.endPoint.x = ray.origin.x + ray.dir.x * distance;
            ray.endPoint.y = ray.origin.y + ray.dir.y * distance;
        }

        draw_ray(ray);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
