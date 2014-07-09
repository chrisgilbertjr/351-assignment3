
CS 351 OPERATING SYSTEMS
07/08/2014

Group DBZ
- Chris Gilbert - Gilbertchris123@gmail.com
- Umair Tahir   - Umair007@csu.fullerton.edu
- Kourun Sok    - Kourun@csu.fullerton.edu

Assignment #3 - Memory Management Simulator
Written in C++ on the Linux Platform

Included files
- main.cpp
- MemoryManager.cpp
- Process.cpp
- Page.cpp
- MemoryManager.h
- Process.h
- Page.h
 
    The design of our program is based around a Memory Manager class. The class has data members that describe the memory that it is managing. These members include the FCFS process queue, timers, The number of processes to manage, the number of paging in memory, the size of the memory chunk, and a list of processes and pages. The processes and pages also have their own classes that describe them. The process struct has members that describe the number of the process, its arrival time, its lifetime in memory, its time admitted into memory, its termination time, its size, and flags for when it is admitted and terminated. The Page structure has the number of the process that is is hold, the processes page numbers, and the size of the page, and an occupied flag letting you know if the page is currently being used. In main, the program asks the user for the size of memory to manage and asks them if they want the page size to be 1:100, 2:200, or 3:300. if they user puts 1, the size will be 100kb, if 2 then 200kb, and if 3 then 300kb. If it is not one of these options, 200 is chosen by default. From here the program asks the user for a filename. If the filename is valid the memory manager goes to work, if not the program terminates. All output of the memory manager is output directly to console. On another note, The entire program has a large ammount of documentation. We tried to make sure everything inside of the program was documented so there would be no confusion as to what is happening inside of the program. The header files have descriptions of the structs used, and there is inline documentation regarding what is happening during each step inside of functions.

To build the program there is an included makefile. To build from the command line, you can type 'make' into the console which will build an executable. The makefile uses g++, since the program was written in C++. You can also clean the directory of the executable with 'make clean'. This will remove the executable. The program builds into the executable p3-DBZ. In order to run the program, type './p3-DBZ' into the command line and the program will start!
