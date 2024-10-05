cmake_minimum_required(VERSION 3.8.1)

project(proxy_server)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11")

include_directories(include)

# Поиск PostgreSQL 
#find_package(PostgreSQL REQUIRED) 
#include_directories(${PostgreSQL_INCLUDE_DIR}) 

option(PROXY_SERVER "Build PROXY_SERVER" OFF)

if (PROXY_SERVER)

    add_definitions(
        -DPROXY_SERVER
    )

    add_executable(proxy_server main_proxy.cpp src/proxy_server.cpp)
    # Добавление флага -lpq 
    # target_link_libraries(proxy ${PROJECT_NAME} ${PostgreSQL_LIBRARIES})

endif()

option(CLIENT "Build CLIENT" ON)

if (CLIENT)

    add_definitions(
        -DCLIENT
    )

    add_executable(client main_client.cpp src/client.cpp)
    # Добавление флага -lpq 
    # target_link_libraries(client ${PROJECT_NAME} ${PostgreSQL_LIBRARIES})

endif()