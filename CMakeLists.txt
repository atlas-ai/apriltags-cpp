# swatbotics/apriltags-cpp/CMakeLists.txt

cmake_minimum_required(VERSION 3.5.1)

project(APRILTAGS)

set(CMAKE_CXX_STANDARD 11) # for std::chrono
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

if(WIN32)
	set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${LIBRARY_OUTPUT_PATH})
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${LIBRARY_OUTPUT_PATH})
else()
	set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
endif()

if(APPLE)
	include_directories(/opt/local/include) # MacPorts
	link_directories(/opt/local/lib)
	find_library(OPENGL_LIBRARY OpenGL)
else()
	find_library(OPENGL_LIBRARY GL)
	find_library(GLU_LIBRARY GLU)
	set(OPENGL_LIBRARY ${OPENGL_LIBRARY} ${GLU_LIBRARY})
endif()

find_library(GLUT_LIBRARY glut)

include(FindPkgConfig)

if(UNIX)
	pkg_search_module(OPENCV REQUIRED opencv>=3.4)
	include_directories(${OPENCV_INCLUDE_DIRS})
	
	pkg_check_modules(CAIRO cairo)
	if(${CAIRO_FOUND})
		add_definitions(-DMZ_HAVE_CAIRO)
	endif()
else()
	find_package(OpenCV REQUIRED)
	if(${OpenCV_FOUND})
		if( ${OpenCV_VERSION}} VERSION_GREATER "3.4")
			include_directories(${OpenCV_INCLUDE_DIRS})
		else()
			message(FATAL_ERROR "OpenCV found but version too old. current version: " ${OpenCV_FIND_VERSION} ". required: 3.4 or newer.")
		endif()
	else()
		message(FATAL_ERROR "OpenCV not found!")
	endif()
endif()

find_package( CGAL QUIET COMPONENTS  )

if (${CGAL_FOUND})
	include( ${CGAL_USE_FILE} )
	add_definitions(-DHAVE_CGAL)
	find_package( Boost REQUIRED )
	
	if(MSVC)
		add_definitions("/EHsc") # To work with boost header only
								 # otherwise may give linking error
	endif(MSVC)
else()
	message("CGAL not found, can not use new quad detector")
endif()

if(UNIX)
	set(CMAKE_C_FLAGS "-Wall -g")
	set(CMAKE_CXX_FLAGS "-Wall -g")

	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wsign-compare -frounding-math")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-compare -frounding-math")
	
	set(CMAKE_C_FLAGS_DEBUG "-O")
	set(CMAKE_CXX_FLAGS_DEBUG "-O")

	set(CMAKE_C_FLAGS_RELEASE "-O2")
	set(CMAKE_CXX_FLAGS_RELEASE "-O2")
endif()

add_subdirectory(src)
