cmake_minimum_required(VERSION 3.10)
project(openglcoursejp_project)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
# find_package(GLFW REQUIRED)
find_package(glfw3 3.2 REQUIRED)
# find_package(glfw3 REQUIRED)

include_directories(
	${OPENGL_INCLUDE_DIR}
	${GLUT_INCLUDE_DIR}
	# ${GLFW_INCLUDE_DIR}
	include/
	src/hello_shader/
)

add_executable(main src/hello_shader/main.cpp)
# add_executable(main src/camera_move.cpp)

target_link_libraries(main 
	${OPENGL_LIBRARIES}
	${GLUT_LIBRARIES}
	# ${GLFW_LIBRARIES}
	glfw
)


message("-- OpenGL --")
# https://cmake.org/cmake/help/v3.10/module/FindOpenGL.html
# message(STATUS "version : ${OPENGL_FOUND}")
# message(STATUS "OpenGL version: ${OPENGL_VERSION}")
message(STATUS "libs : ${OPENGL_LIBRARIES}")
message(STATUS "include dirs : ${OPENGL_INCLUDE_DIR}")

