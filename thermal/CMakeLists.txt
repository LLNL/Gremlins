#INCLUDE_DIRECTORIES (${MPI_C_INCLUDE_PATH} /g/g90/maiterth/Repositories/libmsr/include)
INCLUDE_DIRECTORIES (${MPI_C_INCLUDE_PATH} ${LIBMSR_INCLUDE_PATH})

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

LINK_DIRECTORIES(${LIBMSR_LIB_PATH})
add_wrapped_file(thermalMeter.c   thermalMeter.w)
add_library(ThermalMeter SHARED thermalMeter.c ../utils/utils.c)
target_link_libraries(ThermalMeter ${MPI_C_LIBRARIES} ${LIBMSR_LIBRARIES} dl)
target_link_libraries(ThermalMeter dl)

#SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

install(TARGETS ThermalMeter DESTINATION lib)
