COMPONENT=BlinkToRadioAppC
TOSMAKE_PRE_EXE_DEPS += BlinkToRadio.class
TOSMAKE_CLEAN_EXTRA = *.class BlinkToRadioMsg.java

BlinkToRadio.class: $(wildcard *.java) BlinkToRadioMsg.java
	javac -target 1.6 -source 1.6 *.java

BlinkToRadioMsg.java:
	nescc-mig java $(CFLAGS) -java-classname=BlinkToRadioMsg BlinkToRadio.h BlinkToRadioMsg -o $@


TINYOS_ROOT_DIR?=/home/hover/tinyos-main
include $(TINYOS_ROOT_DIR)/Makefile.include