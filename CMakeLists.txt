cmake_minimum_required(VERSION 3.15)
project(AsioPDS LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

    find_package(Boost REQUIRED)
find_package(Threads REQUIRED)

file(GLOB_RECURSE SOURCES
     "src/*.cpp"
     "Include/*.h")

add_executable(AsioPDSApp ${SOURCES})

target_include_directories(AsioPDSApp PRIVATE
    Include
    ${Boost_INCLUDE_DIRS})

target_link_libraries(AsioPDSApp PRIVATE
    Boost::system
    Boost::thread 
    Boost::log
    Boost::log_setup
    Threads::Threads)

target_compile_definitions(AsioPDSApp PRIVATE BOOST_LOG_DYN_LINK)
