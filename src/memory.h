#ifndef MEMORY_H
#define MEMORY_H

/**
 * MemoryPartition - a data structure to hold a partition of memory
 */
class MemoryPartition {
    char processID;
    int size;
    int position;
    
public:
    MemoryPartition(char processID, int size, int position);
    char getID() const { return processID; }
    int getSize() const { return size; }
    int getPosition() const { return position; }

    void setSize(int newSize) { size = newSize; }
    void setPosition(int newPosition) { position = newPosition; }

    bool operator<(MemoryPartition other) const;
}

#endif
