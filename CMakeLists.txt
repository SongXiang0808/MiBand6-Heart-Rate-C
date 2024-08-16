cmake_minimum_required(VERSION 3.5.0)
project(MiBand VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include_directories(/opt/remi-sdk/sysroots/aarch64-poky-linux/usr/include)

add_executable(MiBand main.cpp)

target_link_libraries(MiBand PUBLIC bluetooth)