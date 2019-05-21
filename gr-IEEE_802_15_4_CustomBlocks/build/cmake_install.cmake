# Install script for directory: /home/tanvir/workarea-seniordesign/senior-design-sdr/gr-IEEE_802_15_4_CustomBlocks

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
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

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/IEEE_802_15_4_CustomBlocks" TYPE FILE FILES "/home/tanvir/workarea-seniordesign/senior-design-sdr/gr-IEEE_802_15_4_CustomBlocks/cmake/Modules/IEEE_802_15_4_CustomBlocksConfig.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/tanvir/workarea-seniordesign/senior-design-sdr/gr-IEEE_802_15_4_CustomBlocks/build/include/IEEE_802_15_4_CustomBlocks/cmake_install.cmake")
  include("/home/tanvir/workarea-seniordesign/senior-design-sdr/gr-IEEE_802_15_4_CustomBlocks/build/lib/cmake_install.cmake")
  include("/home/tanvir/workarea-seniordesign/senior-design-sdr/gr-IEEE_802_15_4_CustomBlocks/build/swig/cmake_install.cmake")
  include("/home/tanvir/workarea-seniordesign/senior-design-sdr/gr-IEEE_802_15_4_CustomBlocks/build/python/cmake_install.cmake")
  include("/home/tanvir/workarea-seniordesign/senior-design-sdr/gr-IEEE_802_15_4_CustomBlocks/build/grc/cmake_install.cmake")
  include("/home/tanvir/workarea-seniordesign/senior-design-sdr/gr-IEEE_802_15_4_CustomBlocks/build/apps/cmake_install.cmake")
  include("/home/tanvir/workarea-seniordesign/senior-design-sdr/gr-IEEE_802_15_4_CustomBlocks/build/docs/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/tanvir/workarea-seniordesign/senior-design-sdr/gr-IEEE_802_15_4_CustomBlocks/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
