# Install script for directory: /home/tanvir/workarea-seniordesign/senior-design-sdr/gr-IEEE_802_15_4_CustomBlocks/grc

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/gnuradio/grc/blocks" TYPE FILE FILES
    "/home/tanvir/workarea-seniordesign/senior-design-sdr/gr-IEEE_802_15_4_CustomBlocks/grc/IEEE_802_15_4_CustomBlocks_MAC_Tx.xml"
    "/home/tanvir/workarea-seniordesign/senior-design-sdr/gr-IEEE_802_15_4_CustomBlocks/grc/IEEE_802_15_4_CustomBlocks_receiver_packet_sink.xml"
    "/home/tanvir/workarea-seniordesign/senior-design-sdr/gr-IEEE_802_15_4_CustomBlocks/grc/IEEE_802_15_4_CustomBlocks_MAC_Rx.xml"
    "/home/tanvir/workarea-seniordesign/senior-design-sdr/gr-IEEE_802_15_4_CustomBlocks/grc/IEEE_802_15_4_CustomBlocks_Beacon.xml"
    "/home/tanvir/workarea-seniordesign/senior-design-sdr/gr-IEEE_802_15_4_CustomBlocks/grc/IEEE_802_15_4_CustomBlocks_MAC_Coordinator.xml"
    "/home/tanvir/workarea-seniordesign/senior-design-sdr/gr-IEEE_802_15_4_CustomBlocks/grc/IEEE_802_15_4_CustomBlocks_MAC_Sender.xml"
    "/home/tanvir/workarea-seniordesign/senior-design-sdr/gr-IEEE_802_15_4_CustomBlocks/grc/IEEE_802_15_4_CustomBlocks_AES.xml"
    "/home/tanvir/workarea-seniordesign/senior-design-sdr/gr-IEEE_802_15_4_CustomBlocks/grc/IEEE_802_15_4_CustomBlocks_MAC_Receiver.xml"
    )
endif()

