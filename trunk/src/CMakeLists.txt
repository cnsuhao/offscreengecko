include_directories(${OffscreenGecko_SOURCE_DIR}/include)

IF(CMAKE_GENERATOR MATCHES "Visual Studio")
  IF(NOT XULRUNNER_SDK_DIR)
    set(XULRUNNER_SDK_DIR_DEFAULT "../xulrunner-sdk")
    MESSAGE("XULRUNNER_SDK_DIR variable is not set; assuming src/${XULRUNNER_SDK_DIR_DEFAULT}")
    set(XULRUNNER_SDK_DIR "${XULRUNNER_SDK_DIR_DEFAULT}")
  ENDIF(NOT XULRUNNER_SDK_DIR)

  include_directories(${XULRUNNER_SDK_DIR}/include)
  include_directories(${XULRUNNER_SDK_DIR}/include/dom)
  include_directories(${XULRUNNER_SDK_DIR}/include/gfx)
  include_directories(${XULRUNNER_SDK_DIR}/include/nspr)
  include_directories(${XULRUNNER_SDK_DIR}/include/pref)
  include_directories(${XULRUNNER_SDK_DIR}/include/string)
  include_directories(${XULRUNNER_SDK_DIR}/include/thebes)
  include_directories(${XULRUNNER_SDK_DIR}/include/widget)
  include_directories(${XULRUNNER_SDK_DIR}/include/xpcom)
  
  link_directories(${XULRUNNER_SDK_DIR}/lib)
  
  add_definitions(-DXP_WIN)
  add_definitions(-Zc:wchar_t-)
ENDIF(CMAKE_GENERATOR MATCHES "Visual Studio")

add_definitions(-DOSGK_BUILD)
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
    xpcomglue.lib xul.lib nspr4.lib)
  target_link_libraries(OffscreenGecko 
    delayimp.lib)
  set_target_properties (OffscreenGecko PROPERTIES
    LINK_FLAGS "/DELAYLOAD:xul.dll /DELAYLOAD:nspr4.dll")
ENDIF(CMAKE_GENERATOR MATCHES "Visual Studio")

