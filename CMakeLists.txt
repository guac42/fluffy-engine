cmake_minimum_required(VERSION 3.19)
project(GameFrame)

set(CMAKE_CXX_STANDARD 20)

include_directories(include/bullet)
include_directories(include)

file(GLOB_RECURSE SOURCE
        src/*.cpp
        source/glad.c
        include/imgui/*.cpp)

add_executable(Fluffy ${SOURCE})

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    file(GLOB LIBS libs/linux/*.a)
    find_package(Threads REQUIRED)
    find_package(Bullet REQUIRED)
    target_link_libraries(Fluffy ${LIBS} ${CMAKE_DL_LIBS} ${BULLET_LIBRARIES} Threads::Threads)
elseif (MSVC)
    if (CMAKE_BUILD_TYPE MATCHES Debug)
        file(GLOB LIBS
                libs/msvc/*.lib
                libs/msvc/debug/*.lib)
    else()
        file(GLOB LIBS
                libs/msvc/*.lib
                libs/msvc/release/*.lib)
    endif()
    target_link_libraries(Fluffy ${LIBS})
elseif (MINGW)
    file(GLOB LIBS
            libs/mingw/*.a)
    target_link_libraries(Fluffy ${LIBS})
endif()

# Resource folder for shaders etc.
file(GLOB_RECURSE RESOURCES RELATIVE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/resources/*)

foreach(FILE ${RESOURCES})
    configure_file(${CMAKE_SOURCE_DIR}/${FILE} ${CMAKE_CURRENT_BINARY_DIR}/${FILE} COPYONLY)
endforeach()