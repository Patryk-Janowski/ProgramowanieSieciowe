CC=gcc
CFLAGS=-I.
#CFLAGS=

OBJECTS = 3udp_daytime_cli_listen 3udp_daytime_srvv4_br 4udp_daytime_cli_br 4udp_daytime_srvv4 6multicast_main 7multicast_main_s 7multicast_main_r

all: $(OBJECTS)

$(OBJECTS):%:%.c
	@echo Compiling $<  to  $@
	$(CC) -o $@ $< $(CFLAGS)

	
clean:
	rm  $(OBJECTS) 
