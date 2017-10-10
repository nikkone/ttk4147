# program executable name
TARGET = list_test
# compiler flags
CFLAGS = -g -Wall
# linker flags
LDFLAGS = -g
# list of sources
SOURCES = $(shell find -name "*.c")
# default rule, to compile everything
all: $(TARGET)
# define object files
OBJECTS = $(SOURCES:.c=.o)
# link programs
$(TARGET): $(OBJECTS)
	/home/student/gr20/buildroot-avr32-v3.0.0_TTK4147/output/staging/usr/bin/avr32-linux-gcc $(LDFLAGS) -o $@ $^
# compile
%.o : %.c
	/home/student/gr20/buildroot-avr32-v3.0.0_TTK4147/output/staging/usr/bin/avr32-linux-gcc $(CFLAGS) -c -o $@ $<
# cleaning
clean:
	rm -f $(TARGET) $(OBJECTS)
