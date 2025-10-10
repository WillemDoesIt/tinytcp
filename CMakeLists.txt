cmake_minimum_required(VERSION 3.29)
project(ttcp)

add_subdirectory(source)
install(TARGETS ttcp DESTINATION bin)

