cmake_minimum_required(VERSION 3.16...3.31)

cmake_policy(SET CMP0003 NEW) # Libraries linked via full path != linker search path error
cmake_policy(SET CMP0010 NEW) # Syntax problems are errors

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SUPPRESS_REGENERATION TRUE) # Removes uncessessary cmake files when generating MSVC project file
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE) #link time optimization

set(resource_directory "${CMAKE_CURRENT_SOURCE_DIR}/Resources" CACHE INTERNAL "")
set(binary_output_directory "${CMAKE_CURRENT_SOURCE_DIR}/Build/${CMAKE_BUILD_TYPE}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${binary_output_directory}/Archive")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${binary_output_directory}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${binary_output_directory}")

project(
    "Learning OpenGL"
    LANGUAGES C CXX
)

include(CMake/Compiler.cmake)
add_subdirectory(Common)

function(Add exec_name)
    file(GLOB hdr_files CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR} "Examples/${exec_name}/*.h")
    file(GLOB src_files CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR} "Examples/${exec_name}/*.cpp")

    # Shaders
    file(GLOB shader_vert_files CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR} "Examples/${exec_name}/*.vert")
    file(GLOB shader_frag_files CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR} "Examples/${exec_name}/*.frag")
    file(GLOB shader_geom_files CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR} "Examples/${exec_name}/*.geom")
    file(GLOB shader_comp_files CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR} "Examples/${exec_name}/*.comp")

    # Image files
    file(GLOB image_jpg_files CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR} "Examples/${exec_name}/*.jpg")
    file(GLOB image_png_files CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR} "Examples/${exec_name}/*.png")

    add_executable(${exec_name}
        ${hdr_files}
        ${src_files}

        ${shader_vert_files}
        ${shader_frag_files}
        ${shader_geom_files}
        ${shader_comp_files}

        ${image_jpg_files}
        ${image_png_files}
    )
    target_include_directories(${exec_name} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Examples/${exec_name}
        ${Common_INCLUDE_DIRS}
    )
    target_link_libraries(${exec_name} PUBLIC
        Common
    )
    target_compile_definitions(${exec_name} PUBLIC
        PROJECT_NAME="${exec_name}"
        PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}/Examples/${exec_name}"
    )
    target_compile_options(${exec_name} PUBLIC
        ${platform_specific_compiler_options}
    )
    if (WIN32 AND MSVC)
        set_target_properties(${exec_name} PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY "${binary_output_directory}/"
        )
    endif()
endfunction()

Add(1HelloTriangle)
Add(2HelloRect)
Add(3Texture)
Add(4ProjectionViewAndModel)
