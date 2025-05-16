cmake_minimum_required(VERSION 3.16...3.31)

cmake_policy(SET CMP0003 NEW) # Libraries linked via full path != linker search path error
cmake_policy(SET CMP0010 NEW) # Syntax problems are errors

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SUPPRESS_REGENERATION TRUE) # Removes uncessessary cmake files when generating MSVC project file
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE) #link time optimization

set(RESOURCE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Resources" CACHE INTERNAL "")
set(BINARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Build/${CMAKE_BUILD_TYPE}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BINARY_OUTPUT_DIRECTORY}/Archive")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BINARY_OUTPUT_DIRECTORY}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BINARY_OUTPUT_DIRECTORY}")

project(
    "Learning OpenGL"
    LANGUAGES C CXX
)

include(CMake/Compiler.cmake)
add_subdirectory(Common)

function(Add EXEC_NAME)
    file(GLOB HDR_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR} "Examples/${EXEC_NAME}/*.h")
    file(GLOB SRC_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR} "Examples/${EXEC_NAME}/*.cpp")

    # Shaders
    file(GLOB SHADER_VERT_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR} "Examples/${EXEC_NAME}/*.vert")
    file(GLOB SHADER_FRAG_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR} "Examples/${EXEC_NAME}/*.frag")
    file(GLOB SHADER_GEOM_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR} "Examples/${EXEC_NAME}/*.geom")
    file(GLOB SHADER_COMP_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR} "Examples/${EXEC_NAME}/*.comp")

    # Image files
    file(GLOB IMAGE_JPG_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR} "Examples/${EXEC_NAME}/*.jpg")
    file(GLOB IMAGE_PNG_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR} "Examples/${EXEC_NAME}/*.png")

    add_executable(${EXEC_NAME}
        ${HDR_FILES}
        ${SRC_FILES}

        ${SHADER_VERT_FILES}
        ${SHADER_FRAG_FILES}
        ${SHADER_GEOM_FILES}
        ${SHADER_COMP_FILES}

        ${IMAGE_JPG_FILES}
        ${IMAGE_PNG_FILES}
    )
    target_include_directories(${EXEC_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Examples/${EXEC_NAME}
        ${Common_INCLUDE_DIRS}
    )
    target_link_libraries(${EXEC_NAME} PUBLIC
        Common
    )
    target_compile_definitions(${EXEC_NAME} PUBLIC
        PROJECT_NAME="${EXEC_NAME}"
        PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}/Examples/${EXEC_NAME}"
    )
    target_compile_options(${EXEC_NAME} PUBLIC
        ${platform_specific_compiler_options}
    )
    if (WIN32 AND MSVC)
        set_target_properties(${EXEC_NAME} PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY "${BINARY_OUTPUT_DIRECTORY}/"
        )
    endif()
endfunction()

# Basics
Add(1_HelloTriangle)
Add(2_HelloRect)
Add(3_Texture)
Add(4_ProjectionViewAndModel)
Add(5_FpsCameraControls)

# Phong Lighting
Add(6_BasicLights_Phong)
Add(7_BasicLights_Materials)
Add(8_BasicLights_LightCasters)

Add(9_ModelLoading)
Add(10_DepthAndStencilTesting)
