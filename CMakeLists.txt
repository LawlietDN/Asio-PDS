cmake_minimum_required(VERSION 3.10)
project(AsioPDS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED)
find_package(Threads REQUIRED)

include_directories(
    ${Boost_INCLUDE_DIRS}
    Include
)

file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "Include/*.h"
)

add_executable(AsioPDSApp ${SOURCES})

target_link_libraries(AsioPDSApp
    ${Boost_LIBRARIES}
    Threads::Threads
)
