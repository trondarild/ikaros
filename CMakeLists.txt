# Set cmake version
cmake_minimum_required(VERSION 2.6)

# elseif
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)



# Project name is not mandatory, but you should use it
project(ikaros)
MESSAGE( STATUS ${CMAKE_SYSTEM_NAME})


 if(WIN32)
	MESSAGE( STATUS "Detected WIN32 system")
	add_definitions(-DWIN32)

 elseif(APPLE)
 	MESSAGE( STATUS "Detected APPLE system")
	add_definitions(-DMAC_OS_X)

 	# Set compiler
	set(CMAKE_C_COMPILER "clang")
	set(CMAKE_CXX_COMPILER "clang++")
	
   	#SET(GUI_TYPE MACOSX_BUNDLE)
   	INCLUDE_DIRECTORIES ( /Developer/Headers/FlatCarbon )
   	FIND_LIBRARY(ACCELERATE_LIBRARY Accelerate)
	FIND_LIBRARY(COCOA_LIBRARY Cocoa)
	FIND_LIBRARY(QTKIT_LIBRARY QTKit)
    FIND_LIBRARY(QUARTZ_LIBRARY Quartz)
  
  	MARK_AS_ADVANCED (ACCELERATE_LIBRARY
                     COCOA_LIBRARY
                     QTKIT_LIBRARY
                     QUARTZ_LIBRARY)
   SET(EXTRA_LIBS ${ACCELERATE_LIBRARY} ${COCOA_LIBRARY} ${QTKIT_LIBRARY} ${QUARTZ_LIBRARY})
 else()
 	MESSAGE( STATUS "Not Windows ... not Apple... Linux system?")
	add_definitions(-DMAC_OS_X)
	set(CMAKE_C_COMPILER "gcc")
	set(CMAKE_CXX_COMPILER "g++")
	MESSAGE( STATUS "UNIX")
	add_definitions(-DLINUX)
 endif()



# Set Ikaros root path 
set(IKAROS_SOURCE_DIR ${PROJECT_SOURCE_DIR})

# Set where to put binary file
set(EXECUTABLE_OUTPUT_PATH ${IKAROS_SOURCE_DIR}/bin)

# Include the source directory
INCLUDE_DIRECTORIES(Source/)

# Add sub directory
add_subdirectory(Source)

# Crete a binary with source files
ADD_EXECUTABLE(ikaros ${SOURCE})

# Check libraries

# BLAS
FIND_PACKAGE(BLAS)
find_package (BLAS)
if (BLAS_FOUND)
  include_directories(${BLAS_INCLUDE_DIRS})
  target_link_libraries (ikaros ${BLAS_LIBRARIES})
endif (BLAS_FOUND)

# JPEG
FIND_PACKAGE(JPEG)
find_package (JPEG)
if (JPEG_FOUND)
  include_directories(${JPEG_INCLUDE_DIRS})
  target_link_libraries (ikaros ${JPEG_LIBRARIES})
endif (JPEG_FOUND)

# PNG
FIND_PACKAGE(PNG)
find_package (PNG)
if (PNG_FOUND)
  include_directories(${PNG_INCLUDE_DIRS})
  target_link_libraries (ikaros ${PNG_LIBRARIES})
endif (PNG_FOUND)

# Add extra libraries when linking
TARGET_LINK_LIBRARIES(ikaros ${EXTRA_LIBS} )

# Clean 

#SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Remote Operations On Files")
#SET(CPACK_PACKAGE_VENDOR "Rachid Koucha")
##SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
##SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
#SET(CPACK_PACKAGE_VERSION_MAJOR "1")
#SET(CPACK_PACKAGE_VERSION_MINOR "0")
#SET(CPACK_PACKAGE_VERSION_PATCH "0")
#SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
##SET(CPACK_STRIP_FILES "bin/MyExecutable")
##SET(CPACK_SOURCE_STRIP_FILES "")
#SET(CPACK_PACKAGE_EXECUTABLES "roof" "FTP client")
#INCLUDE(CPack)