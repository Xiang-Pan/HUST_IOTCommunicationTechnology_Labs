COMPONENT=BlinkToRadioAppC
BUILD_EXTRA_DEPS=BlinkToRadio.class
CLEAN_EXTRA = *.class BlinkToRadioMsg.java
PELAGS += -DCC2420_DEF_CHANNEL = 26

BlinkToRadio.class: BlinkToRadioMsg.java
	javac *.java

BlinkToRadioMsg.java:
	mig java -target=null $(CFLAGS) -java-classname=BlinkToRadioMsg BlinkToRadio.h BlinkToRadioMsg -o $@

clean:
	rm -f BlinkToRadioMsg.java *.class

include $(MAKERULES)