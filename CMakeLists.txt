cmake_minimum_required(VERSION 3.17)
project(eruptor)

include(${CMAKE_SOURCE_DIR}/cmake/modules/libs_maker.cmake)

find_package(Vulkan REQUIRED)
find_package(assimp REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_subdirectory(hardware/)

set(RESOURCE_SRC ${CMAKE_CURRENT_LIST_DIR}/src/resource/resource_manager.cpp
                 ${CMAKE_CURRENT_LIST_DIR}/src/resource/resource_handle.cpp
                 ${CMAKE_CURRENT_LIST_DIR}/src/resource/resource.cpp)

set(RENDERER_SRC ${CMAKE_CURRENT_LIST_DIR}/src/renderer/renderer.cpp
                 ${CMAKE_CURRENT_LIST_DIR}/src/renderer/cameras.cpp)

set(SCENE_SRC ${CMAKE_CURRENT_LIST_DIR}/src/scene/scene.cpp
              ${CMAKE_CURRENT_LIST_DIR}/src/scene/render_object.cpp
              ${CMAKE_CURRENT_LIST_DIR}/src/scene/transformation.cpp)

set(ENGINE_SRC ${CMAKE_CURRENT_LIST_DIR}/src/eruptor.cpp)

Make_library(TARGET eruptor_resource
SOURCES RESOURCE_SRC
LIBRARIES_TO_LINK_PRIV eruptor_hardware assimp::assimp eruptor_event
COMPILATION_FEATURES cxx_std_23)

Make_library(TARGET eruptor_scene
SOURCES SCENE_SRC
LIBRARIES_TO_LINK_PRIV eruptor_resource glm::glm
COMPILATION_FEATURES cxx_std_23)

Make_library(TARGET eruptor_renderer
SOURCES RENDERER_SRC
LIBRARIES_TO_LINK_PRIV eruptor_hardware eruptor_resource eruptor_scene Vulkan::Vulkan eruptor_event
COMPILATION_FEATURES cxx_std_23)

Make_library(TARGET eruptor_engine
SOURCES ENGINE_SRC
LIBRARIES_TO_LINK_PRIV eruptor_hardware eruptor_event
LIBRARIES_TO_LINK_PUB eruptor_resource eruptor_renderer
COMPILATION_FEATURES cxx_std_23)
