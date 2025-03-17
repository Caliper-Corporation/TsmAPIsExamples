cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
project(VehicleMonitor)

set(TSM_VER 7.0)
set(TSM_FOLDER "C://Program Files//TransModeler ${TSM_VER}")

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
endif()


include_directories(
        "${CMAKE_SOURCE_DIR}"
        "${CMAKE_SOURCE_DIR}/spdlog"
        "${TSM_FOLDER}"
        "${PROJECT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}.dir")

link_directories("${TSM_FOLDER}")

add_library(OS_LIB SHARED IMPORTED UNKNOWN)
set_target_properties(OS_LIB PROPERTIES
        IMPORTED_LOCATION_RELEASE ${TSM_FOLDER}/os.lib
        IMPORTED_LOCATION_DEBUG ${TSM_FOLDER}/os.lib)

set(SRC
        dllmain.cpp
        pch.cpp
        vehicle.cpp
        VehicleMonitor.rc)

add_library(${PROJECT_NAME} SHARED ${SRC})
target_link_libraries(${PROJECT_NAME} OS_LIB)

