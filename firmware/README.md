Firmware
================================================================================
Firmware implements the [API](../openapi/catalog.yaml) on simple devices
(embedded systems).

Implementation Syntax
--------------------------------------------------------------------------------
[Rust](https://www.rust-lang.org/) makes the implementation easlity portable to
new hardware.

<details>
<summmary>Why C/C++ efforts fail</summary>

Attempts to implement on ESP32 in C/C++ resulted in runtime failures due to
awkward implementation of std::filesystem. Attempts by espressif to refactor
the compiler have not been successful for over 5 years.

Correcting these errors requires lots of one-off runs to determine where
the memory management contract is violated.

By using Rust, compile time static analysis ensures that memory management contracts can
not be violated at runtime.
</details>

Leveraging the Rust ecosystem
================================================================================
Main Functional Requirements
* wifi access point
* sdcard filesystem

Current Dependencies
* [Embassy](https://embassy.dev/) - provides support for readily available hardware
    * provides a scheduler
    * provides a network stack
        * provides abstraction of network hardware

Supported Hardware
--------------------------------------------------------------------------------
[Espressif](esp) - support for Espressif chips