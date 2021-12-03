cmake_minimum_required(VERSION 3.1)

project(OpenGL)

set(CMAKE_CXX_STANDARD 11)

configure_file(CMakeProjectMacros.h.in Project-macros/CMakeProjectMacros.h)

#TODO: Find some other way to add these files
add_executable(OpenGL 
            Source/App.cpp 
            Source/Camera.h
            Source/Camera.cpp
            Source/CameraController.h
            Source/CameraController.cpp
            Source/SaveBase.h
            Source/SaveBase.cpp
            Source/SaveCamera.h
            Source/SaveCamera.cpp
            Source/Path.h
            Source/Path.cpp
            Source/RaytracingContainers.h
            OpenGLTypes/Buffer.h
            OpenGLTypes/Buffer.cpp
            Source/glad.c 
            OpenGLTypes/Shader.cpp 
            OpenGLTypes/Shader.h
            OpenglTypes/Texture.h
            OpenglTypes/TextureParameters.h
            OpenglTypes/Texture.cpp 
            OpenglTypes/TImageLoader.cpp
            Dependencies/stb_image/stb_image.cpp
         )

include_directories( Dependencies/Include  "${PROJECT_SOURCE_DIR}/Source" "${PROJECT_BINARY_DIR}/Project-macros" OpenGLTypes Dependencies)

find_library(GLFW_LIBRARY glfw3 glfw3.lib "${PROJECT_SOURCE_DIR}/Dependencies/Lib")

target_link_libraries(OpenGL LINK_PUBLIC ${GLFW_LIBRARY} opengl32.lib)



