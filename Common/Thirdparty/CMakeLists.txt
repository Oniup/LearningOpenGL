# Graphics Libraries
# ---------------------------------------------------------------------------------------------------------------------
# OpenGL Maths Library
option(GLM_BUILD_INSTALL OFF)
option(GLM_ENABLE_CXX_20 OFF)
add_subdirectory(glm)
set_target_properties(glm PROPERTIES FOLDER "Common Dependencies")

# GLFW
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_DOCS OFF)
option(GLFW_INSTALL OFF)
add_subdirectory(glfw)
set_target_properties(glfw PROPERTIES FOLDER "Common Dependencies")

# GLAD
add_library(glad STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/glad/include/glad/glad.h
    ${CMAKE_CURRENT_SOURCE_DIR}/glad/include/KHR/khrplatform.h
    ${CMAKE_CURRENT_SOURCE_DIR}/glad/src/glad.c
)
target_include_directories(glad
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/glad/include/
)
set_target_properties(glad PROPERTIES FOLDER "Common Dependencies")

# STB
add_library(stb INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/stb/stb/stb_image.h
    ${CMAKE_CURRENT_SOURCE_DIR}/stb/stb/stb_image_resize2.h
    ${CMAKE_CURRENT_SOURCE_DIR}/stb/stb/stb_image_write.h
    ${CMAKE_CURRENT_SOURCE_DIR}/stb/stb/stb_perlin.h
    ${CMAKE_CURRENT_SOURCE_DIR}/stb/stb/stb_sprintf.h
)
target_include_directories(stb
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/stb
)
set_target_properties(stb PROPERTIES FOLDER "Common Dependencies")

# Utility Libraries
# ---------------------------------------------------------------------------------------------------------------------

# ImGui
add_library(imgui STATIC
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui/imconfig.h
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui/imgui_demo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui/imgui_draw.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui/imgui_internal.h
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui/imgui_tables.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui/imgui_widgets.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui/imgui.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui/imgui.h
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui/imstb_rectpack.h
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui/imstb_textedit.h
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui/imstb_truetype.h

    # GLFW
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui/backends/imgui_impl_glfw.h
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui/backends/imgui_impl_glfw.cpp
    # OpenGL
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui/backends/imgui_impl_opengl3.h
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui
    ${glad_INCLUDE_DIRS}
    ${glfw_INCLUDE_DIRS}
)
target_link_libraries(imgui
    PUBLIC
    glfw
    glad
)
if (WIN32 AND MSVC)
    target_compile_definitions(imgui
        PUBLIC
        "_CRT_SECURE_NO_WARNINGS"
    )
endif()
set_target_properties(imgui PROPERTIES FOLDER "Common Dependencies")

# Assimp
option(ASSIMP_BUILD_TESTS OFF)
option(ASSIMP_INSTALL OFF)
option(ASSIMP_INSTALL_PDB OFF)
add_subdirectory(Assimp)
set_target_properties(assimp PROPERTIES FOLDER "Common Dependencies")
set_target_properties(zlibstatic PROPERTIES FOLDER "Common Dependencies/Assimp")
set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTIES FOLDER "Common Dependencies/Assimp")
