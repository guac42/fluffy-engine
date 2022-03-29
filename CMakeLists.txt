cmake_minimum_required(VERSION 3.19)
project(GameFrame)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

file(GLOB_RECURSE SOURCE
        src/*.cpp
        source/glad.c
        include/imgui/*.cpp)

add_executable(GameGL ${SOURCE})

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    file(GLOB LIBS
            libs/*.a)
    find_package(Threads REQUIRED)
    target_link_libraries(GameGL ${LIBS} ${CMAKE_DL_LIBS} Threads::Threads)
elseif (MSVC)
    file(GLOB LIBS
            libs/msvc/*.lib)
    target_link_libraries(GameGL ${LIBS})
elseif (MINGW)
    file(GLOB LIBS
            libs/mingw/*.a)
    target_link_libraries(GameGL ${LIBS})
endif()

# Resource folder for shaders etc.
file(GLOB_RECURSE RESOURCES RELATIVE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/resources/*)

foreach(FILE ${RESOURCES})
    configure_file(${CMAKE_SOURCE_DIR}/${FILE} ${CMAKE_CURRENT_BINARY_DIR}/${FILE} COPYONLY)
endforeach()