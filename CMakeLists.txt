cmake_minimum_required (VERSION 3.8)

project(ImaGE-Libraries)

# Add sub directories
# THIS IS FOR LIBRARIES THAT MUST BE BUILT WITH CMAKE
add_subdirectory(Libraries/glfw)
add_subdirectory(Libraries/assimp)
add_subdirectory(Libraries/freetype)
add_subdirectory(Libraries/rttr)