#pragma once

#include <string>

class WifiAccessPoint {
public:
    static constexpr char TAG[] = "WifiAccessPoint";    ///< ESP logging tag

    /// @brief Creates the Open Wifi Access Point
    WifiAccessPoint(const std::string ssid);

    /// @brief change the published SSID
    void setSsid(const std::string ssid);

private:
    std::string ssid;
};
