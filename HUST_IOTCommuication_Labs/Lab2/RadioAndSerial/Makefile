COMPONENT=BlinkToRadioAppC
BUILD_EXTRA_DEPS=BlinkToRadioMsg.class
CLEAN_EXTRA = *.class BlinkToRadioMsg.java
PELAGS += -DCC2420_DEF_CHANNEL = 26

BlinkToRadioMsg.class: BlinkToRadioMsg.java
	javac BlinkToRadioMsg.java

BlinkToRadioMsg.java:
	mig java -target=null $(CFLAGS) -java-classname=BlinkToRadioMsg BlinkToRadio.h BlinkToRadioMsg -o $@

include $(MAKERULES)