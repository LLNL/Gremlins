#!/bin/bash
##Patch script for usage with pnmpi
##patch .so files and set your PNMPI_LIB_PATH variable to $installpath/patched
patcher= ##PATH to your pnmpi-pathc##
patcher=/g/g90/maiterth/Repositories/pnmpi-main/INSTALL/bin/pnmpi-patch
installpath= ##PATH to your install direcotry ##
installpath=/g/g90/maiterth/Repositories/gremlins_release/INSTALL_TEST

mkdir $installpath/patched
$patcher $installpath/lib/libGlobalPowerBound.so $installpath/patched/libGlobalPowerBound.so
$patcher $installpath/lib/libPowerBound.so $installpath/patched/libPowerBound.so
$patcher $installpath/lib/libTapasyasPersonalGremlin.so $installpath/patched/libTapasyasPersonalGremlin.so
$patcher $installpath/lib/libMemory_gremlin.so $installpath/patched/libMemory_gremlin.so
$patcher $installpath/lib/libPowerMeter.so $installpath/patched/libPowerMeter.so
$patcher $installpath/lib/libThermalMeter.so $installpath/patched/libThermalMeter.so
$patcher $installpath/lib/libTerseInfo.so $installpath/patched/libTerseInfo.so
$patcher $installpath/lib/libResilienceGremlin.so $installpath/patched/libResilienceGremlin.so

