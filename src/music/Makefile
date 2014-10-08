
#
# Defines the project name as the source directory.
#
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
PROJECT := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

#
# Defines the part type that this project uses.
#
PART=TM4C123GH6PM

#
# Where to find the project root.
#
ROOT=../..

#
# Include the common make definitions.
#
include ${ROOT}/makedefs

#
# Where to find source files that do not live in this directory.
#
VPATH=${ROOT}/utils

#
# Where to find header files that do not live in the source directory.
#
IPATH=${ROOT}

#
# The default rule, which causes the ${PROJECT} example to be built.
#
all: ${COMPILER}
all: ${COMPILER}/${PROJECT}.axf

#
# The rule to clean out all the build products.
#
clean:
	@echo ${current_dir}
	@rm -rf ${COMPILER} ${wildcard *~}

#
# The rule to create the target directory.
#
${COMPILER}:
	@mkdir -p ${COMPILER}

#
# Rules for building the ${PROJECT} example.
#
${COMPILER}/${PROJECT}.axf: ${COMPILER}/startup_${COMPILER}.o
${COMPILER}/${PROJECT}.axf: ${COMPILER}/${PROJECT}.o
${COMPILER}/${PROJECT}.axf: ${COMPILER}/uartstdio.o
${COMPILER}/${PROJECT}.axf: ${ROOT}/driverlib/${COMPILER}/libdriver.a
${COMPILER}/${PROJECT}.axf: ${PROJECT}.ld
SCATTERgcc_${PROJECT}=${PROJECT}.ld
ENTRY_${PROJECT}=ResetISR
CFLAGSgcc=-DTARGET_IS_TM4C123_RB1

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
