INCLUDE(FindPkgConfig)

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
    pkg_search_module(XULRUNNER libxul-unstable>=1.9 libxul>=1.9 xulrunner-xpcom>=1.9)
    IF(XULRUNNER_FOUND)
      MESSAGE("xulrunner: ${XULRUNNER_INCLUDE_DIRS}")
    ELSE(XULRUNNER_FOUND)
      MESSAGE("xulrunner not detected; defaulting to  src/${XULRUNNER_SDK_DIR_DEFAULT}")
      set(XULRUNNER_SDK_DIR "${XULRUNNER_SDK_DIR_DEFAULT}")
    ENDIF(XULRUNNER_FOUND)
  ENDIF(XULRUNNER_SDK_DIR)
ENDIF(CMAKE_GENERATOR MATCHES "Visual Studio")

IF(XULRUNNER_SDK_DIR)
  set(XULRUNNER_INCLUDE_DIRS "${XULRUNNER_SDK_DIR}/include")
  set(XULRUNNER_LIBRARY_DIRS "${XULRUNNER_SDK_DIR}/lib")
  set(XULRUNNER_LFLAGS "-lxul -lnspr4")
ENDIF(XULRUNNER_SDK_DIR)

set(CMAKE_REQUIRED_INCLUDES "${XULRUNNER_INCLUDE_DIRS}")
set(CMAKE_REQUIRED_DEFINITIONS "${XULRUNNER_CFLAGS}")
CHECK_CXX_SOURCE_COMPILES(
  "#include \"nscore.h\"

  int main() { return 0; }"
 XULRUNNER_FLAT_INCDIR)

IF(XULRUNNER_FLAT_INCDIR)
  add_definitions(-DOSGK_GECKOINC_FLAT)
  set(CMAKE_REQUIRED_DEFINITIONS "${CMAKE_REQUIRED_DEFINITIONS} -DOSGK_GECKOINC_FLAT")
ENDIF(XULRUNNER_FLAT_INCDIR)

IF(NOT CMAKE_GENERATOR MATCHES "Visual Studio")
  CHECK_CXX_SOURCE_COMPILES(
    "#if defined(OSGK_GECKOINC_FLAT)
    #include \"nsStringAPI.h\"
    #else
    #include \"xpcom/nsStringAPI.h\"
    #endif
    
    int main() { return 0; }"
    XULRUNNER_BUILDS_WITHOUT_SHORT_WCHAR)
  IF(NOT XULRUNNER_BUILDS_WITHOUT_SHORT_WCHAR)
    set(XULRUNNER_CFLAGS "${XULRUNNER_CFLAGS} -fshort-wchar")
  ENDIF(NOT XULRUNNER_BUILDS_WITHOUT_SHORT_WCHAR)
ENDIF(NOT CMAKE_GENERATOR MATCHES "Visual Studio")

set(CMAKE_REQUIRED_INCLUDES "${OffscreenGecko_SOURCE_DIR}/src/" "${OffscreenGecko_SOURCE_DIR}/include/" "${CMAKE_REQUIRED_INCLUDES}")
CHECK_CXX_SOURCE_COMPILES(
  "#include \"defs_private.h\"
  #include GECKO_INCLUDE(xpcom,nsDebug.h)
  #include \"wrap_gfxasurface.h\"
  
  int main() { gfxASurface* surf = 0; surf->Release(); return 0; }"
  XULRUNNER_LINKS_WITHOUT_THEBES)
IF(NOT XULRUNNER_LINKS_WITHOUT_THEBES)
  set(XULRUNNER_LFLAGS "-lthebes ${XULRUNNER_LFLAGS}")
ENDIF(NOT XULRUNNER_LINKS_WITHOUT_THEBES)

include_directories(${XULRUNNER_INCLUDE_DIRS})
IF(XULRUNNER_FLAT_INCDIR)
  add_definitions(-DOSGK_GECKOINC_FLAT)
ELSE(XULRUNNER_FLAT_INCDIR)
  FOREACH(d ${XULRUNNER_INCLUDE_DIRS})
    include_directories(${d}/content)
    include_directories(${d}/dom)
    include_directories(${d}/gfx)
    include_directories(${d}/js)
    include_directories(${d}/nspr)
    include_directories(${d}/pref)
    include_directories(${d}/string)
    include_directories(${d}/thebes)
    include_directories(${d}/widget)
    include_directories(${d}/xpcom)
  ENDFOREACH(d)
ENDIF(XULRUNNER_FLAT_INCDIR)
add_definitions(${XULRUNNER_CFLAGS})

CHECK_CXX_SOURCE_COMPILES(
  "#include \"mozilla-config.h\"
  
  #ifndef MOZ_WIDGET_GTK2
  bomb out!
  #endif
  
  int main() { return 0; }"
  XULRUNNER_WIDGET_GTK2)
IF(XULRUNNER_WIDGET_GTK2)
  pkg_search_module(GTK gtk+-2.0)
  include_directories(${GTK_INCLUDE_DIRS})
  add_definitions(${GTK_CFLAGS})
  link_directories(${GTK_LIBRARY_DIRS})
  link_libraries(${GTK_LDFLAGS})
ENDIF(XULRUNNER_WIDGET_GTK2)

IF(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions("-fno-rtti -ggdb -fvisibility-inlines-hidden -fvisibility=hidden")
  link_libraries("-Wl,--warn-unresolved-symbols")
  link_libraries("-Wl,--as-needed")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)
link_directories(${XULRUNNER_LIBRARY_DIRS})
IF(NOT CMAKE_GENERATOR MATCHES "Visual Studio")
  message("Creating relaytool files...")
  EXEC_PROGRAM(${OffscreenGecko_SOURCE_DIR}/bin/relaytool out/libosgk
    ARGS --relay xul --relay nspr4 ${XULRUNNER_LDFLAGS}
    OUTPUT_VARIABLE XULRUNNER_LFLAGS)
  # -lxpcomglue added manually to work around xpcomglue_s appearing in LFLAGS on gentoo
  link_libraries("-lxpcomglue ${XULRUNNER_LFLAGS}")
ENDIF(NOT CMAKE_GENERATOR MATCHES "Visual Studio")

add_definitions(-DOSGK_BUILD)
add_definitions(-DMOZ_CAIRO_GFX)
add_definitions(-DXPCOM_GLUE=1)

file(GLOB sources *.cpp *.h)
add_library(OffscreenGecko SHARED ${sources})
add_dependencies(OffscreenGecko osgk-common)
IF(NOT CMAKE_GENERATOR MATCHES "Visual Studio")
  install(TARGETS OffscreenGecko
	  ARCHIVE DESTINATION lib
	  LIBRARY DESTINATION lib)
ENDIF(NOT CMAKE_GENERATOR MATCHES "Visual Studio")

IF(CMAKE_GENERATOR MATCHES "Visual Studio")
  target_link_libraries(OffscreenGecko 
    xpcomglue.lib xul.lib nspr4.lib js3250.lib)
  target_link_libraries(OffscreenGecko 
    delayimp.lib)
  set_target_properties (OffscreenGecko PROPERTIES
    LINK_FLAGS "/DELAYLOAD:xul.dll /DELAYLOAD:nspr4.dll /DELAYLOAD:js3250.dll")
ENDIF(CMAKE_GENERATOR MATCHES "Visual Studio")

