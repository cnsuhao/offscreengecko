include_directories(../../include)
add_executable(webpage2image webpage2image.cpp)
target_link_libraries(webpage2image "-Wl,--warn-unresolved-symbols -Wl,--as-needed")
target_link_libraries(webpage2image OffscreenGecko)
target_link_libraries(webpage2image osgk-common)

