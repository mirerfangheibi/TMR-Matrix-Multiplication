# Triple Modular Redundancy Matrix Multiplier

Triple Modular Redundancy (TMR) is a fault-tolerant technique, and it is a form of N-MR. In this method, computations are being done in three simultaneous flows, and in the end, a voter checks the results and chooses the most frequent one between the three outcomes (Fig.1). This scheme will help to cover unwanted faults. There are more to read on: [Triple modular redundancy - Wikipedia](https://en.wikipedia.org/wiki/Triple_modular_redundancy)

The purpose of this program is applying the TMR technique on conventional matrix multiplication. Three threads do matrix multiplication, and a voter chooses the majority as the best result. The code will run on both Windows and Linux systems, but the actual behavior may vary. 

<p align="center">
  <img src="https://raw.githubusercontent.com/mirerfangheibi/TMR-Matrix-Multiplication/master/TMR.png" alt="Sublime's custom image"/><br/>
  Fig1: TMR Diagram
</p>

# Abridged implementation details

The code is split using predefined compiler macros in two parts, Linux and Windows. Windows version implemented using C++ standard [thread](http://www.cplusplus.com/reference/thread/thread/) library. The major problem with the thread library is that it is not possible to pin an individual thread to a specific CPU core. However, in Linux implementation, the core library is POSIX threads (pthread) that there is a possibility to define the desired affinity mask to assign a thread to a particular core of CPU. Read more on pthread: [POSIX Threads - Wikipedia]([https://en.wikipedia.org/wiki/POSIX_Threads](https://en.wikipedia.org/wiki/POSIX_Threads))

# Usage and features


On a Linux machine, the command `g++ -O3 -pthread -std=c++11 Source.cpp -o TMP` compiles and `./TMR` runs the code. Following running the program, a menu will come up with the following options.

0. Exit
1. Hardware Concurrency Info. of Machine
2. Input Matrices
3. Generate Random Matrices with Pre-Defined Maximum Dimension (10x10)
4. Show First Matrix
5. Show Second Matrix
6. Matrix Multiplication
7. Show Multiplication Result
8. Fault Simulation
9. Dump Matrices to File

Putting self-explanatory options aside, number 1 shows maximum threads available on the machine using the standard C++ thread library. Option 8, simulates situations that each thread may face a fault. In theory, the chance of encountering a fault is nearly zero. Therefore, adding some intentional faults is helpful to observe the functionality of this technique.

# Screen Shots
| ![FaultSimulation](https://raw.githubusercontent.com/mirerfangheibi/TMR-Matrix-Multiplication/master/Screenshots/FaultSim.png)Fig.2: Fault Simulation | ![Running](https://raw.githubusercontent.com/mirerfangheibi/TMR-Matrix-Multiplication/master/Screenshots/Running.png)Fig.3: General Running |
|--|--|


# Credits
This program was a part of a take-home midterm exam of "Fault-Tolerant Systems Design" at Sharif University of Technology.
