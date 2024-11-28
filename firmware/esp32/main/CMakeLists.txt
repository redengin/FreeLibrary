idf_component_register(
    SRCS
        "main.cpp"
        "WifiAccessPoint.cpp"
        "WebServer.cpp"
        "SdCard.cpp"
        "Catalog.cpp"

    EMBED_FILES
        "../../portal.html"
        "../../cacert.pem"
        "../../cacert_prvtkey.pem"
)