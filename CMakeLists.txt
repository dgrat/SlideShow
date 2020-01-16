cmake_minimum_required(VERSION 3.5)

project(slide_show LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets Svg REQUIRED)

add_executable(slide_show
  main.cpp
  image_view.cpp
  image_view.h
  image_operations.h
  image_operations.cpp
  image_engine.h
  image_engine.cpp
  app.h
  app.cpp
  ressource.qrc
)

target_link_libraries(slide_show PRIVATE Qt5::Widgets Qt5::Svg)
