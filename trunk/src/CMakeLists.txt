include_directories(${OffscreenGecko_SOURCE_DIR}/include)

IF(CMAKE_GENERATOR MATCHES "Visual Studio")
  IF(NOT GECKO_BUILD_DIR)
    MESSAGE(SEND_ERROR "GECKO_BUILD_DIR variable is not set")
  ENDIF(NOT GECKO_BUILD_DIR)
  set(GECKO_DIST_DIR ${GECKO_BUILD_DIR}/dist)

  include_directories(${GECKO_DIST_DIR}/include)
  include_directories(${GECKO_DIST_DIR}/include/dom)
  include_directories(${GECKO_DIST_DIR}/include/gfx)
  include_directories(${GECKO_DIST_DIR}/include/nspr)
  include_directories(${GECKO_DIST_DIR}/include/pref)
  include_directories(${GECKO_DIST_DIR}/include/string)
  include_directories(${GECKO_DIST_DIR}/include/thebes)
  include_directories(${GECKO_DIST_DIR}/include/widget)
  include_directories(${GECKO_DIST_DIR}/include/xpcom)
  
  link_directories(${GECKO_BUILD_DIR}/embedding/base/standalone)
  link_directories(${GECKO_DIST_DIR}/lib)
  
  add_definitions(-DXP_WIN)
  add_definitions(-Zc:wchar_t-)
ENDIF(CMAKE_GENERATOR MATCHES "Visual Studio")

add_definitions(-DBUILD_OSGK)
add_definitions(-DMOZ_CAIRO_GFX)
add_definitions(-DXPCOM_GLUE=1)

file(GLOB sources *.cpp *.h)
add_library(OffscreenGecko SHARED ${sources})
IF(NOT CMAKE_GENERATOR MATCHES "Visual Studio")
  install(TARGETS OffscreenGecko
	  ARCHIVE DESTINATION lib
	  LIBRARY DESTINATION lib)
ENDIF(NOT CMAKE_GENERATOR MATCHES "Visual Studio")

IF(CMAKE_GENERATOR MATCHES "Visual Studio")
  target_link_libraries(OffscreenGecko 
    xpcomglue.lib xul.lib nspr4.lib embed_base_standalone.lib)
  target_link_libraries(OffscreenGecko 
    delayimp.lib)
  set_target_properties (OffscreenGecko PROPERTIES
    LINK_FLAGS "/DELAYLOAD:xul.dll /DELAYLOAD:nspr4.dll")
ENDIF(CMAKE_GENERATOR MATCHES "Visual Studio")

