#pragma once

#include <iostream>

#include "utils.hpp"

using namespace std;

/**
 * @brief Struct that contains float x and y coordinates
 * @param x X coordinate, type float
 * @param y Y coordinate, type float
*/
struct Vector2f {
    /**
     * @brief Initializer to 0
    */
    Vector2f();
    /**
     * @brief Initializer to parameters
     * @param px To initialize x, type float
     * @param py To initialize y, type float
    */
    Vector2f(float px, float py);
    /**
     * @brief Function to print x and y
    */
    void print();
    float x, y;
};

/**
 * @brief Struct that contains decimal x and y coordinates
 * @param x X coordinate, type int
 * @param y Y coordinate, type int
*/
struct Vector2d {
    /**
     * @brief Initializer to 0
    */
    Vector2d();
    /**
     * @brief Initializer to parameters
     * @param px To initialize x, type int
     * @param py To initialize y, type int
    */
    Vector2d(int px, int py);
    /**
     * @brief Function to print x and y
    */
    void print();
    int x, y;
};

namespace math {
    Uint8 binstrToUint8(string s);
    Uint8 hexstrToUint8(string s);
}