cmake_minimum_required(VERSION 2.6.0)

project(JumpPointSearch C)

set_source_files_properties(priority_queue.c PROPERTIES
    COMPILE_FLAGS "-std=gnu99")

add_library(jps SHARED map.c priority_queue.c jps.c)
install(TARGETS jps LIBRARY DESTINATION lib)

FIND_PACKAGE(PythonLibs)
install(FILES jps.py DESTINATION ${PYTHON_INCLUDE_PATH}/site-packages)

add_executable(example example.c)
target_link_libraries(example jps m)
