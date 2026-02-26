# Define the compiler
CC = gcc

# Define the libraries to link
LIBS = -lsqlite3

# Define the final executable name
TARGET = sms

# This tells make to compile everything
all:
	$(CC) main.c database.c $(LIBS) -o $(TARGET)

# This cleans up the executable if you want to start fresh
clean:
	rm -f $(TARGET)