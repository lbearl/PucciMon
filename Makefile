CXX = cc
CFLAGS = -Wall -std=gnu99
INCLUDE = -I/usr/local/include -I/usr/include -Iinclude
LDFLAGS = -L/usr/local/lib -I/usr/lib
LDLIBS = -lcurl -lrt

SOURCES = src/main.c src/common_dht_read.c src/pi_2_mmio.c src/pi_2_dht_read.c src/twilio.c
OUT = bin/c_sms

all: build

build: $(SOURCES)
	$(CXX) -o $(OUT) $(INCLUDE) $(LDFLAGS) $(CFLAGS) $(SOURCES) $(LDLIBS)

clean:
	rm -rf bin/c_sms
