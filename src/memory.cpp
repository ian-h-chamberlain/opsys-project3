#include <iostream>
#include <map>
#include <string>

#include "memory.h"

MemoryPartition::MemoryPartition() {
    this->processID = '.';
    this->size = 1;
}

MemoryPartition::MemoryPartition(char processID, int size) {
    this->processID = processID;
    this->size = size;
}

void printMemory(const std::map<int, MemoryPartition>& partitions, int t) {
    std::cout << "time " << t << "ms: Simulated Memory:" << std::endl;
    std::cout << std::string(32, '=');

    std::map<int, MemoryPartition>::const_iterator itr = partitions.begin();

    int i = 0;
    // loop through all partitions
    while (itr != partitions.end()) {
        // and get the size of each partition to print that many times
        for (int j=0; j<itr->second.getSize(); j++) {

            // if we've reached the end of a printed "block", print a newline
            if (i % 32 == 0) {
                std::cout << std::endl;
            }
            // print the id character
            std::cout << itr->second.getID();
            i++;
        }
        itr++;
    }

    std::cout << std::endl << std::string(32, '=') << std::endl;
}

int allocateMemoryFirstFit(std::map<int, MemoryPartition>& partitions,
        char proc, int size, int t, int offset) {
    std::map<int, MemoryPartition>::iterator itr = partitions.find(offset);

    int start = itr->first;
    MemoryPartition part = itr->second;

    // check the first partition for enough dealloated memory
    if (part.getID() == '.' && part.getSize() >= size) {
        part.setSize(part.getSize() - size);
        MemoryPartition tmp(proc, size);
        partitions[itr->first] = tmp;
        partitions[itr->first + size] = part;

        printMemory(partitions, t);
        return 0;
    }

    itr++;

    // now iterate through the rest of the partitions, looping back if necessary
    while (itr->first != start) {
        part = itr->second;
        // allocate memory if we find an empty block big enough
        if (part.getID() == '.' && part.getSize() >= size) {
            part.setSize(part.getSize() - size);
            MemoryPartition tmp(proc, size);
            partitions[itr->first] = tmp;
            partitions[itr->first + size] = part;

            printMemory(partitions, t);
            return 0;
        }
#ifdef DEBUG_MODE
        else {
            std::cout << "Blocked by " << part.getID() << std::endl;
        }
        std::cout << "position is " << itr->first << std::endl;
#endif

        itr++;

        if (itr == partitions.end()) {
            itr = partitions.begin();
        }
    }

    return -1;
}

int defragment(std::map<int, MemoryPartition>& partitions, int t) {
    return 0;
}
