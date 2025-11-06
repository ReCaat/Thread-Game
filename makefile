FLAGS = -lsfml-graphics -lsfml-window -lsfml-system
TARGET = exec

all:
	g++ main.cpp -o $(TARGET) $(FLAGS)

run:
	./$(TARGET)

clean:
	rm -f $(TARGET)
