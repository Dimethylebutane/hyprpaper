#pragma once

#include "defines.hpp"
#include "config/ConfigManager.hpp"
#include "render/WallpaperTarget.hpp"
#include "render/External.hpp"
#include "helpers/Monitor.hpp"
#include "events/Events.hpp"
#include "helpers/PoolBuffer.hpp"
#include "helpers/MiscFunctions.hpp"
#include "ipc/Socket.hpp"
#include <mutex>

struct SWallpaperRenderData {
    bool contain = false;
};

class CHyprpaper {
public:
    // important
    wl_display* m_sDisplay; // assured
    wl_compositor* m_sCompositor; // assured
    wl_shm* m_sSHM; // assured  --  SHared Memory with compositor to pass data
    zwlr_layer_shell_v1* m_sLayerShell = nullptr; // expected
    wp_fractional_scale_manager_v1* m_sFractionalScale = nullptr; // will remain null if not bound
    wp_viewporter* m_sViewporter = nullptr; // expected

    // init the utility
    CHyprpaper();
    void        init();
    void        tick(bool force);

    std::unordered_map<std::string, CWallpaperTarget> m_mWallpaperTargets;
    std::unordered_map<std::string, std::string> m_mMonitorActiveWallpapers;
    std::unordered_map<std::string, SWallpaperRenderData> m_mMonitorWallpaperRenderData;
    std::unordered_map<SMonitor*, CWallpaperTarget*> m_mMonitorActiveWallpaperTargets;
    std::unordered_map<std::string, ExternalRendererInfo> m_mMonitorExposed; //hold names of exposed monitors and link to external renderer
    std::vector<std::unique_ptr<SPoolBuffer>> m_vBuffers;
    std::vector<std::unique_ptr<SMonitor>> m_vMonitors;

    bool        m_bIPCEnabled = true;
    bool        m_bRenderSplash = false;
    float       m_fSplashOffset = 2;
    std::string m_szExplicitConfigPath;
    bool        m_bNoFractionalScale = false;

    void        removeOldHyprpaperImages();
    void        preloadAllWallpapersFromConfig();
    void        recheckAllMonitors();
    void        ensureMonitorHasActiveWallpaper(SMonitor*);
    void        createLSForMonitor(SMonitor*);
    void        renderWallpaperForMonitor(SMonitor*);
    void        createBuffer(SPoolBuffer*, int32_t, int32_t, uint32_t);
    void        destroyBuffer(SPoolBuffer*);
    void        launchExternalRenderer(SMonitor*, ExternalRendererInfo*);
    static void sendDataToExternalRenderer(ExternalRendererCom::UpdtData, ExternalRendererCom::Event, ExternalRendererInfo*);
    void terminateExternalRenderer(ExternalRendererInfo&,SMonitor*, bool noDlClose = false);
    int         createPoolFile(size_t, std::string&);
    bool        setCloexec(const int&);
    void        clearWallpaperFromMonitor(const std::string&);
    SMonitor*   getMonitorFromName(const std::string&);
    bool        isPreloaded(const std::string&);
    void        recheckMonitor(SMonitor*);
    void        ensurePoolBuffersPresent();
    SPoolBuffer* getPoolBuffer(SMonitor*, CWallpaperTarget*);
    void        unloadWallpaper(const std::string&);
    void        createSeat(wl_seat*);
    bool        lockSingleInstance(); // fails on multi-instance
    void        unlockSingleInstance();

    std::mutex  m_mtTickMutex;

    SMonitor*   m_pLastMonitor = nullptr;
private:
    
    bool        m_bShouldExit = false;
};

inline std::unique_ptr<CHyprpaper> g_pHyprpaper;
