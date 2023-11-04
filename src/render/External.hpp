#pragma once

#include "../defines.hpp"
#include <atomic>
#include <mutex>
//
// OJECTIF:
// - expose all relevant data to an external renderer:
//       - Shared struct with data (surface, display)
//       - Hyprpaper call to function to async change config (kill, size, scale -> wl event)
// - load .so and run it
// - do not crash and keep previous functionality as is
// - as simple as possible
//

struct ExternalRendererCom {
      enum Event {
         none = 0, //nothing has changed, only value that external renderer may write in updtEvent to clear
         update = 1,
         terminate = -1,
      };
      struct UpdtData {
         uint32_t width, height;
         int scale;
         wl_surface* surface;
      };

      //from Hyprpaper to ExtRndrr
   //mutex not needed bcs event flag ensure memory
   //ExtRndrr must only read this and use 
   //**std::atomic_thread_fence(std::memory_order_acquire)**
   //if an event is received to ensure sync
   UpdtData updtData; //mem consistency for whole block of data
   std::atomic<Event> event;
   std::atomic<wl_display*> display; //atomic jic but should not change ?

      //from ExtRndrr to Hyprpaper
   std::atomic<bool> status {}; //status of external renderer - replace with enum ?
};

//ISSUES: 
//    cleanUp/recreate surface (Comunication? + wait async !alive) 
//    One so = one config function, load multiple so or give new info to already running thread ? or force only one so
//    Options: 
//       1 so and send multiple info -> monitorID for cfg funct
//       n so and data ptr through ERCom struct -> data

using ExternalRendererThrd_pfun = void (*)(ExternalRendererCom*);

// -----------------------------
//  Only for Hyprpaper internal
struct ExternalRendererInfo {
   ExternalRendererCom com;
   bool launched;
   ExternalRendererThrd_pfun renderer;
   std::thread Rdrrthread;
   void* SOhandle; //usefulness of this ? -> dlclose / clean up TODO
   std::string path;
};
//TODO:
// [1/2] clean up 
//    - [ok] handle global remove kill external rendrr if monitor disconnected -> clearWPfromMon
//    - [ok?] unload .so if last to use
//    - [ok?] handleClosed erase CLayerSurface -> dtor
//    - [ok] exit software clean ?
//    - [ok] m_mMonitorExposed updt when terminate
//
// [1/2]Define .so interface and runtime
//    - renderer function of type ExternalRendererThrd_pfun
//    - fence aquire if event to read new data
//    - extrnl should provide its own way of getting user config (not through IPC nor Hyprpaper config file)
//    - extrnl should copy used data bcs change each update
//    -   //   should set alive to false when crashing/terminating
//    -   //   should listen to event to avoid bug and free all resources using monitor related data asap when a terminate event is received
//    -   //   should terminate all execution asap if all its com data are terminated, e.g. if it had received a terminate signal for all its monitors (dlclose will be called after about 10ms)
//    - a same renderer function may be called multiple time (once per monitor, with a diferent Com data ptr, and it has to accomodate this
//    - Hyprpaper should forward any relevant information through updtData
//
// [ok] IPC change -> parseKeyword and tick
// [ok] size of surface not monitor -> ls_configure edit sMonitor size
// [ok?] handle ExtRndrr die
// [ok?] CleanWallPaperFromMonitor -> from WP to extern ok?, see 2 bellow
// [ok?] load .so only once -> monitor id to diferentiate for cfg function
// [ok] Wants reload for external means Surface Update
// [ok] create smonitor -> event from wl
// [ok] wait for readyforls -> event from wl
// [ok?] create surface + set exposed bool -> ensureMonitor in check Monitor
// [ok?] launch .so func -> after LS creation if exposed in ensureHasActiveWP 
// [ok] communicate wl event -> issue when multiple monitor with same .so
//    - [ok] scale 
//    - [ok] quit
//    - [ok] (re)size of layer
// **make CHyprpaper function happy:**
//
// Questions:
// - wl_output ???
// - ViewPort destSize ????
// - wl_display_dispatch main thread and vulkan event polling ??? does vulkan does that ?? -> guess we'll see if bug
// - where is set CHyprpaper_should_exit + clean up -> never set to true lol ?
//
// - does vulkan tell me the same infos -> no need close and resize, lost of surface should be a swapchain error, resize too but need info about new size, scale ?
// - clearWallpaper -> send so a stop sig
// - where are created SMonitors (+ there size) -> Event wayland qui donne les outputs (1 truc dans handleglobal) - si disconnect : autre event aussi (handleglobalremove) - size: outputListener struct by wayland that gives infos about monitors - idem for size of layersurface
// - fractionalScaling/viewport -> size of buffer unrelated to size of surface (saling and cropping) -> need this for scaling and stuff ?
// - IPC -> call CConfigManager parseKeyword - Multithread here
