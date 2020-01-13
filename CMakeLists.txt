cmake_minimum_required(VERSION 3.5)

project(qt5_threaded_image_flipper LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(qt5_threaded_image_flipper
  main.cpp
  image_view.cpp
  image_view.h
  image_operations.h
  image_operations.cpp
  image_engine.h
  image_engine.cpp
  app.h
  app.cpp
)

target_link_libraries(qt5_threaded_image_flipper PRIVATE Qt5::Widgets)
