cmake_minimum_required(VERSION 2.8)
project(Bino_Stereo_SDK)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release)
ENDIF()

option(BUILD_USE_SDL2 "Build SDK using sdl2!" ON)

MESSAGE("Build type: " ${CMAKE_BUILD_TYPE})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall  -O3 -march=native ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall   -O3 -march=native")

# Check C++11 or C++0x support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
   add_definitions(-DCOMPILEDWITHC11)
   message(STATUS "Using flag -std=c++11.")
elseif(COMPILER_SUPPORTS_CXX0X)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
   add_definitions(-DCOMPILEDWITHC0X)
   message(STATUS "Using flag -std=c++0x.")
else()
   message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules)

find_package(OpenCV 3.2.0 QUIET)
if(NOT OpenCV_FOUND)
   find_package(OpenCV 2.4.3 QUIET)
   if(NOT OpenCV_FOUND)
      message(FATAL_ERROR "OpenCV > 2.4.3 not found.")
   endif()
endif()

include_directories(
${PROJECT_SOURCE_DIR}
${PROJECT_SOURCE_DIR}/include
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
add_executable(BinoStereo src/BinoStereo.cpp)
target_link_libraries(BinoStereo 
	${OpenCV_LIBS}
	${PROJECT_SOURCE_DIR}/lib/libBino_Stereo_SDK.so
	)

if (BUILD_USE_SDL2)
	find_package(SDL2 QUIET)
	if(NOT SDL2_FOUND)
		message(STATUS "Not found SDL2,BinoImu will not be compiled")
	else()
		find_package(OpenGL)
		include_directories(
			${SDL2_INCLUDE_DIRS}
			${OPENGL_INCLUDE_DIR}
			)
		set(SDL2_LIBRARIES "-L${SDL2_LIBDIR}  -lSDL2")
		add_executable(BinoImu src/BinoImu.cpp src/CLRRenderSDL.cpp)
		target_link_libraries(BinoImu pthread 
			${OpenCV_LIBS}
			${SDL2_LIBRARIES}
			${OPENGL_LIBRARIES}
			${PROJECT_SOURCE_DIR}/lib/libBino_Stereo_SDK.so
			)
	endif()
endif()

add_executable(BinoPoint src/BinoPoint.cpp)
target_link_libraries(BinoPoint 
	${OpenCV_LIBS}
	${PROJECT_SOURCE_DIR}/lib/libBino_Stereo_SDK.so
	)

add_executable(CalibGetImage src/CalibGetImage.cpp)
target_link_libraries(CalibGetImage
	${OpenCV_LIBS}
	${PROJECT_SOURCE_DIR}/lib/libBino_Stereo_SDK.so
	)

add_executable(CalibStereo src/CalibStereo.cpp)
target_link_libraries(CalibStereo
	${OpenCV_LIBS}
	${PROJECT_SOURCE_DIR}/lib/libBino_Stereo_SDK.so
	)
