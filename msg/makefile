################################################################################
# Automatically-generated file. Do not edit!
################################################################################


RM := rm -rf

OBJS = logprint.o logtime.o buffer.o error.o misc.o
# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: libmsg

# Tool invocations
libmsg: ${OBJS}
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -o libmsg $(OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

logprint.o : logprint.c error.h 
	gcc -c logprint.c

error.o : error.c syshead.h error.h buffer.h logtime.h misc.h
	gcc -c error.c
	
buffer.o : buffer.c syshead.h common.h buffer.h error.h
	gcc -c buffer.c
	
logtime.o : logtime.c syshead.h logtime.h
	gcc -c logtime.c
misc.o : misc.c buffer.h error.h
	gcc -c misc.c
		
# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) libmsg
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
