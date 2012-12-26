cmake_minimum_required(VERSION 2.6.0)

project(JumpPointSearch C)

set_source_files_properties(priority_queue.c PROPERTIES
    COMPILE_FLAGS "-std=gnu99")

add_library(jps SHARED map.c priority_queue.c jps.c)
install(TARGETS jps LIBRARY DESTINATION lib)

find_package(PythonInterp)
if (NOT PYTHONINTERP_FOUND)
    message(WARNING
        "python interpreter not found, the python wrapper will not be installed"
        )
else()
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE}
        -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())"
        OUTPUT_VARIABLE PYTHON_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE)
    install(FILES jps.py DESTINATION ${PYTHON_SITE_PACKAGES})
endif()

add_executable(example example.c)
target_link_libraries(example jps m)
