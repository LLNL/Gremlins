#INCLUDE_DIRECTORIES (${MPI_C_INCLUDE_PATH} /g/g90/maiterth/Repositories/libmsr/include)
INCLUDE_DIRECTORIES (${MPI_C_INCLUDE_PATH} ${LIBMSR_INCLUDE_PATH})

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
LINK_DIRECTORIES(${LIBMSR_LIB_PATH})

add_wrapped_file(powerMeter.c   powerMeter.w)
add_library(PowerMeter SHARED powerMeter.c)
target_link_libraries(PowerMeter ${MPI_C_LIBRARIES} ${LIBMSR_LIBRARIES} )
target_link_libraries(PowerMeter dl)

add_wrapped_file(globalPowerBound.c   globalPowerBound.w)
add_library(GlobalPowerBound SHARED globalPowerBound.c ${CMAKE_CURRENT_SOURCE_DIR}/../utils/utils.c)
target_link_libraries(GlobalPowerBound ${MPI_C_LIBRARIES} ${LIBMSR_LIBRARIES} )
target_link_libraries(GlobalPowerBound dl)

add_wrapped_file(powerBound.c   powerBound.w)
add_library(PowerBound SHARED powerBound.c ${CMAKE_CURRENT_SOURCE_DIR}/../utils/utils.c)
target_link_libraries(PowerBound ${MPI_C_LIBRARIES} ${LIBMSR_LIBRARIES} )
target_link_libraries(PowerBound dl)

add_wrapped_file(tapasyasPersonalGremlin.c   tapasyasPersonalGremlin.w)
add_library(TapasyasPersonalGremlin SHARED tapasyasPersonalGremlin.c ${CMAKE_CURRENT_SOURCE_DIR}/../utils/utils.c)
target_link_libraries(TapasyasPersonalGremlin ${MPI_C_LIBRARIES} ${LIBMSR_LIBRARIES} )
target_link_libraries(TapasyasPersonalGremlin dl)

#SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

install(TARGETS PowerBound GlobalPowerBound PowerMeter TapasyasPersonalGremlin DESTINATION lib)
