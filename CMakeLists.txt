cmake_minimum_required(VERSION 3.1)

project(Raytracing)

set(CMAKE_CXX_STANDARD 11)

configure_file(CMakeProjectMacros.h.in Project-macros/CMakeProjectMacros.h)

add_executable(Raytracing 
            Source/App.cpp 
            Source/GlmTransformsPrint.h
            Source/GlmTransformsPrint.cpp
            Source/SendDataToShader.h
            Source/SendDataToShader.cpp
            Source/Camera.h
            Source/Camera.cpp
            Source/CameraController.h
            Source/CameraController.cpp
            Source/Path.h
            Source/Path.cpp
            Source/FilesCopy.h
            Source/FilesCopy.cpp
            Source/RaytracingSceneStructs.h
            OpenGLTypes/Buffer.h
            OpenGLTypes/Buffer.cpp
            Source/glad.c 
            OpenGLTypes/Shader.cpp 
            OpenGLTypes/Shader.h
            OpenglTypes/Texture.h
            OpenglTypes/TextureParameters.h
            OpenglTypes/Texture.cpp 
            OpenglTypes/ImageLoader.h
            OpenglTypes/ImageLoader.cpp
            OpenGLTypes/Framebuffer.h
            OpenGLTypes/Framebuffer.cpp
            OpenGLTypes/Cubemap.h
            OpenGLTypes/Cubemap.cpp
            Dependencies/stb_image/stb_image.cpp
         )

include_directories( Dependencies/Include  "${PROJECT_SOURCE_DIR}/Source" "${PROJECT_BINARY_DIR}/Project-macros" OpenGLTypes Dependencies)

find_library(GLFW_LIBRARY glfw3 glfw3.lib "${PROJECT_SOURCE_DIR}/Dependencies/Lib")

target_link_libraries(Raytracing LINK_PUBLIC ${GLFW_LIBRARY} opengl32.lib)



