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

    // resolve all DNS queries to the captive portal
    static std::vector<char> dnsReply(const void* const request, const size_t sz);

private:
    std::string ssid;
};
