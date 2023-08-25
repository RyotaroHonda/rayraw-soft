cmake_minimum_required(VERSION 3.13)
project(RayrawSoftware)

message(STATUS "Project name: " ${PROJECT_NAME})

set(my_cxx_options  "-Wall")
set(my_cxx_features "cxx_std_17")

message(STATUS "GCC options:  " ${my_cxx_options})
message(STATUS "C++ standard: " ${my_cxx_features})
message(STATUS "")

include(GNUInstallDirs)
message(STATUS "CMAKE_INSTALL_PREFIX:     " ${CMAKE_INSTALL_PREFIX} )
message(STATUS "CMAKE_INSTALL_BINDIR:     " ${CMAKE_INSTALL_BINDIR} )
message(STATUS "CMAKE_INSTALL_INCLUDEDIR: " ${CMAKE_INSTALL_INCLUDEDIR} )
message(STATUS "CMAKE_INSTALL_LIBDIR:     " ${CMAKE_INSTALL_LIBDIR} )
message(STATUS "CMAKE_INSTALL_DATADIR:    " ${CMAKE_INSTALL_DATADIR} )
message(STATUS "")

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)
set(CMAKE_INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")

if(APPLE)
  message(STATUS "[BUILD on APPLE]")
elseif(UNIX)
  message(STATUS "[BUILD on UNIX AND NOT APPLE]")
  set(CMAKE_EXE_LINKER_FLAGS     "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-as-needed,--enable-new-dtags")
  set(CMAKE_SHARED_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-as-needed,--enable-new-dtags")
endif()

message(STATUS "CMAKE_EXE_LINKER_FLAGS:    " ${CMAKE_EXE_LINKER_FLAGS} )
message(STATUS "CMAKE_SHARED_LINKER_FLAGS: " ${CMAKE_SHARED_LINKER_FLAGS} )
message(STATUS "")

find_package(HulCore REQUIRED)
include(CMakePrintHelpers)

if(HulCore_FOUND)
  cmake_print_properties(TARGETS HulCore PROPERTIES INTERFACE_INCLUDE_DIRECTORIES)
else()
  message(FITAL "HulCore is not found")
endif()

add_subdirectory(YaenamiControl)
add_subdirectory(TrgFw)
