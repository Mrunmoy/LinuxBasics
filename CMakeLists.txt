cmake_minimum_required(VERSION 3.10)
project(LinuxEpollChat)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(stage01_blocking_io)
add_subdirectory(stage02_blocking_server)
add_subdirectory(stage03_select_server)
# add_subdirectory(stage04_poll_server)
# add_subdirectory(stage05_epoll_server)
