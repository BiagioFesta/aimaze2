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
  ${PROJECT_SOURCE_DIR}/src/Player.cpp
  ${PROJECT_SOURCE_DIR}/src/Obstacle.cpp
  ${PROJECT_SOURCE_DIR}/src/ObstacleManager.cpp
  ${PROJECT_SOURCE_DIR}/src/Score.cpp
  ${PROJECT_SOURCE_DIR}/src/Genome.cpp
  ${PROJECT_SOURCE_DIR}/src/GeneNode.cpp
  ${PROJECT_SOURCE_DIR}/src/GeneConnection.cpp
  ${PROJECT_SOURCE_DIR}/src/InnovationHistory.cpp
  ${PROJECT_SOURCE_DIR}/src/GenomeDrawner.cpp
  ${PROJECT_SOURCE_DIR}/src/Population.cpp
  ${PROJECT_SOURCE_DIR}/src/Species.cpp
  ${PROJECT_SOURCE_DIR}/src/InfoDrawner.cpp)
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

option(BUILD_TESTS "Compile Unit Tests" NO)
if(${BUILD_TESTS})
  find_package(GTest REQUIRED)

  add_executable(${PROJECT_NAME}_test
    ${PROJECT_SOURCE_DIR}/test/main.cpp
    ${PROJECT_SOURCE_DIR}/test/testGenome.cpp
    ${PROJECT_SOURCE_DIR}/test/testPopulation.cpp
    ${PROJECT_SOURCE_DIR}/src/Genome.cpp
    ${PROJECT_SOURCE_DIR}/src/GeneNode.cpp
    ${PROJECT_SOURCE_DIR}/src/GeneConnection.cpp
    ${PROJECT_SOURCE_DIR}/src/InnovationHistory.cpp
    ${PROJECT_SOURCE_DIR}/src/Population.cpp
    ${PROJECT_SOURCE_DIR}/src/Species.cpp)
  target_include_directories(${PROJECT_NAME}_test PRIVATE src)
  target_link_libraries(${PROJECT_NAME}_test GTest::GTest GTest::Main)
  target_compile_features(${PROJECT_NAME}_test PRIVATE cxx_std_17)

  include(CTest)
  add_test(NAME TestAll COMMAND ${PROJECT_NAME}_test)
endif()
