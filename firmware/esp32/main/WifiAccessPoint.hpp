#pragma once

#include <string>
#include <vector>

class WifiAccessPoint {
public:
    static constexpr char TAG[] = "WifiAccessPoint";    ///< ESP logging tag

    /// @brief Creates the Open Wifi Access Point
    /// @post call publish()
    /// @param ssid 
    WifiAccessPoint(const std::string ssid = "LittleFreeLibrary");

    /// @brief change the published SSID
    void setSsid(const std::string ssid);

private:
    std::string ssid;
};
