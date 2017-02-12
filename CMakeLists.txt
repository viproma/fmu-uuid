cmake_minimum_required(VERSION 2.8.12)
project(fmu-uuid)

if(MSVC)
    add_compile_options("/WX" "/wd4996")
elseif(CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options("-std=c++1y" "-Wall" "-Werror")
endif()

if(WIN32)
    set(Boost_USE_STATIC_LIBS ON)
endif()
find_package(Boost REQUIRED)

add_executable("fmu-uuid" "fmu-uuid.cpp")
target_include_directories("fmu-uuid" PRIVATE ${Boost_INCLUDE_DIRS})
