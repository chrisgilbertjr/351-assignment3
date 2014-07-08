main: main.cpp MemoryManager.cpp Process.cpp Page.cpp
	g++ main.cpp MemoryManager.cpp Process.cpp Page.cpp -o main

clean:
	rm -rf main

