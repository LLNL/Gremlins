include_directories(${MPI_C_INCLUDE_PATH})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/fault_injector)

add_wrapped_file(resilienceGremlin.c resilienceGremlin.w)
add_library(ResilienceGremlin SHARED resilienceGremlin.c fault_injector/fault_injector.cc)
target_link_libraries( ResilienceGremlin ${CMAKE_THREAD_LIBS_INIT} )
target_link_libraries( ResilienceGremlin dl)

install(TARGETS ResilienceGremlin DESTINATION lib)

#INCLUDE_DIRECTORIES(${MPI_C_INCLUDE_PATH})
#include_directories(fault_injector)
#
#SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
#LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/fault_injector)
#
#
#add_wrapped_file(resilienceGremlin.c resilienceGremlin.w)
#add_library(ResilienceGremlin SHARED resilienceGremlin.c ./fault_injector/fault_injector.cc)
#
#target_link_libraries(ResilienceGremlin ${MPI_C_LIBRARIES}) 
#find_package(Threads)
#target_link_libraries(ResilienceGremlin ${CMAKE_THREAD_LIBS_INIT})
#target_link_libraries(ResilienceGremlin dl)
#
#
#install(TARGETS ResilienceGremlin DESTINATION lib)
