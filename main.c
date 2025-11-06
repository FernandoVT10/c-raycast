#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <float.h>

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

typedef struct {
    Vector2 start;
    Vector2 end;
} LineSegment;

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

bool ray_line_segment_collision(MyRay ray, LineSegment line, Vector2 *resPoint) {
    // solves equation
    // R(t) = L(s)
    //
    // where:
    // R(t) = (x0 + t * dx, y0 + t * dy)
    // x0 = x coordinate of the ray's origin
    // dx = ray's x direction
    // y0 = y coordinate of the ray's origin
    // dy = ray's y direction
    //
    // L(s) = A + s(B - A)
    // A and B = start and end point of a line segment
    //
    // here R(t) represents a point in a ray, and L(s) represents a point in a line segment
    // so, we want to find a t and s where R(t) = L(s), a collision is found when:
    // 0 <= s <= 1: since if s is out of this range, L(s) is outside of the line segment
    // t >= 0: if t is negative, the point was found opposite to the ray's direction

    float dx = line.end.x - line.start.x;
    float dy = line.end.y - line.start.y;

    float s = ray.dir.x * (line.start.y - ray.origin.y) - ray.dir.y * (line.start.x - ray.origin.x);
    s /= dx * ray.dir.y - dy * ray.dir.x;

    float t = (line.start.x - ray.origin.x) + s * dx;
    t /= ray.dir.x;

    bool collision = s >= 0 && s <= 1 && t > 0;

    if(collision) {
        resPoint->x = ray.origin.x + ray.dir.x * t;
        resPoint->y = ray.origin.y + ray.dir.y * t;
    }

    return collision;
}

typedef struct {
    bool hit;
    Vector2 point;
} MyRayCollision;

MyRayCollision get_ray_collision_rec(MyRay ray, Rectangle rec) {
    float t = 0;
    float s = FLT_MAX;

    if(ray.dir.x != 0) {
        // left line
        t = (rec.x - ray.origin.x) / ray.dir.x;
        float y = ray.origin.y + t * ray.dir.y;

        if(t > 0 && y >= rec.y && y <= rec.y + rec.height && t < s) {
            s = t;
        }

        t += rec.width / ray.dir.x;
        y = ray.origin.y + t * ray.dir.y;

        if(t > 0 && y >= rec.y && y <= rec.y + rec.height && t < s) {
            s = t;
        }
    }

    if(ray.dir.y != 0) {
        t = (rec.y - ray.origin.y) / ray.dir.y;
        float x = ray.origin.x + t * ray.dir.x;

        if(t > 0 && x >= rec.x && x <= rec.x + rec.width && t < s) {
            s = t;
        }

        t += rec.height / ray.dir.y;
        x = ray.origin.x + t * ray.dir.x;
        if(t > 0 && x >= rec.x && x <= rec.x + rec.width && t < s) {
            s = t;
        }
    }

    Vector2 point = {
        .x = ray.origin.x + s * ray.dir.x,
        .y = ray.origin.y + s * ray.dir.y,
    };

    return (MyRayCollision){
        .hit = s != FLT_MAX,
        .point = point,
    };
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

    Rectangle rec = {100, 100, 50, 50};

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

        LineSegment line = {
            .start = {800, 200},
            .end = {850, 250},
        };

        DrawLineEx(line.start, line.end, 2, WHITE);

        Vector2 collisionPoint = {0};
        if(ray_line_segment_collision(ray, line, &collisionPoint)) {
            ray.endPoint = collisionPoint;
        } else {
            float distance = 1000;
            ray.endPoint.x = ray.origin.x + ray.dir.x * distance;
            ray.endPoint.y = ray.origin.y + ray.dir.y * distance;
        }

        float speed = 10;
        if(IsKeyDown(KEY_RIGHT)) rec.x += speed;
        else if(IsKeyDown(KEY_LEFT)) rec.x -= speed;

        if(IsKeyDown(KEY_DOWN)) rec.y += speed;
        else if(IsKeyDown(KEY_UP)) rec.y -= speed;

        DrawRectangleLinesEx(rec, 2, RED);

        MyRayCollision coll = get_ray_collision_rec(ray, rec);

        if(coll.hit) {
            ray.endPoint = coll.point;
        }

        draw_ray(ray);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
