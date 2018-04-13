cmake_minimum_required(VERSION 2.8) # Проверка версии CMake.
# Если версия установленой программы
# старее указаной, произайдёт аварийный выход.
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/lib/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)

if(WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif(WIN32)
set(NAME "mm_view")

project(${NAME})
add_executable(${NAME}
    src/mm_view.cpp 
    src/Render.cpp 
    src/blvmap.cpp 
    src/log.cpp 
    src/lodfile.cpp
    src/aeTexture.cpp 
    include/glad.c 
    include/stb_image.cpp)       # Создает исполняемый файл с именем main
find_package(OpenGL REQUIRED)
#find_package(GLUT REQUIRED)
#include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )
include_directories( ${OPENGL_INCLUDE_DIRS}   )

#target_link_libraries(${NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )

target_link_libraries(${NAME} ${OPENGL_LIBRARIES}  glfw3.lib glew32s.lib libpng16_static.lib zlibstatic.lib)

file(GLOB SHADERS
         "resources/*.vs"
         "resources/*.fs"
         "resources/*.gs"
)
foreach(SHADER ${SHADERS})
    if(WIN32)
        add_custom_command(TARGET ${NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} $<TARGET_FILE_DIR:${NAME}>/resources)
    elseif(UNIX AND NOT APPLE)
        file(COPY ${SHADER} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/${CHAPTER})
    elseif(APPLE)
        get_filename_component(SHADERNAME ${SHADER} NAME)
        makeLink(${SHADER} ${CMAKE_CURRENT_BINARY_DIR}/bin/${CHAPTER}/${SHADERNAME} ${NAME})
    endif(WIN32)
endforeach(SHADER)

# из исходника main.cpp
