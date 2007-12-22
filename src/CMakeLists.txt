INCLUDE(CheckCXXSourceCompiles)
INCLUDE(UsePkgConfig)

include_directories(${OffscreenGecko_SOURCE_DIR}/include)
set(XULRUNNER_SDK_DIR_DEFAULT "../xulrunner-sdk")

IF(CMAKE_GENERATOR MATCHES "Visual Studio")
  IF(NOT XULRUNNER_SDK_DIR)
    MESSAGE("XULRUNNER_SDK_DIR variable is not set; assuming src/${XULRUNNER_SDK_DIR_DEFAULT}")
    set(XULRUNNER_SDK_DIR "${XULRUNNER_SDK_DIR_DEFAULT}")
  ENDIF(NOT XULRUNNER_SDK_DIR)

  set(XULRUNNER_CFLAGS "-DXP_WIN -Zc:wchar_t-")
ELSE(CMAKE_GENERATOR MATCHES "Visual Studio")
  IF(XULRUNNER_SDK_DIR)
    MESSAGE("using XULRUNNER_SDK_DIR=${XULRUNNER_SDK_DIR}")
  ELSE(XULRUNNER_SDK_DIR)
    MESSAGE("XULRUNNER_SDK_DIR variable is not set; detecting with pkgconfig")
    PKGCONFIG(xulrunner-xpcom XULRUNNER_INCDIR XULRUNNER_LIBDIR XULRUNNER_LFLAGS XULRUNNER_CFLAGS)
    IF(XULRUNNER_INCDIR)
      MESSAGE("xulrunner: ${XULRUNNER_INCDIR}")
    ELSE(XULRUNNER_INCDIR)
      MESSAGE("xulrunner not detected; defaulting to  src/${XULRUNNER_SDK_DIR_DEFAULT}")
      set(XULRUNNER_SDK_DIR "${XULRUNNER_SDK_DIR_DEFAULT}")
    ENDIF(XULRUNNER_INCDIR)
  ENDIF(XULRUNNER_SDK_DIR)
ENDIF(CMAKE_GENERATOR MATCHES "Visual Studio")

IF(XULRUNNER_SDK_DIR)
  set(XULRUNNER_INCDIR "${XULRUNNER_SDK_DIR}/include")
  set(XULRUNNER_LIBDIR "${XULRUNNER_SDK_DIR}/lib")
  set(XULRUNNER_LFLAGS "-lxpcomglue -lxul -lnspr4")
ENDIF(XULRUNNER_SDK_DIR)

set(CMAKE_REQUIRED_INCLUDES "${XULRUNNER_INCDIR}")
set(CMAKE_REQUIRED_DEFINITIONS "${XULRUNNER_CFLAGS}")
CHECK_CXX_SOURCE_COMPILES(
  "#include \"nscore.h\"

  int main() { return 0; }"
 XULRUNNER_FLAT_INCDIR)

IF(XULRUNNER_FLAT_INCDIR)
  add_definitions(-DOSGK_GECKOINC_FLAT)
  set(CMAKE_REQUIRED_DEFINITIONS "${CMAKE_REQUIRED_DEFINITIONS} -DOSGK_GECKOINC_FLAT")
ENDIF(XULRUNNER_FLAT_INCDIR)
CHECK_CXX_SOURCE_COMPILES(
  "#if defined(OSGK_GECKOINC_FLAT)
  #include \"nsStringAPI.h\"
  #else
  #include \"xpcom/nsStringAPI.h\"
  #endif"
  XULRUNNER_BUILDS_WITHOUT_SHORT_WCHAR)
IF(NOT XULRUNNER_BUILDS_WITHOUT_SHORT_WCHAR)
  set(XULRUNNER_CFLAGS "${XULRUNNER_CFLAGS} -fshort-wchar")
ENDIF(NOT XULRUNNER_BUILDS_WITHOUT_SHORT_WCHAR)

include_directories(${XULRUNNER_INCDIR})
IF(XULRUNNER_FLAT_INCDIR)
  add_definitions(-DOSGK_GECKOINC_FLAT)
ELSE(XULRUNNER_FLAT_INCDIR)
  include_directories(${XULRUNNER_INCDIR}/dom)
  include_directories(${XULRUNNER_INCDIR}/gfx)
  include_directories(${XULRUNNER_INCDIR}/nspr)
  include_directories(${XULRUNNER_INCDIR}/pref)
  include_directories(${XULRUNNER_INCDIR}/string)
  include_directories(${XULRUNNER_INCDIR}/thebes)
  include_directories(${XULRUNNER_INCDIR}/widget)
  include_directories(${XULRUNNER_INCDIR}/xpcom)
ENDIF(XULRUNNER_FLAT_INCDIR)
add_definitions(${XULRUNNER_CFLAGS})

IF(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions("-fno-rtti")
  link_libraries("-Wl,--warn-unresolved-symbols")
  link_libraries("-Wl,--as-needed")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)
link_directories(${XULRUNNER_LIBDIR})
IF(NOT CMAKE_GENERATOR MATCHES "Visual Studio")
  message("Creating relaytool files...")
  EXEC_PROGRAM(${OffscreenGecko_SOURCE_DIR}/bin/relaytool out
    ARGS --relay xul --relay nspr4 -L${XULRUNNER_LIBDIR} ${XULRUNNER_LFLAGS}
    OUTPUT_VARIABLE XULRUNNER_LFLAGS)
ENDIF(NOT CMAKE_GENERATOR MATCHES "Visual Studio")
link_libraries("${XULRUNNER_LFLAGS}")

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
  #target_link_libraries(OffscreenGecko 
  #  xpcomglue.lib xul.lib nspr4.lib)
  target_link_libraries(OffscreenGecko 
    delayimp.lib)
  set_target_properties (OffscreenGecko PROPERTIES
    LINK_FLAGS "/DELAYLOAD:xul.dll /DELAYLOAD:nspr4.dll")
ENDIF(CMAKE_GENERATOR MATCHES "Visual Studio")

