
#ifndef MAIN_H
#define MAIN_H

#define RENDER_HEIGHT 360
#define RENDER_WIDTH 640
#define RENDER_FOV 60
#define MINI_MAP_SIZE 64
#define SHADOW_STRENGTH 40

#define TEXTURE_SIZE 32
#define TEXTURE_COUNT 10

#define MOVE_DISTANCE 0.002
#define ROTATE_ANGLE 0.001
#define PLAYER_COLLISION_RADIUS 0.3


enum keyCodes {
    W = 119,
    A = 97,
    S = 115,
    D = 100,
};

struct KeyState {
    bool w = false;
    bool a = false;
    bool s = false;
    bool d = false;
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
