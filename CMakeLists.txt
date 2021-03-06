cmake_minimum_required(VERSION 3.0)
project(OSControls)
include(CTest)

set(CPACK_GENERATOR "ZIP")
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)

set(LEAP_COMPANY_NAME "Leap Motion")
set(APP_COPYRIGHT "Copyright © 2014 Leap Motion, Inc. All rights reserved.")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")
include(LeapCMakeTemplates)

if(APPLE)
  set(CMAKE_XCODE_ATTRIBUTE_STRIP_INSTALLED_PRODUCT YES)
  set(CMAKE_XCODE_ATTRIBUTE_DEAD_CODE_STRIPPING YES)
endif()

leap_find_external_libraries(x86_vc12)
leap_use_standard_platform_settings()

include(TargetImportedLibraries)
include(AddPCH)
include(ConditionalSources)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

enable_testing()

find_package(Eigen 3.2.1 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Glew 1.9.0 REQUIRED)
if(MSVC)
  find_package(FreeImage REQUIRED)
endif()

add_subdirectory(contrib)
add_subdirectory(src)
include(CPack)

