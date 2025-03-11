CC=gcc

CC_FLAGS= -levent -lwiringPi

C_SRCS= $(wildcard ./*.c)
C_OBJS= $(C_SRCS:.c=.o)
BINARY= blockage_detection_server 
all: run

%.o: %.c 
	$(CC)  -o $@ -c $<

run: link
	./$(BINARY)

link: $(C_OBJS)
	$(CC)  $(C_OBJS) -o $(BINARY) $(CC_FLAGS) 

clean:
	rm ./*.o




