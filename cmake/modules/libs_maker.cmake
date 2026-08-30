include_guard()

option(PAP_CMAKE_SILENCE_MAKE_LIBRARY OFF "Turn off  Make_library debug messages.")
option(PAP_CMAKE_SILENCE_MAKE_HEADER_LIBRARY OFF "Turn off  Make_header_library debug messages.")

function(Make_library)
    set(no_values)
    set(single_values TARGET SOURCES)
    set(multi_values COMPILATION_FEATURES LIBRARIES_TO_LINK_PUB LIBRARIES_TO_LINK_PRIV)

    cmake_parse_arguments(
        arg
        "${no_values}" "${single_values}" "${multi_values}"
        ${ARGN})

    if(NOT PAP_CMAKE_SILENCE_MAKE_LIBRARY)
        message("Setup ${arg_TARGET} library: Start")
    endif()

    add_library(${arg_TARGET} ${${arg_SOURCES}})

    target_include_directories(${arg_TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include/)

    target_compile_features(${arg_TARGET} PRIVATE ${arg_COMPILATION_FEATURES})

    if(arg_LIBRARIES_TO_LINK_PUB)
        target_link_libraries(${arg_TARGET} PUBLIC ${arg_LIBRARIES_TO_LINK_PUB})
    endif()

    if(arg_LIBRARIES_TO_LINK_PRIV)
        target_link_libraries(${arg_TARGET} PRIVATE ${arg_LIBRARIES_TO_LINK_PRIV})
    endif()

    if(NOT PAP_CMAKE_SILENCE_MAKE_LIBRARY)
        message("Setup ${arg_TARGET} library: Done")
    endif()
endfunction()

function(Make_header_library)
    set(no_values)
    set(single_values TARGET)
    set(multi_values COMPILATION_FEATURES LIBRARIES_TO_LINK)

    cmake_parse_arguments(
        arg
        "${no_values}" "${single_values}" "${multi_values}"
        ${ARGN})

    if(NOT PAP_CMAKE_SILENCE_MAKE_HEADER_LIBRARY)
        message("Setup ${arg_TARGET} library: Start")
    endif()

    add_library(${arg_TARGET} INTERFACE)

    target_include_directories(${arg_TARGET} INTERFACE ${PROJECT_SOURCE_DIR}/include/libs/)

    target_compile_features(${arg_TARGET} INTERFACE cxx_std_23)
    target_link_libraries(${arg_TARGET} INTERFACE ${LIBRARIES_TO_LINK})

    if(NOT PAP_CMAKE_SILENCE_MAKE_HEADER_LIBRARY)
        message("Setup ${arg_TARGET} library: Done.")
    endif()

endfunction()

