
#ifndef MAIN_H
#define MAIN_H

#define RENDER_HEIGHT 480
#define RENDER_WIDTH 640
#define RENDER_FOV 60
#define MINI_MAP_SCALE 8
#define MOVE_DISTANCE 0.2
#define ROTATE_ANGLE 0.1

enum keyCodes {
    W = 119,
    A = 97,
    S = 115,
    D = 100,
};

struct vi2D {
    int x;
    int y;
};

struct vd2D {
    double x;
    double y;
};

struct Player {
    vd2D pos;
    vd2D dir;
};

#endif //MAIN_H
