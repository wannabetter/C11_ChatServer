cmake_minimum_required(VERSION 3.10)
project(ChatServer)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)
include_directories(/root/GaoSheng)

file(GLOB_RECURSE ServerSRC ${PROJECT_SOURCE_DIR}/src/server/*.cpp)

set(muduo "/root/GaoSheng/build/release-install-cpp11")
include_directories(${muduo}/include)
link_directories(${muduo}/lib)

link_libraries(muduo_net muduo_base mysqlclient hiredis pthread)

add_executable(ChatServer main.cpp ${ServerSRC})
add_executable(ChatClient src/client/main.cpp)


