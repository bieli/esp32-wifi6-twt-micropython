# micropython.cmake
# Register the custom C module using the modern MicroPython usermod API

mp_usermod_add_library(usermod_esp32_twt INTERFACE)

target_sources(usermod_esp32_twt INTERFACE "${CMAKE_CURRENT_LIST_DIR}/esp32_twt.c")

target_include_directories(usermod_esp32_twt INTERFACE "${CMAKE_CURRENT_LIST_DIR}")

# Link the module library to the main usermod target group
target_link_libraries(usermod INTERFACE usermod_esp32_twt)
