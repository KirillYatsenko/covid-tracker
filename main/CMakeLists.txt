set(COMPONENT_SRCS main.c fetch/fetch.c connect/connect.c display/display.c display/u8g2_esp32_hal.c sntp-time/sntp-time.c )
set(COMPONENT_ADD_INCLUDEDIRS "." "includes/")

register_component()