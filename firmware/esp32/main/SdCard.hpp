#pragma once

class SdCard {
public:
    static constexpr char TAG[] = "SdCard"; ///< ESP logging tag

    SdCard();

private:
    static constexpr char path[] = "/sdcard";
};
