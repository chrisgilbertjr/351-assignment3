main: main.cpp MemoryManager.cpp Process.cpp
	g++ main.cpp MemoryManager.cpp Process.cpp -o main

clean:
	rm -rf main

