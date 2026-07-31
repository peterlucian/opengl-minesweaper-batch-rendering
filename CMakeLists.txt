cmake_minimum_required(VERSION 3.20)

project(OpenGLMinesweeper VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

file(GLOB CPP_SOURCES CONFIGURE_DEPENDS "*.cpp")
file(GLOB C_SOURCES CONFIGURE_DEPENDS "glad/*.c")


add_executable(OpenGLMinesweeper
${CPP_SOURCES}
${C_SOURCES}
)

add_custom_command(TARGET OpenGLMinesweeper POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${PROJECT_SOURCE_DIR}/basic.shader
        $<TARGET_FILE_DIR:OpenGLMinesweeper>/basic.shader
)

add_custom_command(TARGET OpenGLMinesweeper POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/res
        $<TARGET_FILE_DIR:OpenGLMinesweeper>/res
)

target_include_directories(OpenGLMinesweeper PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/glad
    ${PROJECT_SOURCE_DIR}/glm
)

target_sources(OpenGLMinesweeper PRIVATE
    glad/glad.c
)

target_link_libraries(OpenGLMinesweeper PRIVATE
    glfw
    OpenGL::GL
)