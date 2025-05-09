include(../CMake/Compiler.cmake)
add_subdirectory(Thirdparty)

file(GLOB_RECURSE hdr_files CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR} "Src/*.h")
file(GLOB_RECURSE src_files CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR} "Src/*.cpp")

add_library(Common
    ${hdr_files}
    ${src_files}
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
    ${required_preprocessor_definitions}
    RESOURCE_DIR="${resource_directory}"
	GLFW_INCLUDE_NONE
)
target_compile_options(Common PUBLIC
    ${platform_specific_compiler_options}
)
target_precompile_headers(Common PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/pch.h
)
