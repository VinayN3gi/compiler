CC = gcc
CFLAGS = -Wall -Wextra -g

SRCS = main.c lexer.c token.c parser.c icg.c codegen.c vm.c
OBJS = $(SRCS:.c=.o)
TARGET = compiler

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: all
	./compiler