#pragma once

#include <string>

class WifiAccessPoint {
public:
    WifiAccessPoint(const std::string ssid = "LittleFreeLibrary");

    /// begin publising SSID over the air
    void publish();

    /// @brief change the published SSID
    void setSsid(const std::string ssid);

private:
    std::string ssid;
};
