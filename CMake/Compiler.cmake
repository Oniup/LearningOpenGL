set(REQUIRED_PREPROCESSOR_DEFINITIONS)
set(PLATFORM_SPECIFIC_COMPILER_OPTIONS)

# Platform specific definitions
if (WIN32)
    if (MSVC)
        list(APPEND PLATFORM_SPECIFIC_COMPILER_OPTIONS "/utf-8")
        list(APPEND REQUIRED_PREPROCESSOR_DEFINITIONS "_CRT_SECURE_NO_WARNINGS")
    endif()
elseif (UNIX)
    if (APPLE)
    else()
    endif()
endif()
