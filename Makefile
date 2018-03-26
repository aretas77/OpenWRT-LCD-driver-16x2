SOURCES = lcd.c lct.c getter.c chGenerator.c
OBJS = $(SOURCES:.c=.o)
TARGET = lcd
RM = rm -f 

CFLAGS= -Wall -lm

all:
	$(CC) $(SOURCES) $(CFLAGS) -o $(TARGET)

clean:
	$(RM) $(TARGET)

