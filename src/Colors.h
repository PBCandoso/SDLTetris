#pragma once
#include <cstdint>

typedef struct Color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} Color;

inline Color
color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    Color result;
    result.r = r;
    result.g = g;
    result.b = b;
    result.a = a;
    return result;
}

const Color GRID_OUTLINE = color(0xD2, 0xD2, 0xD2, 0xFF);

const Color BASE_COLORS[] = {
    color(0x28, 0x28, 0x28, 0xFF),
    color(0x2D, 0x99, 0x99, 0xFF),
    color(0x99, 0x99, 0x2D, 0xFF),
    color(0x99, 0x2D, 0x99, 0xFF),
    color(0x2D, 0x99, 0x51, 0xFF),
    color(0x99, 0x2D, 0x2D, 0xFF),
    color(0x2D, 0x63, 0x99, 0xFF),
    color(0x99, 0x63, 0x2D, 0xFF),

};

const Color LIGHT_COLORS[] = {
    color(0x28, 0x28, 0x28, 0xFF),
    color(0x44, 0xE5, 0xE5, 0xFF),
    color(0xE5, 0xE5, 0x44, 0xFF),
    color(0xE5, 0x44, 0xE5, 0xFF),
    color(0x44, 0xE5, 0x7A, 0xFF),
    color(0xE5, 0x44, 0x44, 0xFF),
    color(0x44, 0x95, 0xE5, 0xFF),
    color(0xE5, 0x95, 0x44, 0xFF)
};

const Color DARK_COLORS[] = {
    color(0x28, 0x28, 0x28, 0xFF),
    color(0x1E, 0x66, 0x66, 0xFF),
    color(0x66, 0x66, 0x1E, 0xFF),
    color(0x66, 0x1E, 0x66, 0xFF),
    color(0x1E, 0x66, 0x36, 0xFF),
    color(0x66, 0x1E, 0x1E, 0xFF),
    color(0x1E, 0x42, 0x66, 0xFF),
    color(0x66, 0x42, 0x1E, 0xFF)
};
