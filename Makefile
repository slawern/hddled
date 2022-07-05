CC = gcc 
CC_FLAGS = -DGDK_DISABLE_DEPRECATION_WARNINGS -std=c99
.PHONY: clean
	
hddled: main.c icon.h
	$(CC) $(CC_FLAGS) `pkg-config --cflags gtk+-3.0` main.c -o hddled `pkg-config --libs gtk+-3.0`
	strip hddled

clean:
	rm hddled
