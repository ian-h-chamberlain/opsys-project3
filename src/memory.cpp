#include "memory.h"

MemoryPartition::MemoryPartition(processID, size) {
    this->processID = processID;
    this->size = size;
}

int allocateMemoryFirstFit(std::map<int, MemoryPartition>& partitions,
        char proc, int size, int t, int offset) {
    
}
