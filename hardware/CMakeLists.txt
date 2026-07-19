cmake_policy(SET CMP0135 NEW)

include(FetchContent)
include(${CMAKE_SOURCE_DIR}/cmake/modules/libs_maker.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/modules/shader.cmake)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

FetchContent_Declare( vmahpp
                      URL      https://github.com/YaaZ/VulkanMemoryAllocator-Hpp/releases/download/v3.3.0%2B3/VulkanMemoryAllocator-Hpp-3.3.0.tar.gz
                      URL_HASH SHA256=8c2a0573babe1f86f3241d12b4c9df40bb944649b7f4239c1c611797f8af6cd5)

FetchContent_MakeAvailable(vmahpp)

set(HARDWARE_SRC ${CMAKE_CURRENT_LIST_DIR}/src/hardware/hardware.cpp
                 ${CMAKE_CURRENT_LIST_DIR}/src/hardware/core.cpp
                 ${CMAKE_CURRENT_LIST_DIR}/src/hardware/device.cpp
                 ${CMAKE_CURRENT_LIST_DIR}/src/hardware/window.cpp
                 ${CMAKE_CURRENT_LIST_DIR}/src/hardware/swapchain.cpp
                 ${CMAKE_CURRENT_LIST_DIR}/src/hardware/uniform_buffers.cpp
                 ${CMAKE_CURRENT_LIST_DIR}/src/hardware/pipeline.cpp
                 ${CMAKE_CURRENT_LIST_DIR}/src/hardware/command_manager.cpp
                 ${CMAKE_CURRENT_LIST_DIR}/src/hardware/vma.cpp
                 ${CMAKE_CURRENT_LIST_DIR}/src/hardware/utilities.cpp

                 ${CMAKE_CURRENT_LIST_DIR}/src/hardware/resources/resource_manager.cpp
                 ${CMAKE_CURRENT_LIST_DIR}/src/hardware/resources/texture.cpp
                 ${CMAKE_CURRENT_LIST_DIR}/src/hardware/resources/push_constant.cpp
)

set(EVENT_SRC ${CMAKE_CURRENT_LIST_DIR}/src/event/event_manager.cpp
              ${CMAKE_CURRENT_LIST_DIR}/src/event/event_listener.cpp
)

Make_library(TARGET eruptor_event
SOURCES EVENT_SRC
COMPILATION_FEATURES cxx_std_23)

Make_library(TARGET eruptor_hardware
SOURCES HARDWARE_SRC
LIBRARIES_TO_LINK_PUB Vulkan::Vulkan glm::glm glfw VulkanMemoryAllocator-Hpp::VulkanMemoryAllocator-Hpp
LIBRARIES_TO_LINK_PRIV eruptor_event
COMPILATION_FEATURES cxx_std_23)

add_dependencies(eruptor_hardware main_shader)
target_include_directories(eruptor_hardware PUBLIC ${vmahpp_SOURCE_DIR})
target_include_directories(eruptor_hardware PUBLIC ${vmahpp_SOURCE_DIR}/include)

add_slang_shader_target(main_shader SOURCES ${CMAKE_SOURCE_DIR}/shaders_src/shader.slang)
