cmake_minimum_required(VERSION 3.16)
project(resource_loader_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(resource_loader_demo
    demo.cpp
    JsonResource.cpp
    Registry.cpp
    ResourceManager.cpp
    ResourceLoader.cpp
    ExampleConcrete.cpp
)

target_include_directories(resource_loader_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
