file(GLOB SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(ttcp ${SRC_FILES})
target_include_directories(ttcp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../headers)

find_package(fmt REQUIRED)
target_link_libraries(ttcp PRIVATE fmt::fmt)

