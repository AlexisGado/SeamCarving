cmake_minimum_required(VERSION 2.9)

 list(APPEND CMAKE_PREFIX_PATH "/usr/local/Qt-5.15.2")

file(TO_CMAKE_PATH "$ENV{IMAGINEPP_ROOT}/CMake" p)
list(APPEND CMAKE_MODULE_PATH "${p}") #For old Imagine++
list(APPEND CMAKE_SYSTEM_FRAMEWORK_PATH /Library/Frameworks) #Mac, why not auto?
find_package(Imagine REQUIRED)

project(SeamCarvingFinal)
add_executable(SeamCarvingFinal main.cpp tools.cpp tools.h resizing.cpp resizing.h)
ImagineUseModules(SeamCarvingFinal Graphics Images)
