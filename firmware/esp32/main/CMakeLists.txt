idf_component_register(
    SRCS
        "main.cpp"
        "WifiAccessPoint.cpp"
        "WebServer.cpp"
    # INCLUDE_DIRS
    #     "."

    EMBED_FILES
        "../../portal.html"
)