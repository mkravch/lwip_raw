
source ../../scripts/adi_env.tcl
source $ad_hdl_dir/projects/scripts/adi_project_xilinx.tcl
source $ad_hdl_dir/projects/scripts/adi_board.tcl

set ADI_USE_OOC_SYNTHESIS 1
set NUMBER_OF_PROCESSOR_CORES 24
set WRITE_REPORTS 0
set ADI_POWER_OPTIMIZATION 0
set INW_POST_ROUTE_OPTIMIZATION 1
set INW_POST_PLACE_OPTIMIZATION 1

adi_project lan_kcu105
adi_project_files lan_kcu105 [glob rtl/*.sv rtl/*.vh rtl/*.v rtl/*.txt rtl/*.vhd constr/*.xdc ip/*/*.xci]
adi_create_bd_xilinx lan_kcu105

adi_project_run lan_kcu105 1
set_property strategy Performance_ExplorePostRoutePhysOpt [get_runs impl_1]
adi_project_run lan_kcu105 2