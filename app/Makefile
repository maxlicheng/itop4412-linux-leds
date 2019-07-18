CC = arm-none-linux-gnueabi-gcc
CFLAGS = -Wall
OBJS = leds_app.o

all: leds_app

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

leds_app: $(OBJS)
	$(CC) -o $@ $(OBJS)

clean:
	rm -f rbcfg *.o
