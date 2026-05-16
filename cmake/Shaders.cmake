find_program(SLANGC_EXECUTABLE slangc REQUIRED)
set(SHADERS_DIR ${CMAKE_SOURCE_DIR}/assets/shaders/compiled)

function(add_shader_targets TARGET)
    cmake_parse_arguments("SHADER" "" "" "SOURCES;ENTRIES" ${ARGN})
    set(SHADERS_DIR ${CMAKE_CURRENT_LIST_DIR}/assets/shaders/compiled)

    add_custom_command(
        OUTPUT ${SHADERS_DIR}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADERS_DIR}
    )

    set(SHADER_OUTPUTS)

    foreach(SOURCE ${SHADER_SOURCES})
        get_filename_component(NAME ${SOURCE} NAME_WE)

        set(ENTRY_ARGS)
        foreach(ENTRY ${SHADER_ENTRIES})
            list(APPEND ENTRY_ARGS -entry ${ENTRY})
        endforeach()

        if(RHI_BACKEND STREQUAL "metal")
            set(OUTPUT ${SHADERS_DIR}/${NAME}.metallib)
            add_custom_command(
                OUTPUT ${OUTPUT}
                COMMAND ${SLANGC_EXECUTABLE} ${SOURCE} -target metallib ${ENTRY_ARGS} -o ${OUTPUT}
                DEPENDS ${SHADERS_DIR} ${SOURCE}
                COMMENT "Compiling ${NAME}.metallib"
                VERBATIM
            )
        else()
            set(OUTPUT ${SHADERS_DIR}/${NAME}.spv)
            add_custom_command(
                OUTPUT ${OUTPUT}
                COMMAND ${SLANGC_EXECUTABLE} ${SOURCE} -target spirv -profile spirv_1_4 -emit-spirv-directly -fvk-use-entrypoint-name ${ENTRY_ARGS} -o ${OUTPUT}
                DEPENDS ${SHADERS_DIR} ${SOURCE}
                COMMENT "Compiling ${NAME}.spv"
                VERBATIM
            )
        endif()

        list(APPEND SHADER_OUTPUTS ${OUTPUT})
    endforeach()

    add_custom_target(${TARGET} DEPENDS ${SHADER_OUTPUTS})
endfunction()