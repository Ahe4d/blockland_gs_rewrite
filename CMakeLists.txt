cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_BUILD_TYPE Release)
project (OurRewrite)
include(MologieDetours.cmake)
set(mathfu_build_benchmarks OFF CACHE BOOL "")
set(mathfu_build_tests OFF CACHE BOOL "")
add_subdirectory(mathfu/ mathfu)
include(mathfu/CMakeLists.txt)
mathfu_configure_flags(OurRewrite)

ADD_LIBRARY(OurRewrite SHARED dllmain.cpp Torque.cpp client_audio.cpp client_canvas.cpp client_init.cpp client_message.cpp client_mission.cpp client_missionDownload.cpp main.cpp server_anticheat.cpp)
SET_TARGET_PROPERTIES(OurRewrite PROPERTIES LINK_FLAGS "-static-libgcc -static-libstdc++")
set(OurRewrite "${CMAKE_CXX_FLAGS} -static -O2")
target_link_libraries(OurRewrite MologieDetours mathfu)