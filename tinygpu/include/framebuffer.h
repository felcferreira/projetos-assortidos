#pragma once
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

/* -===============================================================================
    Includes
   ===============================================================================- */

#include <cstdint>
#include <functional>

#include <raylib.h>
#include <raymath.h>

/* -===============================================================================
    Constant
   ===============================================================================- */

constexpr const int FB_WIDTH        = 320;
constexpr const int FB_HEIGHT       = 240;
constexpr const int FB_BPP          = 6;
constexpr const int FB_PALETTE_SIZE = 256;
constexpr const int SCREEN_WIDTH    = (FB_WIDTH * 3);
constexpr const int SCREEN_HEIGHT   = (FB_HEIGHT * 3);

/* -===============================================================================
    Framebuffer
   ===============================================================================- */

class Framebuffer {
public:
    void init();
    void update(std::function<Color(Vector2)> callback);
    void draw();

    void  set(float x, float y, Color color);
    Color get(float x, float y) const;
    int   get_index(float x, float y) const;
    int   dither(Vector4 color, int x, int y) const;

    void rect(float x, float y, float w, float h, Color color);

    Vector4 vec_palette[FB_PALETTE_SIZE];
    Color palette[FB_PALETTE_SIZE];
    Color data[FB_WIDTH * FB_HEIGHT];
};

constexpr Vector4 color2vec(Color color) {
    return { (float)color.r / 255.f, (float)color.g / 255.f, (float)color.b / 255.f, (float)color.a / 255.f };
}

constexpr Color vec2color(Vector4 color) {
    return {
        (uint8_t)Clamp(color.x * 255.f, 0, 255), 
        (uint8_t)Clamp(color.y * 255.f, 0, 255), 
        (uint8_t)Clamp(color.z * 255.f, 0, 255), 
        (uint8_t)Clamp(color.w * 255.f, 0, 255)
    };
}

#endif
