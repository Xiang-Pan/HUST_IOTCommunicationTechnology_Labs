COMPONENT=RadioAndSerialAppC
TOSMAKE_PRE_EXE_DEPS += RadioAndSerial.class
TOSMAKE_CLEAN_EXTRA = *.class RadioAndSerial.java

RadioAndSerial.class: $(wildcard *.java) RadioAndSerialMsg.java
	javac -target 1.6 -source 1.6 *.java

RadioAndSerialMsg.java:
	nescc-mig java $(CFLAGS) -java-classname=RadioAndSerialMsg RadioAndSerial.h RadioAndSerialMsg -o $@


TINYOS_ROOT_DIR?=/home/hover/tinyos-main
include $(TINYOS_ROOT_DIR)/Makefile.include