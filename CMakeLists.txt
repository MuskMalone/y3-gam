project(ImaGE-Libraries)

# Add sub directories
# THIS IS FOR LIBRARIES THAT MUST BE BUILT WITH CMAKE
add_subdirectory(Libraries/glfw)
add_subdirectory(Libraries/assimp)
add_subdirectory(Libraries/freetype)

option(BUILD_RTTR "Build RTTR library" OFF)
if (BUILD_RTTR)
    add_subdirectory(Libraries/rttr)
endif()