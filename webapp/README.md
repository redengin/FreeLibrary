Web Application
================================================================================
The webapp is embedded into the firmware to provide access to content without
the need of a native app (aka mobile app).

Choice of Framework
--------------------------------------------------------------------------------
The most common frameworks are [Angular](https://angular.dev/)
and [React](https://react.dev/)

### Framework Requirements
* sustained support of the framework
    * Google maintains Angular
        * Google will last, React continuation is questionable
* ability to create both webapp and native apps
    * primary use is to embed the webapp into the firmware
    * reuse for native apps (Android/iOS) can increase the reach

It really comes down to ecosystem. The React ecosystem requires the additional
burden of creating a user within the React ecosystem to support builds for
native apps (Android/iOS).

The Angular ecosystem supports can build native apps (Android/iOS) without this
burden.

While I really like React, Angular best supports the needs of this project.

