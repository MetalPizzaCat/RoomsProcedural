cmake_minimum_required(VERSION 3.0.0)
project(Rooms VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)
add_executable(Rooms main.cpp)


target_link_libraries(Rooms PRIVATE sfml-graphics)
target_compile_features(Rooms PRIVATE cxx_std_17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
