cmake_minimum_required(VERSION 3.23)
project(hana_zip_with)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED)

add_executable(hana_zip_with main.cpp)
target_link_libraries(hana_zip_with Boost::boost)
