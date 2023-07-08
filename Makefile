TARGET = vm_riskxvii

CC = gcc

CFLAGS     = -c -Wvla -Os -std=c11
LDFLAGS    = -s
SRC        = vm_riskxvii.c helper.c operations.c memory_handling.c
OBJ        = $(SRC:.c=.o)

all:$(TARGET)

$(TARGET):$(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ)

.SUFFIXES: .c .o

.c.o:
	 $(CC) $(CFLAGS) $<

run:
	./$(TARGET)

test:
	./test.sh

clean:
	rm -f *.o *.obj $(TARGET) *.gcda *.gcno *.gcov