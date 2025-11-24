FLAGS = -l raylib
EFLAGS = -g -Wall -Wextra
TARGET = exec
LINKS = cobra.cpp comida.cpp

all:
	g++ main.cpp $(LINKS) -o $(TARGET) $(FLAGS) $(EFLAGS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)
