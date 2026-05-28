/* -===============================================================================
    Includes
   ===============================================================================- */

#include <chrono>
#include <thread>
#include <barrier>

#include <raylib.h>
#include <raymath.h>

#include <framebuffer.h>

/* -===============================================================================
    Constant
   ===============================================================================- */

/* -===============================================================================
    Display
   ===============================================================================- */

void Framebuffer::init() {
    for (int i = 0; i < FB_PALETTE_SIZE; ++i)
        vec_palette[i] = color2vec(palette[i]);
}

void Framebuffer::update(std::function<Color(Vector2)> callback) {
    constexpr int thread_count = 12;
    std::barrier sync_point(thread_count);
    std::thread threads[thread_count];

    for (int i = 0; i < thread_count; ++i) {
        threads[i] = std::thread([&, i]() {
            // All threads wait here until everyone is ready
            sync_point.arrive_and_wait();

            int size = (FB_WIDTH * FB_HEIGHT) / thread_count;
            for (int j = 0; j < size; ++j) {
                int base = i * size + j;
                data[base] = callback({(float)(base % FB_WIDTH), (float)base / FB_WIDTH});
            }
        });
    }

    for (int i = 0; i < thread_count; ++i) {
        threads[i].join();
    }
}

void Framebuffer::draw() {
    for (int y = 0; y < FB_HEIGHT; ++y)
    for (int x = 0; x < FB_WIDTH; ++x) {
        DrawPixel(x, y, data[x + y * FB_WIDTH]);
    }
}

void Framebuffer::set(float x, float y, Color color) {
    if (x < 0 || x >= FB_WIDTH || y < 0 || y >= FB_HEIGHT) return;
    int idx = (int)(x + y * FB_WIDTH);
    data[idx] = color;//palette[dither(color2vec(color), (int)x % FB_WIDTH, (int)y / FB_WIDTH)];
}

Color Framebuffer::get(float x, float y) const {
    if ((x < 0 || x >= FB_WIDTH) && (y < 0 || y >= FB_HEIGHT))
        return { 0, 0, 0, 255 };
    return data[get_index(x, y)];
}

int Framebuffer::get_index(float x, float y) const {
    return round(x) + round(y) * FB_WIDTH;
}

void Framebuffer::rect(float x, float y, float w, float h, Color color) {
    for (float iy = 0; iy < h; ++iy)
    for (float ix = 0; ix < w; ++ix) {
        set(x + ix, y + iy, color);
    }
}