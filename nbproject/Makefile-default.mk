#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/RMCU_PIC24_SBR.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/RMCU_PIC24_SBR.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=source/main.c source/apiStartup.c source/apiDelay.c source/apiTick.c source/libModemUART.c source/apiModem.c source/apiEEPROM.c source/apiHelpers.c source/apiTask.c source/apiModbus.c source/libModbus.c source/libModbusUART.c source/cTimer.c source/apiRTC.c source/libI2C.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/source/main.o ${OBJECTDIR}/source/apiStartup.o ${OBJECTDIR}/source/apiDelay.o ${OBJECTDIR}/source/apiTick.o ${OBJECTDIR}/source/libModemUART.o ${OBJECTDIR}/source/apiModem.o ${OBJECTDIR}/source/apiEEPROM.o ${OBJECTDIR}/source/apiHelpers.o ${OBJECTDIR}/source/apiTask.o ${OBJECTDIR}/source/apiModbus.o ${OBJECTDIR}/source/libModbus.o ${OBJECTDIR}/source/libModbusUART.o ${OBJECTDIR}/source/cTimer.o ${OBJECTDIR}/source/apiRTC.o ${OBJECTDIR}/source/libI2C.o
POSSIBLE_DEPFILES=${OBJECTDIR}/source/main.o.d ${OBJECTDIR}/source/apiStartup.o.d ${OBJECTDIR}/source/apiDelay.o.d ${OBJECTDIR}/source/apiTick.o.d ${OBJECTDIR}/source/libModemUART.o.d ${OBJECTDIR}/source/apiModem.o.d ${OBJECTDIR}/source/apiEEPROM.o.d ${OBJECTDIR}/source/apiHelpers.o.d ${OBJECTDIR}/source/apiTask.o.d ${OBJECTDIR}/source/apiModbus.o.d ${OBJECTDIR}/source/libModbus.o.d ${OBJECTDIR}/source/libModbusUART.o.d ${OBJECTDIR}/source/cTimer.o.d ${OBJECTDIR}/source/apiRTC.o.d ${OBJECTDIR}/source/libI2C.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/source/main.o ${OBJECTDIR}/source/apiStartup.o ${OBJECTDIR}/source/apiDelay.o ${OBJECTDIR}/source/apiTick.o ${OBJECTDIR}/source/libModemUART.o ${OBJECTDIR}/source/apiModem.o ${OBJECTDIR}/source/apiEEPROM.o ${OBJECTDIR}/source/apiHelpers.o ${OBJECTDIR}/source/apiTask.o ${OBJECTDIR}/source/apiModbus.o ${OBJECTDIR}/source/libModbus.o ${OBJECTDIR}/source/libModbusUART.o ${OBJECTDIR}/source/cTimer.o ${OBJECTDIR}/source/apiRTC.o ${OBJECTDIR}/source/libI2C.o

# Source Files
SOURCEFILES=source/main.c source/apiStartup.c source/apiDelay.c source/apiTick.c source/libModemUART.c source/apiModem.c source/apiEEPROM.c source/apiHelpers.c source/apiTask.c source/apiModbus.c source/libModbus.c source/libModbusUART.c source/cTimer.c source/apiRTC.c source/libI2C.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/RMCU_PIC24_SBR.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=24EP256GP204
MP_LINKER_FILE_OPTION=,--script=p24EP256GP204.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/source/main.o: source/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/main.o.d 
	@${RM} ${OBJECTDIR}/source/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/main.c  -o ${OBJECTDIR}/source/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/main.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/source/apiStartup.o: source/apiStartup.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/apiStartup.o.d 
	@${RM} ${OBJECTDIR}/source/apiStartup.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/apiStartup.c  -o ${OBJECTDIR}/source/apiStartup.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/apiStartup.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/apiStartup.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/source/apiDelay.o: source/apiDelay.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/apiDelay.o.d 
	@${RM} ${OBJECTDIR}/source/apiDelay.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/apiDelay.c  -o ${OBJECTDIR}/source/apiDelay.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/apiDelay.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/apiDelay.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/source/apiTick.o: source/apiTick.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/apiTick.o.d 
	@${RM} ${OBJECTDIR}/source/apiTick.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/apiTick.c  -o ${OBJECTDIR}/source/apiTick.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/apiTick.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/apiTick.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/source/libModemUART.o: source/libModemUART.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/libModemUART.o.d 
	@${RM} ${OBJECTDIR}/source/libModemUART.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/libModemUART.c  -o ${OBJECTDIR}/source/libModemUART.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/libModemUART.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/libModemUART.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/source/apiModem.o: source/apiModem.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/apiModem.o.d 
	@${RM} ${OBJECTDIR}/source/apiModem.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/apiModem.c  -o ${OBJECTDIR}/source/apiModem.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/apiModem.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/apiModem.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/source/apiEEPROM.o: source/apiEEPROM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/apiEEPROM.o.d 
	@${RM} ${OBJECTDIR}/source/apiEEPROM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/apiEEPROM.c  -o ${OBJECTDIR}/source/apiEEPROM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/apiEEPROM.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/apiEEPROM.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/source/apiHelpers.o: source/apiHelpers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/apiHelpers.o.d 
	@${RM} ${OBJECTDIR}/source/apiHelpers.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/apiHelpers.c  -o ${OBJECTDIR}/source/apiHelpers.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/apiHelpers.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/apiHelpers.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/source/apiTask.o: source/apiTask.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/apiTask.o.d 
	@${RM} ${OBJECTDIR}/source/apiTask.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/apiTask.c  -o ${OBJECTDIR}/source/apiTask.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/apiTask.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/apiTask.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/source/apiModbus.o: source/apiModbus.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/apiModbus.o.d 
	@${RM} ${OBJECTDIR}/source/apiModbus.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/apiModbus.c  -o ${OBJECTDIR}/source/apiModbus.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/apiModbus.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/apiModbus.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/source/libModbus.o: source/libModbus.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/libModbus.o.d 
	@${RM} ${OBJECTDIR}/source/libModbus.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/libModbus.c  -o ${OBJECTDIR}/source/libModbus.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/libModbus.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/libModbus.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/source/libModbusUART.o: source/libModbusUART.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/libModbusUART.o.d 
	@${RM} ${OBJECTDIR}/source/libModbusUART.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/libModbusUART.c  -o ${OBJECTDIR}/source/libModbusUART.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/libModbusUART.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/libModbusUART.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/source/cTimer.o: source/cTimer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/cTimer.o.d 
	@${RM} ${OBJECTDIR}/source/cTimer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/cTimer.c  -o ${OBJECTDIR}/source/cTimer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/cTimer.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/cTimer.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/source/apiRTC.o: source/apiRTC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/apiRTC.o.d 
	@${RM} ${OBJECTDIR}/source/apiRTC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/apiRTC.c  -o ${OBJECTDIR}/source/apiRTC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/apiRTC.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/apiRTC.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/source/libI2C.o: source/libI2C.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/libI2C.o.d 
	@${RM} ${OBJECTDIR}/source/libI2C.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/libI2C.c  -o ${OBJECTDIR}/source/libI2C.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/libI2C.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/libI2C.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/source/main.o: source/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/main.o.d 
	@${RM} ${OBJECTDIR}/source/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/main.c  -o ${OBJECTDIR}/source/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/main.o.d"      -mno-eds-warn  -g -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/source/apiStartup.o: source/apiStartup.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/apiStartup.o.d 
	@${RM} ${OBJECTDIR}/source/apiStartup.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/apiStartup.c  -o ${OBJECTDIR}/source/apiStartup.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/apiStartup.o.d"      -mno-eds-warn  -g -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/apiStartup.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/source/apiDelay.o: source/apiDelay.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/apiDelay.o.d 
	@${RM} ${OBJECTDIR}/source/apiDelay.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/apiDelay.c  -o ${OBJECTDIR}/source/apiDelay.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/apiDelay.o.d"      -mno-eds-warn  -g -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/apiDelay.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/source/apiTick.o: source/apiTick.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/apiTick.o.d 
	@${RM} ${OBJECTDIR}/source/apiTick.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/apiTick.c  -o ${OBJECTDIR}/source/apiTick.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/apiTick.o.d"      -mno-eds-warn  -g -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/apiTick.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/source/libModemUART.o: source/libModemUART.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/libModemUART.o.d 
	@${RM} ${OBJECTDIR}/source/libModemUART.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/libModemUART.c  -o ${OBJECTDIR}/source/libModemUART.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/libModemUART.o.d"      -mno-eds-warn  -g -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/libModemUART.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/source/apiModem.o: source/apiModem.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/apiModem.o.d 
	@${RM} ${OBJECTDIR}/source/apiModem.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/apiModem.c  -o ${OBJECTDIR}/source/apiModem.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/apiModem.o.d"      -mno-eds-warn  -g -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/apiModem.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/source/apiEEPROM.o: source/apiEEPROM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/apiEEPROM.o.d 
	@${RM} ${OBJECTDIR}/source/apiEEPROM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/apiEEPROM.c  -o ${OBJECTDIR}/source/apiEEPROM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/apiEEPROM.o.d"      -mno-eds-warn  -g -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/apiEEPROM.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/source/apiHelpers.o: source/apiHelpers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/apiHelpers.o.d 
	@${RM} ${OBJECTDIR}/source/apiHelpers.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/apiHelpers.c  -o ${OBJECTDIR}/source/apiHelpers.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/apiHelpers.o.d"      -mno-eds-warn  -g -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/apiHelpers.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/source/apiTask.o: source/apiTask.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/apiTask.o.d 
	@${RM} ${OBJECTDIR}/source/apiTask.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/apiTask.c  -o ${OBJECTDIR}/source/apiTask.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/apiTask.o.d"      -mno-eds-warn  -g -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/apiTask.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/source/apiModbus.o: source/apiModbus.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/apiModbus.o.d 
	@${RM} ${OBJECTDIR}/source/apiModbus.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/apiModbus.c  -o ${OBJECTDIR}/source/apiModbus.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/apiModbus.o.d"      -mno-eds-warn  -g -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/apiModbus.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/source/libModbus.o: source/libModbus.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/libModbus.o.d 
	@${RM} ${OBJECTDIR}/source/libModbus.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/libModbus.c  -o ${OBJECTDIR}/source/libModbus.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/libModbus.o.d"      -mno-eds-warn  -g -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/libModbus.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/source/libModbusUART.o: source/libModbusUART.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/libModbusUART.o.d 
	@${RM} ${OBJECTDIR}/source/libModbusUART.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/libModbusUART.c  -o ${OBJECTDIR}/source/libModbusUART.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/libModbusUART.o.d"      -mno-eds-warn  -g -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/libModbusUART.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/source/cTimer.o: source/cTimer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/cTimer.o.d 
	@${RM} ${OBJECTDIR}/source/cTimer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/cTimer.c  -o ${OBJECTDIR}/source/cTimer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/cTimer.o.d"      -mno-eds-warn  -g -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/cTimer.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/source/apiRTC.o: source/apiRTC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/apiRTC.o.d 
	@${RM} ${OBJECTDIR}/source/apiRTC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/apiRTC.c  -o ${OBJECTDIR}/source/apiRTC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/apiRTC.o.d"      -mno-eds-warn  -g -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/apiRTC.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/source/libI2C.o: source/libI2C.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/source" 
	@${RM} ${OBJECTDIR}/source/libI2C.o.d 
	@${RM} ${OBJECTDIR}/source/libI2C.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/libI2C.c  -o ${OBJECTDIR}/source/libI2C.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/libI2C.o.d"      -mno-eds-warn  -g -omf=elf -D__DEBUG   -mlarge-code -mlarge-data -O0 -I"include" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/source/libI2C.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/RMCU_PIC24_SBR.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/RMCU_PIC24_SBR.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -D__DEBUG    -mreserve=data@0x1000:0x101B -mreserve=data@0x101C:0x101D -mreserve=data@0x101E:0x101F -mreserve=data@0x1020:0x1021 -mreserve=data@0x1022:0x1023 -mreserve=data@0x1024:0x1027 -mreserve=data@0x1028:0x104F   -Wl,--local-stack,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/RMCU_PIC24_SBR.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/RMCU_PIC24_SBR.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -D__DEBUG   -Wl,--local-stack,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}\\xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/RMCU_PIC24_SBR.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf  
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
