#pragma once

#include "../defines.hpp"

class CWallpaperTarget;

struct SPoolBuffer {
    wl_buffer* buffer = nullptr;
    cairo_surface_t* surface = nullptr;
    cairo_t* cairo = nullptr;
    void* data = nullptr;
    size_t size = 0;
    std::string name = "";

    std::string target = ""; //path of picture 
    Vector2D pixelSize;
};
