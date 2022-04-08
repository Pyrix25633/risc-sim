#include "math.hpp"

using namespace std;

Vector2f::Vector2f() :x(0.0), y(0.0) {}

Vector2f::Vector2f(float px, float py) :x(px), y(py) {}

void Vector2f::print() {
    cout << x << ", " << y << endl;
}

bool Vector2f::operator == (HitBox2d hitbox) {
    return (x >= hitbox.x && x <= (hitbox.x + hitbox.w) && y >= hitbox.y && y <= (hitbox.y + hitbox.h));
}

bool Vector2f::operator != (HitBox2d hitbox) {
    return !(*this == hitbox);
}

Vector2d::Vector2d() :x(0), y(0) {}

Vector2d::Vector2d(int px, int py) :x(px), y(py) {}

void Vector2d::print() {
    cout << x << ", " << y << endl;
}

bool Vector2d::operator == (HitBox2d hitbox) {
    return (x >= hitbox.x && x <= (hitbox.x + hitbox.w) && y >= hitbox.y && y <= (hitbox.y + hitbox.h));
}

bool Vector2d::operator != (HitBox2d hitbox) {
    return !(*this == hitbox);
}

HitBox2d::HitBox2d(int px, int py, int pw, int ph) :x(px), y(py), w(pw), h(ph) {}

Uint8 math::binstrToUint8(string s) {
    bool b;
    Uint8 n = 0, m;
    Int8 i;
    for(i = 7, m = 1; i >= 0; i--, m *= 2) {
        b = s[i] == '1';
        n += b * m;
    }
    return n;
}

Uint8 math::hexstrToUint8(string s) {
    Uint8 h;
    char c;
    Uint8 n = 0, m;
    Int8 i;
    for(i = 1, m = 1; i >= 0; i--, m *= 16) {
        c = s[i];
        if(c >= '0' && c <= '9') {
            h = Uint8(c) - 48;
        }
        else if(c >= 'a' && c <= 'f') {
            h = Uint8(c) - 87;
        }
        else if(c >= 'A' && c <= 'F') {
            h = Uint8(c) - 55;
        }
        n += h * m;
    }
    return n;
}