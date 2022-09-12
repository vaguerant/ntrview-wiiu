#pragma once

//how much C++ can a C programmer squeeze into one header? well...

#include <memory>
#include <span>
#include <vector>
#include <array>

#if defined(GFX_GX2)
#include <gx2r/buffer.h>
#include <gx2r/surface.h>
#include <gx2/texture.h>
#include <gx2/utils.h>
#include <gx2/sampler.h>
#endif

namespace Gfx {

typedef enum Rotation {
    GFX_ROTATION_0 = 0,
    GFX_ROTATION_90 = 1,
    GFX_ROTATION_180 = 2,
    GFX_ROTATION_270 = 3,
} Rotation;
typedef enum Resolution {
    RESOLUTION_480P  = 0,
    RESOLUTION_720P  = 1,
    RESOLUTION_1080P = 2,

    RESOLUTION_MAX = 3,
} Resolution;

typedef enum DrawMode {
    DRAWMODE_TEXTURE_RGB = 0,
    DRAWMODE_TEXT,
} DrawMode;

typedef struct Dimensions {
    int w;
    int h;
} Dimensions;
typedef struct Rect {
    int x;
    int y;
    Dimensions d;
    Rotation rotation = GFX_ROTATION_0;
} Rect;
typedef struct Tri { //a mildly absurd triangle abstraction
    int x, y;
    int size;
    Rotation rotation = GFX_ROTATION_0;
} Tri;
typedef struct rgb {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a = 0xFF;
    float flt_r() const { return (float)r / 255; }
    float flt_g() const { return (float)g / 255; }
    float flt_b() const { return (float)b / 255; }
    float flt_a() const { return (float)a / 255; }
} rgb;

class FillRect {
public:
    Rect r;
    std::array<rgb, 4> c;
    FillRect(Rect r, rgb c) : r(r) {
        this->c.fill(c);
    }
    FillRect() {}
};

class FillTri {
public:
    Tri t;
    std::array<rgb, 3> c;
    FillTri(Tri t, rgb c) : t(t) {
        this->c.fill(c);
    }
    FillTri() {}
};

//todo probs should be a member function
void DrawFillRect(const FillRect& rect);
void DrawFillTri(const FillTri& tri);

class Texture {
public:
    Dimensions d;
    int bpp = 32;
    int bypp = 4;
    void Render(Rect dest, rgb colour);
    void Render(Rect dest) { Render(dest, (rgb) { .r = 0xff, .g = 0xff, .b = 0xff, .a = 0xff}); }

    DrawMode mode;

    bool locked = false;
    int pitch;
    std::span<uint8_t> Lock();
    void Unlock(std::span<uint8_t>& pixels);

    Texture(int w, int h, DrawMode mode = DRAWMODE_TEXTURE_RGB);
    Texture() {};

#if defined(GFX_GX2)
    GX2Sampler sampler;
    GX2Texture gx2_tex = {
        .surface = (GX2Surface) {
            .dim = GX2_SURFACE_DIM_TEXTURE_2D,
            .depth = 1,
            .mipLevels = 1,
            .format = GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8,
            .tileMode = GX2_TILE_MODE_LINEAR_ALIGNED,
        },
        .viewNumSlices = 1,
        .compMap = GX2_COMP_MAP(
            GX2_SQ_SEL_R, GX2_SQ_SEL_G, GX2_SQ_SEL_B, GX2_SQ_SEL_A
        ),
    };
    bool valid() { return d.w > 0 && d.h > 0; }
#endif
};

bool Init();
void Quit();
void Clear(rgb colour);
void PrepRender();
void PrepRenderTop();
void DoneRenderTop();
void PrepRenderBtm();
void DoneRenderBtm();
void Present();

const char* GetError();
Resolution GetResolution();
int GetCurrentScreenWidth();
int GetCurrentScreenHeight();

} //namespace Gfx
