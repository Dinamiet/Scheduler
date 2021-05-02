cmake_minimum_required(VERSION 3.20.0)

project(
    TaskScheduler
)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    # Let's nicely support folders in IDEs
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    include(CTest)

    add_subdirectory(app)

endif()

add_library(
    ${PROJECT_NAME}
    STATIC
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        include
)

target_sources(
    ${PROJECT_NAME}
    PUBLIC  # All public source and header files listed here
        include/task_scheduler.h
    PRIVATE # All private source and header files listed here
        src/task_scheduler.c
)

add_subdirectory(extern/linked_lists)
target_link_libraries(${PROJECT_NAME} PRIVATE LinkedLists)
