
# README for Oxygen and Hydrogen Molecule Simulation

## Overview
This project simulates the creation of water molecules from oxygen and hydrogen atoms using process synchronization in C. Each atom is represented as a separate process that must coordinate with others to form water molecules according to chemical rules (H₂O requires two hydrogen atoms and one oxygen atom).

The main components of the project are:
- `proj_2.c`: Contains the main logic for atom synchronization and process creation.
- `header.h`: Includes the declaration of data structures and functions used across the program.
- `Makefile`: Used to compile the program and manage dependencies.

## Prerequisites
- Linux Operating System
- GCC Compiler
- GNU Make
- Standard C libraries

## Compilation
To compile the program, navigate to the project directory and run the following command in the terminal:
```
make all
```
This command uses the `Makefile` to compile the program with the necessary flags and libraries.

## Usage
After compiling the program, you can run it using the following command format:
```
./proj2 max_mol max_atom TB TI
```
Where:
- `max_mol` is the maximum number of water molecules that can be created.
- `max_atom` is the maximum number of atom processes (not used directly in the given code, may require implementation for specific usage).
- `TB` is the maximum time in milliseconds that a process simulating an atom can block (simulating reaction bonding time).
- `TI` is the maximum time in milliseconds that an atom waits before entering the bonding process (simulating the arrival of the atom).

## Example Command
```
./proj2 10 30 100 100
```
This command will simulate the creation of up to 10 water molecules, where atoms can wait up to 100ms before starting to bond and can take up to 100ms in the bonding process.

## Output
The program outputs the logs of atom actions to the file `proj2.out` in the project directory, detailing each step in the molecule creation process.

## Cleaning Up
To clean up the compiled files and any other generated output, you can use the following command:
```
make clean
```

## Note
Ensure all required semaphores and shared resources are properly handled and that you terminate all processes gracefully to avoid any resource leaks or deadlocks.
