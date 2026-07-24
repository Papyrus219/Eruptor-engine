include(${CMAKE_SOURCE_DIR}/cmake/modules/libs_maker.cmake)

find_package(glm REQUIRED)

set(PHYSIC_SRC ${CMAKE_CURRENT_LIST_DIR}/src/colision_box.cpp
               ${CMAKE_CURRENT_LIST_DIR}/src/physic_manager.cpp)

Make_library(TARGET eruptor_physic
SOURCES PHYSIC_SRC
LIBRARIES_TO_LINK_PRIV glm::glm eruptor_event eruptor_scene
COMPILATION_FEATURES cxx_std_23
)
