cmake_minimum_required(VERSION 2.6)

file(GLOB SOURCES "*.cpp" "*.c" "*.h")

add_executable(systhemer ${SOURCES})

#find_package(wxWidgets COMPONENTS net gl core base)
#if(wxWidgets_FOUND)
#  include(${wxWidgets_USE_FILE})
#  # and for each of your dependent executable/library targets:
#  target_link_libraries(Wx ${wxWidgets_LIBRARIES})
#endif()
