include_guard()

option(PAP_CMAKE_SILENCE_MAKE_TEST_PROGRAM OFF "Turn off  Make_test_program debug messages.")

function(Make_test_program)
    set(no_values )
    set(single_values TARGET SOURCES)
    set(multi_values COMPILATION_FEATURES LIBRARIES_TO_LINK)

    cmake_parse_arguments(
        arg
        "${no_values}" "${single_values}" "${multi_values}"
        ${ARGN})

    if(NOT PAP_CMAKE_SILENCE_MAKE_TEST_PROGRAM)
        message("Setup test program [ ${arg_TARGET} ] : Start")
    endif()

    add_executable( ${arg_TARGET} ${arg_SOURCES})
    target_compile_features(${arg_TARGET} PRIVATE ${arg_COMPILATION_FEATURES})
    target_link_libraries(${arg_TARGET} PRIVATE ${arg_LIBRARIES_TO_LINK})
    install(TARGETS ${arg_TARGET} RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bin)

    if(NOT PAP_CMAKE_SILENCE_MAKE_TEST_PROGRAM)
        message("Setup test program [ ${arg_TARGET} ] : Done")
    endif()
endfunction()
