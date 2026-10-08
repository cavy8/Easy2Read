cmake_minimum_required(VERSION 3.21)

# ---- Project Configuration ----
set(PROJECT_NAME "Easy2Read")
set(PROJECT_VERSION 1.4.1)

# CommonLib NG CMake setup
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# ---- Project ----
project(
    ${PROJECT_NAME}
    VERSION ${PROJECT_VERSION}
    LANGUAGES CXX
)

# ---- Include guards ----
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not allowed.")
endif()

# ---- Dependencies ----
include(FetchContent)

# Pin the current ng revision so every build uses the same runtime support.
set(ENABLE_SKYRIM_SE ON CACHE BOOL "" FORCE)
set(ENABLE_SKYRIM_AE ON CACHE BOOL "" FORCE)
set(ENABLE_SKYRIM_VR OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(CommonLibSSE
    GIT_REPOSITORY https://github.com/alandtse/CommonLibSSE-NG.git
    GIT_TAG 94faaed0c60eddd8347767f2d4d29a97c93bde8c
    GIT_SUBMODULES ""
)
FetchContent_MakeAvailable(CommonLibSSE)
include(${commonlibsse_SOURCE_DIR}/cmake/CommonLibSSE.cmake)
find_package(imgui CONFIG REQUIRED)
find_package(directxtk CONFIG REQUIRED)
find_path(SIMPLEINI_INCLUDE_DIRS SimpleIni.h REQUIRED)

# ---- Source Files ----
set(SOURCES
    src/main.cpp
    src/Config/Settings.cpp
    src/Hooks/MenuWatcher.cpp
    src/Hooks/InputHandler.cpp
    src/Hooks/D3D11Hook.cpp
    src/Hooks/MenuControlsHook.cpp
    src/Utils/BookUtils.cpp
    src/Utils/ImageMappings.cpp
    src/Utils/AliasResolver.cpp
    src/UI/Overlay.cpp
    src/UI/BookPromptIcon.cpp
    src/UI/SkyUIFrame.cpp
    src/ImGui/imgui_impl_dx11.cpp
    src/ImGui/imgui_impl_win32.cpp
)

set(HEADERS
    src/PCH.h
    src/Config/Settings.h
    src/Hooks/MenuWatcher.h
    src/Hooks/InputHandler.h
    src/Hooks/D3D11Hook.h
    src/Hooks/MenuControlsHook.h
    src/Utils/BookUtils.h
    src/Utils/ImageMappings.h
    src/Utils/AliasResolver.h
    src/UI/Overlay.h
    src/UI/BookPromptIcon.h
    src/UI/SkyUIFrame.h
    src/UI/SkyUIFrameData.h
    src/ImGui/imgui_impl_dx11.h
    src/ImGui/imgui_impl_win32.h
)

# ---- Create DLL ----
add_commonlibsse_plugin(
    ${PROJECT_NAME}
    AUTHOR "cavy8"
    USE_ADDRESS_LIBRARY
    SOURCES ${SOURCES} ${HEADERS}
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

# ---- Use static runtime to match CommonLibSSE ----
set_property(TARGET ${PROJECT_NAME} PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

# ---- Include directories ----
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${SIMPLEINI_INCLUDE_DIRS}
)

# ---- Precompiled headers ----
target_precompile_headers(${PROJECT_NAME}
    PRIVATE
        src/PCH.h
)

# ---- Link ImGui ----
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        imgui::imgui
        Microsoft::DirectXTK
        d3d11
        dxgi
)

target_link_libraries(${PROJECT_NAME} PUBLIC CommonLibSSE::CommonLibSSE)

# ---- Post-build: Copy to Skyrim mods folder (if environment variable is set) ----
if(DEFINED ENV{SKYRIM_MODS_FOLDER})
    set(MOD_FOLDER "$ENV{SKYRIM_MODS_FOLDER}/${PROJECT_NAME}")
    
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${MOD_FOLDER}/SKSE/Plugins"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:${PROJECT_NAME}>" "${MOD_FOLDER}/SKSE/Plugins/"
        COMMENT "Copying Easy2Read.dll to ${MOD_FOLDER}/SKSE/Plugins/"
        VERBATIM
    )
endif()

# ---- Compiler-specific settings ----
if(MSVC)
    target_compile_options(${PROJECT_NAME}
        PRIVATE
            /W4         # Warning level 4
            /WX-        # Don't treat warnings as errors (for now)
            /permissive-
            /Zc:__cplusplus
            /Zc:preprocessor
            /EHsc
            /utf-8      # Required for fmt library Unicode support
            /Zi         # Generate debug info for all configurations
    )
    
    # Generate PDB for Release builds
    target_link_options(${PROJECT_NAME}
        PRIVATE
            /DEBUG      # Generate PDB
            /OPT:REF    # Optimize references
            /OPT:ICF    # COMDAT folding
    )
    
    # Set PDB output name
    set_target_properties(${PROJECT_NAME} PROPERTIES
        COMPILE_PDB_NAME ${PROJECT_NAME}
        PDB_NAME ${PROJECT_NAME}
    )
endif()

# ---- Copy PDB to mods folder ----
if(DEFINED ENV{SKYRIM_MODS_FOLDER} AND MSVC)
    set(MOD_FOLDER "$ENV{SKYRIM_MODS_FOLDER}/${PROJECT_NAME}")
    
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_PDB_FILE:${PROJECT_NAME}>" "${MOD_FOLDER}/SKSE/Plugins/"
        COMMENT "Copying Easy2Read.pdb to ${MOD_FOLDER}/SKSE/Plugins/"
        VERBATIM
    )
endif()
