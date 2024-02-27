
cmake_minimum_required(VERSION 3.20)
project(3g4-lab)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")

function(add_shader TARGET SHADER)
    # find_program(GLSLC glslc)

    set(current-shader-path ${CMAKE_CURRENT_SOURCE_DIR}/src/${SHADER})
    set(current-output-path ${CMAKE_BINARY_DIR}/shaders/${SHADER}.spv)

    # Add a custom command to compile GLSL to SPIR-V.
    get_filename_component(current-output-dir ${current-output-path} DIRECTORY)
    file(MAKE_DIRECTORY ${current-output-dir})

    add_custom_command(
           OUTPUT ${current-output-path}
           COMMAND ${Vulkan_GLSLC_EXECUTABLE} -o ${current-output-path} ${current-shader-path}
           DEPENDS ${current-shader-path}
           IMPLICIT_DEPENDS CXX ${current-shader-path}
           VERBATIM)
    #add_custom_command(
    #       OUTPUT ${current-output-path}
    #       COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} -V -o ${current-output-path} ${current-shader-path}
    #       DEPENDS ${current-shader-path}
    #       IMPLICIT_DEPENDS CXX ${current-shader-path}
    #       VERBATIM)

    # Make sure our build depends on this output.
    set_source_files_properties(${current-output-path} PROPERTIES GENERATED TRUE)
    target_sources(${TARGET} PRIVATE ${current-output-path})
    target_sources(${TARGET} PRIVATE ${current-shader-path})
	
endfunction(add_shader)

# Find SDL2 and OpenGL
#if (WIN32)
# use next line if wxwidgets is copied to a specific folder
#set(wxWidgets_ROOT_DIR D:/code/wxWidgets-3.2.2.1)
#endif()
find_package(wxWidgets REQUIRED COMPONENTS net core base gl)
find_package(OpenGL REQUIRED)
set(glm_DIR ${CMAKE_SOURCE_DIR}/glm/cmake/glm)
find_package(glm REQUIRED)

if(APPLE)
	if(DEFINED ENV_WX_CONFIG)
		set(ENV{WX_CONFIG} ${ENV_WX_CONFIG})
	endif()
endif()

if (wxWidgets_VERSION_STRING LESS 3.2)
    message(FATAL_ERROR "Found wxWidgets Version $(wxWidgets_VERSION_STRING) found. Required >= 3.2.")
endif()

if(UNIX AND NOT APPLE)
    # for Linux, BSD, Solaris, Minix
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
	include_directories(${GTK3_INCLUDE_DIRS})
	link_directories(${GTK3_LIBRARY_DIRS})
	add_definitions(${GTK3_CFLAGS_OTHER})
	
	find_package(Wayland REQUIRED)
	include_directories(${WAYLAND_INCLUDE_DIRS})

	find_package(X11 REQUIRED)
	include_directories(${X11_INCLUDE_DIR})
endif()

find_package(Vulkan REQUIRED)

add_library(libisosurf STATIC IMPORTED)
if(WIN32)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  SET_PROPERTY(TARGET libisosurf PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/isosurf/Debugx64/isosurfx64d.lib)
  else()
  SET_PROPERTY(TARGET libisosurf PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/isosurf/Releasex64/isosurfx64.lib)
  endif()
elseif(APPLE)
SET_PROPERTY(TARGET libisosurf PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/isosurf/libisosurf_apple.a)
else()
SET_PROPERTY(TARGET libisosurf PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/isosurf/libisosurf.a)
endif()

# Define executable target
include(${wxWidgets_USE_FILE})
#file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS *.cpp *.h)
if(APPLE)
set(APPLE_FILES src/CocoaSurface.h src/CocoaSurface.mm)
endif()
set(SOURCE_FILES src/3g4_lab_app.cpp src/3g4_lab_app.h src/VulkanWindow.cpp src/VulkanWindow.h src/VulkanException.cpp src/VulkanException.h src/VulkanCanvas.cpp src/VulkanCanvas.h src/wxWayland.h src/isosurf.h src/linear_interp.h src/linear_interp.cpp src/exercises.cpp src/stb_image.h src/vk_mem_alloc.h src/guistructs.h)
include_directories(${OPENGL_INCLUDE_DIR} ${CMAKE_BINARY_DIR} ${Vulkan_INCLUDE_DIRS})

if(WIN32)
	add_executable(3g4-lab WIN32 ${SOURCE_FILES})
	target_sources(3g4-lab PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/3g4-lab.exe.manifest)
else()
	if(APPLE)
	add_executable(3g4-lab ${SOURCE_FILES} ${APPLE_FILES})
	else()
	add_executable(3g4-lab ${SOURCE_FILES})
	endif()
endif()

target_link_libraries(3g4-lab ${SDL2_LIBS} ${OPENGL_LIBRARIES} ${wxWidgets_LIBRARIES})

if(UNIX AND NOT APPLE)
    # for Linux, BSD, Solaris, Minix
	target_link_libraries(3g4-lab ${GTK3_LIBRARIES})
	
	target_link_libraries(3g4-lab ${WAYLAND_LIBRARIES})

	target_link_libraries(3g4-lab ${X11_LIBRARIES})
endif()

target_link_libraries(3g4-lab ${Vulkan_LIBRARIES})

target_link_libraries(3g4-lab glm::glm)

target_link_libraries(3g4-lab libisosurf)

set_property(TARGET 3g4-lab PROPERTY CXX_STANDARD 17)
set_property(TARGET 3g4-lab PROPERTY CXX_STANDARD_REQUIRED ON)

file(GLOB VERT_SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.vert)
file(GLOB FRAG_SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.frag)

foreach(shader ${VERT_SHADERS})
	cmake_path(GET shader FILENAME shader_filename)
	add_shader(3g4-lab ${shader_filename})
endforeach()

foreach(shader ${FRAG_SHADERS})
	cmake_path(GET shader FILENAME shader_filename)
	add_shader(3g4-lab ${shader_filename})
endforeach()

add_custom_command ( TARGET 3g4-lab POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_if_different
${CMAKE_CURRENT_SOURCE_DIR}/src/ct_data.dat $<TARGET_FILE_DIR:3g4-lab>
)

add_custom_command ( TARGET 3g4-lab POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_if_different
${CMAKE_CURRENT_SOURCE_DIR}/src/synthetic_data.dat $<TARGET_FILE_DIR:3g4-lab>
)

add_custom_command ( TARGET 3g4-lab POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/src/cubemap/ ${CMAKE_BINARY_DIR}/cubemap
)
