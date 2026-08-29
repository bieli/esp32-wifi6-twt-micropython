# micropython.cmake
# Register the user C module within the MicroPython CMake build system
_extmod_add_library(esp32_twt INTERFACE)
_extmod_target_sources(esp32_twt INTERFACE "${CMAKE_CURRENT_LIST_DIR}/esp32_twt.c")
_extmod_target_include_directories(esp32_twt INTERFACE "${CMAKE_CURRENT_LIST_DIR}")
_extmod_register_module(esp32_twt)
