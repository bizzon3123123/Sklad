cmake_minimum_required(VERSION 3.16)

project(MINM VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Сначала находим основные компоненты Qt6
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    Gui
    Network
)

# Создаем цель Qt6::HttpServer вручную, если она не найдена
if(NOT TARGET Qt6::HttpServer)
    message(STATUS "Creating Qt6::HttpServer target manually")
    
    # Проверяем существование библиотеки
    if(EXISTS "/usr/lib/x86_64-linux-gnu/libQt6HttpServer.so")
        add_library(Qt6::HttpServer SHARED IMPORTED)
        set_target_properties(Qt6::HttpServer PROPERTIES
            IMPORTED_LOCATION "/usr/lib/x86_64-linux-gnu/libQt6HttpServer.so"
            INTERFACE_INCLUDE_DIRECTORIES "/usr/include/x86_64-linux-gnu/qt6/QtHttpServer"
        )
        
        # Добавляем зависимости
        target_link_libraries(Qt6::HttpServer INTERFACE
            Qt6::Core
            Qt6::Network
        )
        
        message(STATUS "Qt6::HttpServer target created manually")
    else()
        message(FATAL_ERROR "Qt6HttpServer library not found. Install it with: sudo apt install libqt6httpserver-dev")
    endif()
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)  # Исправлено: было AUTRRC
set(CMAKE_AUTOUIC ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::Network
    Qt6::HttpServer
)

target_include_directories(${PROJECT_NAME} PRIVATE include)

# Для отладки добавьте:
message(STATUS "Qt6::Core found: ${Qt6Core_FOUND}")
message(STATUS "Qt6::Network found: ${Qt6Network_FOUND}")
message(STATUS "Qt6::HttpServer target exists: $<TARGET_EXISTS:Qt6::HttpServer>")