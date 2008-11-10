include_directories(../../include)
add_executable(webpage2image webpage2image.cpp)
IF(CMAKE_COMPILER_IS_GNUCXX)
  target_link_libraries(webpage2image "-Wl,--warn-unresolved-symbols -Wl,--as-needed")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)
target_link_libraries(webpage2image OffscreenGecko)
target_link_libraries(webpage2image osgk-common)

