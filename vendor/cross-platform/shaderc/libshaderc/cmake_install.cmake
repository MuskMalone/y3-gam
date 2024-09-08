# Install script for directory: C:/Users/terra/OneDrive/Documents/references/y3/MoravaEngine/vendor/cross-platform/shaderc/libshaderc

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/shaderc" TYPE FILE FILES
    "C:/Users/terra/OneDrive/Documents/references/y3/MoravaEngine/vendor/cross-platform/shaderc/libshaderc/include/shaderc/env.h"
    "C:/Users/terra/OneDrive/Documents/references/y3/MoravaEngine/vendor/cross-platform/shaderc/libshaderc/include/shaderc/status.h"
    "C:/Users/terra/OneDrive/Documents/references/y3/MoravaEngine/vendor/cross-platform/shaderc/libshaderc/include/shaderc/visibility.h"
    "C:/Users/terra/OneDrive/Documents/references/y3/MoravaEngine/vendor/cross-platform/shaderc/libshaderc/include/shaderc/shaderc.h"
    "C:/Users/terra/OneDrive/Documents/references/y3/MoravaEngine/vendor/cross-platform/shaderc/libshaderc/include/shaderc/shaderc.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/terra/OneDrive/Documents/references/y3/MoravaEngine/vendor/cross-platform/shaderc/libshaderc/Debug/shaderc.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/terra/OneDrive/Documents/references/y3/MoravaEngine/vendor/cross-platform/shaderc/libshaderc/Release/shaderc.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/terra/OneDrive/Documents/references/y3/MoravaEngine/vendor/cross-platform/shaderc/libshaderc/MinSizeRel/shaderc.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/terra/OneDrive/Documents/references/y3/MoravaEngine/vendor/cross-platform/shaderc/libshaderc/RelWithDebInfo/shaderc.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/terra/OneDrive/Documents/references/y3/MoravaEngine/vendor/cross-platform/shaderc/libshaderc/Debug/shaderc_shared.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/terra/OneDrive/Documents/references/y3/MoravaEngine/vendor/cross-platform/shaderc/libshaderc/Release/shaderc_shared.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/terra/OneDrive/Documents/references/y3/MoravaEngine/vendor/cross-platform/shaderc/libshaderc/MinSizeRel/shaderc_shared.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/terra/OneDrive/Documents/references/y3/MoravaEngine/vendor/cross-platform/shaderc/libshaderc/RelWithDebInfo/shaderc_shared.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Users/terra/OneDrive/Documents/references/y3/MoravaEngine/vendor/cross-platform/shaderc/libshaderc/Debug/shaderc_shared.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Users/terra/OneDrive/Documents/references/y3/MoravaEngine/vendor/cross-platform/shaderc/libshaderc/Release/shaderc_shared.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Users/terra/OneDrive/Documents/references/y3/MoravaEngine/vendor/cross-platform/shaderc/libshaderc/MinSizeRel/shaderc_shared.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Users/terra/OneDrive/Documents/references/y3/MoravaEngine/vendor/cross-platform/shaderc/libshaderc/RelWithDebInfo/shaderc_shared.dll")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "C:/Users/terra/OneDrive/Documents/references/y3/MoravaEngine/vendor/cross-platform/shaderc/libshaderc/${CMAKE_INSTALL_CONFIG_NAME}/shaderc_combined.lib")
endif()

