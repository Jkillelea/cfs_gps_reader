cmake_minimum_required(VERSION 2.6.4)
project(GPS_READER C)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(${sc_MISSION_DIR}/fsw/platform_inc)
include_directories(${libnmea_MISSION_DIR}/include)
include_directories(${io_lib_MISSION_DIR}/fsw/public_inc)

aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(gps_reader ${APP_SRC_FILES})
target_link_libraries(gps_reader m)
