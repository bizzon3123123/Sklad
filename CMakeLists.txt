cmake_minimum_required(VERSION 3.16)
project(WarehouseSystem)

set(CMAKE_CXX_STANDARD 17)

# Ищем Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Создаем исполняемый файл
add_executable(${PROJECT_NAME} main.cpp)

# Подключаем библиотеки Qt
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Отключаем предупреждения об отсутствии UI файлов
set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOMOC ON
)