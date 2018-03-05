SOURCES = lcd.c lct.c getter.c chGenerator.c
OBJS = $(SOURCES:.c=.o)
TARGET = lcd
RM = rm -f 

CFLAGS= -Wall -lm -luci -ltlt_uci

all:
	$(CC) $(SOURCES) $(CFLAGS) -o $(TARGET)

clean:
	$(RM) $(TARGET)

