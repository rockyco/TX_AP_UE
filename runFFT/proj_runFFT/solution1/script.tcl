############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
## Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
############################################################
open_project proj_runFFT
set_top runFFT
add_files fft_top.cpp
add_files runFFT.cpp
add_files -tb goldenFFT.txt -cflags "-Wno-unknown-pragmas"
add_files -tb data4FFT.txt -cflags "-Wno-unknown-pragmas"
add_files -tb runFFT_tb.cpp -cflags "-Wno-unknown-pragmas"
open_solution "solution1" -flow_target vivado
set_part {xczu28dr-ffvg1517-2-e}
create_clock -period 300MHz -name default
config_rtl -reset control
set_clock_uncertainty 12.5%
#source "./proj_runFFT/solution1/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -format ip_catalog
