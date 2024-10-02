cmake_minimum_required (VERSION 3.8)

project(ImaGE)

# Add sub directories
add_subdirectory(Libraries/glfw)

add_subdirectory(ImaGE-Core)
add_subdirectory(ImaGE-Editor)
add_subdirectory(ImaGE-Game)