cmake_minimum_required(VERSION 2.8) # Проверка версии CMake.
# Если версия установленой программы
# старее указаной, произайдёт аварийный выход.
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/lib/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)

if(WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif(WIN32)

project(mm_view)
add_executable(mm_view src/mm_view.cpp src/Render.cpp src/log.cpp src/lodfile.cpp src/aeTexture.cpp include/glad.c include/stb_image.cpp)       # Создает исполняемый файл с именем main
find_package(OpenGL REQUIRED)
#find_package(GLUT REQUIRED)
#include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )
include_directories( ${OPENGL_INCLUDE_DIRS}   )

#target_link_libraries(mm_view ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )

target_link_libraries(mm_view ${OPENGL_LIBRARIES}  glfw3.lib glew32s.lib libpng16_static.lib zlibstatic.lib)
# из исходника main.cpp
