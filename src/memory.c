#include "memory.h"

MemoryPartition::MemoryPartition(processID, size, position) {
    this->processID = processID;
    this->size = size;
    this->position = position;
}

MemoryPartition::operator<(MemoryPartition other) const {
    return position < other.position;
}
