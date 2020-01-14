CC      = g++
CFLAGS  = -Wall -Werror -pthread
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:%.c=%.o)

.PHONY: all clean

all: wordcount

clean:
	rm *.o wordcount

clean-result:
	rm result-*.txt

clean-all: clean clean-result

compress:
	zip A2.zip README.md *.cpp *.h Makefile

%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@ -g

wordcount: $(OBJECTS)
	$(CC) $(CFLAGS) -o wordcount $(OBJECTS)
