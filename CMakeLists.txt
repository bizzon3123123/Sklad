cmake_minimum_required(VERSION 3.16)
project(WarehouseWorkerSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

set(CMAKE_AUTOMOC ON)

# Файлы проекта
add_executable(${PROJECT_NAME}
    main.cpp
    mainwindow.cpp
    warehouse.cpp
)

target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)