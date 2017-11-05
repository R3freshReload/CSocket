cmake_minimum_required(VERSION 3.8)
project(CSocket)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES Server.cpp)
add_executable(Server ${SOURCE_FILES})
target_link_libraries(Server wsock32 ws2_32)

set(SOURCE_FILES Client.cpp)
add_executable(Client ${SOURCE_FILES})
target_link_libraries(Client wsock32 ws2_32)