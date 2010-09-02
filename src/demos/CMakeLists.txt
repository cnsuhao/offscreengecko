include_directories(../../include)

IF(CMAKE_COMPILER_IS_GNUCXX)
  set(DEMO_FLAGS "-Wl,--warn-unresolved-symbols -Wl,--as-needed")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

add_executable(webpage2image webpage2image.cpp)
target_link_libraries(webpage2image ${DEMO_FLAGS} OffscreenGecko osgk-common)

add_executable(webpage2image_c webpage2image.c)
target_link_libraries(webpage2image_c ${DEMO_FLAGS} OffscreenGecko)
