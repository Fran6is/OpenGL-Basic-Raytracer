# A basic raytracer using OpenGL and C++
<hr>

## Features / Pipeline: 

- Phong illumination
- Reflection
- **Render passes:** Geometry, Light, and Post process
- **Post processing:** HDR mapping, Gamma correction
- **Traced shapes:** Spheres and Planes
- **Lights:** Directional and point (W shadow casts )

<hr>

## Key Bindings
- **ESC:** Exit application

- **W A S D:** Camera translation

- **Mouse movement:** Camera rotation

- **Mouse scroll:** Camera zoom  (FOV)  

<hr>

***Platform build***: x86 / all configurations

***OpenGL***: GLSL3.3, OpenGL32.lib, glad

***Windowing***: GLFW

***Math library***: GLM

<hr>

## CMake build (With CMake installed and command prompt opened in project's root directory)

- Generate project files for x86 platform ( in this case win32 )
```
[Project root directory]: cmake . -B build -A Win32
```
- Build project
```
[Project root directory]: cmake --build build -t Raytracing
[Project root directory]: cd build/Debug
[Project root directory / build / Debug]: Raytracing
```



<hr>

![img](https://res.cloudinary.com/asuelimf/image/upload/v1639091954/ProjectScreenshots/Raytracing1_zrvzmm.png)

![img](https://res.cloudinary.com/asuelimf/image/upload/v1638944428/ProjectScreenshots/raytracing3_uk8zy7.png)

![img](https://res.cloudinary.com/asuelimf/image/upload/v1638944428/ProjectScreenshots/raytracing2_ul8mf4.png)








