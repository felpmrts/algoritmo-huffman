CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -Isrc
TARGET = huffman
SRCS = src/ArvoreHuffman.c src/FilaPrioridade.c src/Huffman.c src/main.c

.PHONY: all testar valgrind limpar

all: $(TARGET)

$(TARGET): $(SRCS) src/ArvoreHuffman.h src/FilaPrioridade.h src/Huffman.h
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

testar: $(TARGET)
	./$(TARGET) -e
	./$(TARGET) -b

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./$(TARGET) -e
	valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./$(TARGET) -b

limpar:
	rm -f $(TARGET) *.o temp_*.huff temp_*.txt
