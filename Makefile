all: ledctl

CFLAGS=-O2 -s

OBJECTS = ledctl.o led_info.o brightness.o device.o

ledctl: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LDFLAGS)

ledctl.o: ledctl.c ledctl.h led_info.h brightness.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

led_info.o: led_info.c led_info.h ledctl.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

brightness.o: brightness.c brightness.h ledctl.h led_info.h device.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

device.o: device.c ledctl.h led_info.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

.DUMMY: clean

clean:
	rm -f ledctl *.o
