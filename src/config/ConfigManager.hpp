#pragma once
#include "../defines.hpp"
#include "../render/External.hpp"

class CIPCSocket;

class CConfigManager {
public:
    // gets all the data from the config
    CConfigManager();

    std::deque<std::string> m_dRequestedPreloads;
    std::string getMainConfigPath();

private:
    std::string parseError;

    void parseLine(std::string&);
    std::string removeBeginEndSpacesTabs(std::string in);
    void parseKeyword(const std::string&, const std::string&);

    void handleWallpaper(const std::string&, const std::string&);
    void handlePreload(const std::string&, const std::string&);
    void handleUnload(const std::string&, const std::string&);
    void handleUnloadAll(const std::string&, const std::string&);
    void handleExternSurface(const std::string& monitor, const std::string& wallpaper);
    std::string trimPath(std::string path);

    friend class CIPCSocket;
};

inline std::unique_ptr<CConfigManager> g_pConfigManager;
