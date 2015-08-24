all: ledctl

CFLAGS=-O2 -s

ledctl: ledctl.o led_info.o brightness.o 
	$(CC) $(CFLAGS) -o $@ ledctl.o brightness.o led_info.o $(LDFLAGS)

ledctl.o: ledctl.c ledctl.h led_info.h brightness.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

led_info.o: led_info.c led_info.h ledctl.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

brightness.o: brightness.c brightness.h ledctl.h led_info.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

.DUMMY: clean

clean:
	rm -f ledctl *.o
