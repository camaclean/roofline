CPPC=g++-9
CCFLAGS=-O3 -I . -I/opt/inteldevstack/intelFPGA_pro/hld/host/include20/ -g -std=c++17 -Wno-ignored-attributes -DCL_HPP_MINIMUM_OPENCL_VERSION=120 -DCL_HPP_TARGET_OPENCL_VERSION=120
LIBS =  -L${HOME}/roofline -Wl,-rpath=${HOME}/roofline -lOpenCL -pthread

all: driver1 driver2 # driver-cpu

driver1: driver2.cpp
	$(CPPC) $^ $(INC) $(CCFLAGS) -DKERNEL=1 -DDEVICE=CL_DEVICE_TYPE_GPU $(LIBS) -o $@ -lfpgametrics

driver2: driver3.cpp
	$(CPPC) $^ $(INC) $(CCFLAGS) -DKERNEL=2 -DDEVICE=CL_DEVICE_TYPE_GPU $(LIBS) -o $@

driver-cpu: driver.cpp
	$(CPPC) $^ $(INC) $(CCFLAGS) -DDEVICE=CL_DEVICE_TYPE_CPU $(LIBS) -o $@

clean:
	rm -f driver1 driver2 driver-cpu

