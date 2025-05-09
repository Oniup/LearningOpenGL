include(../CMake/Compiler.cmake)
add_subdirectory(Thirdparty)

file(GLOB_RECURSE HDR_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR} "Src/*.h")
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR} "Src/*.cpp")

add_library(Common
    ${HDR_FILES}
    ${SRC_FILES}
)
target_include_directories(Common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/
    ${glad_INCLUDE_DIRS}
    ${glfw_INCLUDE_DIRS}
    ${glm_INCLUDE_DIRS}
    ${imgui_INCLUDE_DIRS}
    ${stb_INCLUDE_DIRS}
    ${assimp_INCLUDE_DIRS}
)
target_link_libraries(Common PUBLIC
    glm
    glad
    glfw
    imgui
    stb
    assimp::assimp
)
target_compile_definitions(Common PUBLIC
    ${REQUIRED_PREPROCESSOR_DEFINITIONS}
    RESOURCE_DIR="${RESOURCE_DIRECTORY}"
	GLFW_INCLUDE_NONE
)
target_compile_options(Common PUBLIC
    ${PLATFORM_SPECIFIC_COMPILER_OPTIONS}
)
target_precompile_headers(Common PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/pch.h
)
