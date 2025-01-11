
#ifndef MAIN_H
#define MAIN_H

#define RENDER_HEIGHT 480
#define RENDER_WIDTH 640
#define RENDER_FOV 60
#define MOVE_DISTANCE 0.2

enum keyCodes {
    W = 119,
    A = 97,
    S = 115,
    D = 100,
};

struct v2D {
    double x;
    double y;
};

struct Player {
    v2D pos;
    v2D dir;
};

#endif //MAIN_H
