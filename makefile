FLAGS = -lsfml-graphics -lsfml-window -lsfml-system
EFLAGS = -g -Wall -Wextra
TARGET = exec

all:
	g++ main.cpp -o $(TARGET) $(FLAGS) $(EFLAGS)

run:
	./$(TARGET)

clean:
	rm -f $(TARGET)
