#pragma once

#include "../defines.hpp"

struct SMonitor;

class CLayerSurface {
public:
    explicit CLayerSurface(SMonitor*);
    ~CLayerSurface();

    SMonitor* m_pMonitor = nullptr;

    zwlr_layer_surface_v1* pLayerSurface = nullptr; //used for env, ask for ack, background image
    wl_surface* pSurface = nullptr; //used for rendering

    wl_cursor_theme* pCursorTheme = nullptr;
    wl_cursor_image* pCursorImg = nullptr;
    wl_surface* pCursorSurface = nullptr;

    wp_fractional_scale_v1* pFractionalScaleInfo = nullptr;
    wp_viewport* pViewport = nullptr;
    double fScale = 1.0;
};
