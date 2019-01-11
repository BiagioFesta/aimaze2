cmake_minimum_required(VERSION 3.8)
project(aimaze2)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup(TARGETS)
endif()

find_package(SFML 2 REQUIRED graphics window system)

add_executable(${PROJECT_NAME}
  ${PROJECT_SOURCE_DIR}/src/main.cpp
  ${PROJECT_SOURCE_DIR}/src/AIMaze.cpp
  ${PROJECT_SOURCE_DIR}/src/GameScene.cpp
  ${PROJECT_SOURCE_DIR}/src/Ground.cpp
  ${PROJECT_SOURCE_DIR}/src/Player.cpp)
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
