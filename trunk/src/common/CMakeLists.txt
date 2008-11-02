file(GLOB sources *.cpp *.h)
add_definitions("-fPIC")
add_library(osgk-common STATIC ${sources})
