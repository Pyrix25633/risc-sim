#pragma once

#include <iostream>

#include "utils.hpp"
#include "risc.hpp"
struct StatusRegister;

using namespace std;

typedef int8_t Int8;
typedef int16_t Int16;
typedef int32_t Int32;
ostream& operator << (ostream& os, Uint8 n);
ostream& operator << (ostream& os, Int8 n);

struct Vector2f;
struct Vector2d;
struct HitBox2d;

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
    /**
     * @brief Function to check if a vector is in a hitbox
     * @param hitbox The hitbox
    */
    bool operator == (HitBox2d hitbox);
    /**
     * @brief Function to check if a vector isn't in a hitbox
     * @param hitbox The hitbox
    */
    bool operator != (HitBox2d hitbox);
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
    /**
     * @brief Function to check if a vector is in a hitbox
     * @param hitbox The hitbox
    */
    bool operator == (HitBox2d hitbox);
    /**
     * @brief Function to check if a vector isn't in a hitbox
     * @param hitbox The hitbox
    */
    bool operator != (HitBox2d hitbox);
    int x, y;
};

/**
 * @brief Structure to make an hitbox
 * @param x X coordinate
 * @param y Y coordinate
 * @param w Width
 * @param h Height
*/
struct HitBox2d {
    /**
     * @brief Constructor
     * @param px X coordinate
     * @param py Y coordinate
     * @param pw Width
     * @param ph Height
    */
    HitBox2d(int px, int py, int pw, int ph);
    int x, y, w, h;
};

namespace math {
    /**
     * @brief Function to get the number from a binary string
     * @param s The binary string
     * @returns The number, type Uint8
    */
    Uint8 binstrToUint8(string s);
    /**
     * @brief Function to get the number from an hexadecimal string
     * @param s The hexadecimal string
     * @returns The number, type Uint8
    */
    Uint8 hexstrToUint8(string s);
    /**
     * @brief Function to get the hexadecimal string from a number
     * @param n The number, type Uint8
     * @returns The hexadecimal string
    */
    string Uint8Tohexstr(Uint8 n);
    /**
     * @brief Function to get the hexadecimal string from a number
     * @param n The number, type Uint16
     * @returns The hexadecimal string
    */
    string Uint16Tohexstr(Uint16 n);
    /**
     * @brief Function to get the hex string from the Status Register
     * @param sr The Status Register
     * @return The hex string
    */
    string StatusRegisterToHexstr(StatusRegister sr);
    /**
     * @brief Function to calculate the twos complement of a 16bit number
     * @param n The number
     * @returns The twos complement
    */
    Uint16 twosComplement(Uint16 n);
}