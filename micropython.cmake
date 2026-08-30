# micropython.cmake
# Native CMake registration for MicroPython User C Modules

# 1. Define the module as a standard CMake interface library
add_library(usermod_esp32_twt INTERFACE)

# 2. Add source files using standard target commands
target_sources(usermod_esp32_twt INTERFACE "${CMAKE_CURRENT_LIST_DIR}/esp32_twt.c")

# 3. Add include directories
target_include_directories(usermod_esp32_twt INTERFACE "${CMAKE_CURRENT_LIST_DIR}")

# 4. Link the interface library directly to the primary usermod target
target_link_libraries(usermod INTERFACE usermod_esp32_twt)
