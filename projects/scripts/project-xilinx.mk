####################################################################################
## Copyright 2018(c) Analog Devices, Inc.
####################################################################################

# Assumes this file is in prpojects/scripts/project-xilinx.mk
HDL_PROJECT_PATH := $(subst scripts/project-xilinx.mk,,$(lastword $(MAKEFILE_LIST)))
HDL_LIBRARY_PATH := $(HDL_PROJECT_PATH)../library/

include $(HDL_PROJECT_PATH)../quiet.mk

VIVADO := vivado -mode batch -source
SDK := xsct

CLEAN_TARGET += *.cache
CLEAN_TARGET += *tmp
CLEAN_TARGET += *.btree
CLEAN_TARGET += *.data
CLEAN_TARGET += *.xpr
CLEAN_TARGET += *.log
CLEAN_TARGET += *.jou
CLEAN_TARGET +=  xgui
CLEAN_TARGET += *.runs
CLEAN_TARGET += *.srcs
CLEAN_TARGET += *.sdk
CLEAN_TARGET += *.hw
CLEAN_TARGET += *.sim
CLEAN_TARGET += .Xil
CLEAN_TARGET += *.ip_user_files
CLEAN_TARGET += *.str
CLEAN_TARGET += mem_init_sys.txt
CLEAN_TARGET += *.csv
CLEAN_TARGET += *.hbs
CLEAN_TARGET += sw_src/standalone_bsp_0/microblaze*
CLEAN_TARGET += sw_src/standalone_bsp_0/Makefile
CLEAN_TARGET += sw_src/system_wrapper_hw_platform_0
CLEAN_TARGET += sw_src/.metadata
CLEAN_TARGET += sw_src/webtalk
CLEAN_TARGET += sw_src/RemoteSystemsTempFiles
CLEAN_TARGET += sw_src/.sdk
CLEAN_TARGET += sw_src/SDK.log
CLEAN_TARGET += sw_src/*/Debug
CLEAN_TARGET += sw_src/*/.cproject
CLEAN_TARGET += sw_src/*/.project
CLEAN_TARGET += sw_src/*/.sdkproject

# Common dependencies that all projects have
M_DEPS += system_project.tcl
M_DEPS += system_bd.tcl
M_DEPS += $(wildcard system_top*.v)
M_DEPS += $(wildcard system_constr.xdc) # Not all projects have this file
M_DEPS += $(HDL_PROJECT_PATH)scripts/adi_project_xilinx.tcl
M_DEPS += $(HDL_PROJECT_PATH)scripts/adi_env.tcl
M_DEPS += $(HDL_PROJECT_PATH)scripts/adi_board.tcl

M_DEPS += $(foreach dep,$(LIB_DEPS),$(HDL_LIBRARY_PATH)$(dep)/component.xml)

.PHONY: all lib clean clean-all
all: lib sw_src/system_wrapper.hdf

clean:
	-rm -f reference.dcp
	$(call clean, \
		$(CLEAN_TARGET), \
		$(HL)$(PROJECT_NAME)$(NC) project)

clean-all: clean
	@for lib in $(LIB_DEPS); do \
		$(MAKE) -C $(HDL_LIBRARY_PATH)$${lib} clean; \
	done

MODE ?= "default"

#$(PROJECT_NAME).sdk/system_top.hdf: $(M_DEPS)
sw_src/system_wrapper.hdf: $(M_DEPS)
	@if [ $(MODE) = incr ]; then \
		if [ -f */impl_1/system_top_routed.dcp ]; then \
			echo Found previous run result at `ls */impl_1/system_top_routed.dcp`; \
			cp -u */impl_1/system_top_routed.dcp ./reference.dcp ; \
		fi; \
		if [ -f ./reference.dcp ]; then \
			echo Using reference checkpoint for incremental compilation; \
		fi; \
	else \
		rm -f reference.dcp; \
	fi;
	-rm -rf $(CLEAN_TARGET)
	$(call build, \
		$(VIVADO) system_project.tcl, \
		$(PROJECT_NAME)_vivado.log, \
		$(HL)$(PROJECT_NAME)$(NC) project)

lib:
	@for lib in $(LIB_DEPS); do \
		$(MAKE) -C $(HDL_LIBRARY_PATH)$${lib} xilinx || exit $$?; \
	done

sdk:
	$(call build, \
		$(SDK) sdk_project.tcl, \
		$(PROJECT_NAME)_sdk.log, \
		$(HL)$(PROJECT_NAME)$(NC) project)
