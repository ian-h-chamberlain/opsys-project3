/**
 * Operating Systems Project 3
 * Ian Chamberlain & Brian Kovacik
 *
 * Reads an input file of processes and simulates them
 */
#ifndef MEMORY_H
#define MEMORY_H

/**
 * MemoryPartition - a data structure to hold a partition of memory
 */
class MemoryPartition {
    char processID;
    int size;
    
public:
    MemoryPartition();
    MemoryPartition(char processID, int size);
    char getID() const { return processID; }
    int getSize() const { return size; }

    void setSize(int newSize) { size = newSize; }
};

#endif
