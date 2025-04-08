set(required_preprocessor_definitions)
set(platform_specific_compiler_options)

# Platform specific definitions
if (WIN32)
    if (MSVC)
        list(APPEND platform_specific_compiler_options "/utf-8")
        list(APPEND required_preprocessor_definitions "_CRT_SECURE_NO_WARNINGS")
    endif()
elseif (UNIX)
    if (APPLE)
    else()
    endif()
endif()
