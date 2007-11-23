IF(NOT CMAKE_GENERATOR MATCHES "Visual Studio")
  file(GLOB headers OffscreenGecko/*.h)
  install(FILES ${headers} DESTINATION include/OffscreenGecko)
ENDIF(NOT CMAKE_GENERATOR MATCHES "Visual Studio")
