idf_component_register(
    SRC_DIRS
    "."
    "Application/Controller"
    "Application/Services"
    "Application/Devices"    
    "Application/Drivers"   
    "Application"
    INCLUDE_DIRS
    "."
    REQUIRES
    driver
    esp_timer
    spi_flash
    bt
)
