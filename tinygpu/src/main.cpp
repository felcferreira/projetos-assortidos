/* -===============================================================================
    Includes
   ===============================================================================- */

#include <cstdint>
#include <stdio.h>

#include <raylib.h>
#include <raymath.h>

#include <framebuffer.h>

/* -===============================================================================
    Entry point
   ===============================================================================- */

#define CYAN { 0, 255, 255, 255 }

static Framebuffer fb;

float smoothstep(float edge0, float edge1, float x) {
    x = Clamp((x - edge0) / (edge1 - edge0), 0.f, 1.f);
    return x * x * (3.0f - 2.0f * x);
}

Vector3 Vector3Sin(Vector3 v) {
    return { sinf(v.x), sinf(v.y), sinf(v.z) };
}

Vector4 Vector4Cos(Vector4 v) {
    return { cosf(v.x), cosf(v.y), cosf(v.z), cosf(v.w) };
}

Vector2 Vector2Mod(Vector2 a, Vector2 b) {
    const Vector2 ab = a / b;
    return a - b * Vector2({floor(ab.x), floor(ab.y)});
}

inline Color colors(Vector2 pos) {
    return ColorLerp(ColorLerp(RED, YELLOW, pos.x / FB_WIDTH), ColorLerp(BLUE, CYAN, pos.x / FB_WIDTH), pos.y / FB_HEIGHT);
}

inline Color shades(Vector2 pos) {
    return ColorLerp(ColorLerp(BLACK, WHITE, pos.x / FB_WIDTH), ColorLerp(WHITE, BLACK, pos.x / FB_WIDTH), pos.y / FB_HEIGHT);
}

inline Color fract(Vector2 pos) {
    // https://www.shadertoy.com/view/WXt3Wj
    Vector2 uv = (pos - Vector2({FB_WIDTH, FB_HEIGHT}) * 0.5) / FB_HEIGHT;
    float r = Vector2Length(uv);
    float a = atan2(uv.y, uv.x);
    a = fabs(a);
    a = fmod(a, 3.14159f / 4.f);
    float pattern = sin(10.0 * r + sin(8.0 * a + GetTime() * 0.5)) + 0.5 * cos(20.0 * r - GetTime());
    float mask = smoothstep(0.2, 0.0, abs(pattern));

    return vec2color({ mask, mask, mask });
}

inline Color fract2(Vector2 pos) {
    Vector3 c;
    float l = 0.f;
    float z = GetTime();
    float t = GetTime();
    for (int i = 0; i < 3; ++i) {
        Vector2 p = pos / Vector2({FB_WIDTH,FB_HEIGHT});
        Vector2 uv = (pos - Vector2({FB_WIDTH, FB_HEIGHT}) * 0.5) / FB_HEIGHT;
        p = p - Vector2({.5f,.5f});
        p.x *= (float)FB_WIDTH / (float)FB_HEIGHT;
        z += .07f;
        l = Vector2Length(p);
        uv += p / l * (sinf(z) + 1.f) * fabs(sinf(l * 9.f - z - z));
        ((float *)&c)[i] = .01f / Vector2Length(Vector2Mod(uv, {1.f, 1.f}) - Vector2({.5f,.5f}));
    }
    Vector3 o = c / l;
    return vec2color({ o.x, o.y, o.z, t });
}

inline Color ps3(Vector2 pos) {
    // https://www.shadertoy.com/view/33t3WB
    Vector3 out = Vector3Zero();

    auto wave = [](Vector2 uv, Vector4 amps, Vector4 freqs, Vector4 offset) -> float {
        float x = uv.x;

        float time = GetTime();
        float y = 0.;
        y += amps.x * sin(freqs.x * x + time + offset.x);
        y += amps.y * sin(freqs.y * x + time + offset.y);
        y += amps.z * sin(freqs.z * x + time + offset.z);
        y += amps.w * sin(freqs.w * x + time + offset.w);

        return smoothstep(y + .025, y, uv.y) * smoothstep(y - 1., y, uv.y) * .4;
    };

    Vector2 uv = (Vector2({FB_WIDTH,FB_HEIGHT}) - pos * 2.) * 2. / FB_HEIGHT;
    out = Vector3Lerp(Vector3({.2, .0, .4}), Vector3({.0, .0, .9}), (pos.y / FB_HEIGHT));

    float f = 0.f;
    f += wave(uv, {.1, .2, .3, .4}, {.1, .4, .8, .3}, Vector4({1., 1.5, 2., 2.5}) * 3.1415926535f);
    f += wave(uv, {.1, .3, .4, .1}, {.8, .5, .4, .3}, {5., 2., 1., 3.});
    f += wave(uv, {.3, .2, .1, .2}, {.9, .5, .1, .1}, {1., 2., 2., 3.});

    out = Vector3({.6f,.6f,.6f}) * f + out;
    return vec2color(Vector4({out.x, out.y, out.z, 1.f}));
}

int main(int argc, char * argv[]) {
    // Raylib
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "tinygpu");

    //SetTargetFPS(60);
    SetTraceLogLevel(LOG_WARNING);

    // Framebuffer and palette
    //int colors = 0;
    //Color * palette = LoadImagePalette(pal_image, 256, &colors);
    //
    //for (int i = 0; i < FB_PALETTE_SIZE; ++i)
    //    fb.palette[i] = palette[i];
    //fb.init();

    // RenderTexture
    RenderTexture render_texture = LoadRenderTexture(FB_WIDTH, FB_HEIGHT);

    // Shader
    Shader shader = LoadShader("assets/screen.vs", "assets/screen.fs");
    float screen_size[2] = { FB_WIDTH, FB_HEIGHT };
    SetShaderValue(shader, GetShaderLocation(shader, "screenSize"), screen_size, SHADER_UNIFORM_VEC2);
    SetShaderValueV(shader, GetShaderLocation(shader, "palette"), fb.vec_palette, SHADER_UNIFORM_VEC4, FB_PALETTE_SIZE);

    Texture texture;
    Image img = {
        .data = fb.data,
        .width = FB_WIDTH,
        .height = FB_HEIGHT,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };

    while (!WindowShouldClose()) {
        texture = LoadTextureFromImage(img);

        fb.update(ps3);

        BeginDrawing();
            ClearBackground(BLACK);

            BeginTextureMode(render_texture);
                DrawTexture(texture, 0, 0, WHITE);
            EndTextureMode();

            BeginShaderMode(shader);
                DrawTexturePro(texture, 
                    (Rectangle){ 0.f, 0.f, FB_WIDTH, FB_HEIGHT }, 
                    (Rectangle){ 0.f, 0.f, SCREEN_WIDTH, SCREEN_HEIGHT }, 
                    Vector2Zero(), 0.f, WHITE
                );
            EndShaderMode();

            DrawFPS(1, 1);
        EndDrawing();

        UnloadTexture(texture);
    }

    CloseWindow();

    return 0;
}
