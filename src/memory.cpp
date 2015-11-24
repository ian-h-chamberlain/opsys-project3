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
        for (int j=0; j<itr->second.getSize(); j++) {
            // if we've reached the end of a printed "block", print a newline
            if (i % 32 == 0) {
                std::cout << std::endl;
            }
            // print the id character
#ifdef DEBUG_MODE
            if (j == 0) {
                std::cout << '^';
                i++;
                continue;
            }
#endif
            std::cout << itr->second.getID();
            i++;
        }
        itr++;
    }

    std::cout << std::endl << std::string(32, '=') << std::endl;
}

// allocate the process by first-fit after the offset
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
        return itr->first;
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
            return itr->first;
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

// allocate the process according to best fit
int allocateMemoryBestFit(std::map<int, MemoryPartition>& partitions, char proc, int size, int t) {
    std::map<int, MemoryPartition>::iterator itr = partitions.begin();

    int minPos = -1;
    int minSize = -1;

    while (itr != partitions.end()) {
        if (itr->second.getID() == '.' && itr->second.getSize() > size) {
            if (itr->second.getSize() < minSize || minSize < 0) {
                minPos = itr->first;
                minSize = itr->second.getSize();
            }
        }
        itr++;
    }

    if (minPos < 0)
        return -1;  // failure to find a large enough empty partition

    partitions[minPos].setSize(partitions[minPos].getSize() - size);
    MemoryPartition tmp(proc, size);
    partitions[minPos + size] = partitions[minPos];
    partitions[minPos] = tmp;

    printMemory(partitions, t);
    return 0;

}

// deallocate memory for the given process
void deallocate(std::map<int, MemoryPartition>& partitions, char id, int t) {
    std::map<int, MemoryPartition>::iterator itr = partitions.begin();

    int pos = -1;
    while (itr != partitions.end()) {
        if (itr->second.getID() == id) {
            pos = itr->first;
            break;
        }
        itr++;
    }

    if (pos < 0) {
        std::cout << "Process to deallocate: '" << id << "' not found" << std::endl;
        return;
    }

    int startPos = itr->first;
    int endPos = itr->first + itr->second.getSize();

    std::map<int, MemoryPartition>::iterator prevItr = itr;

    // check if the previous partition needs to be merged
    if (pos > 0) {
        prevItr--;

        if (prevItr->second.getID() == '.') {
            startPos = prevItr->first;
            partitions.erase(prevItr);
        }
    }

    // check the next partition after this one
    std::map<int, MemoryPartition>::iterator nextItr = itr;
    nextItr++;

    if (nextItr != partitions.end() && nextItr->second.getID() == '.') {
        endPos = nextItr->first + nextItr->second.getSize();
        partitions.erase(nextItr);
    }

    partitions.erase(itr);

    MemoryPartition tmp('.', endPos - startPos);
    partitions[startPos] = tmp;

    printMemory(partitions, t);
}

// defragment the partition map provided and return the amount of time taken
int defragment(std::map<int, MemoryPartition>& partitions, int *t) {

    std::cout << "time " << *t << "ms: Starting defragmentation (suspending all processes)" << std::endl;

    printMemory(partitions, *t);

    int t_memmove = 10;

    int time = 0;

    std::map<int, MemoryPartition>::iterator itr = partitions.begin();
    MemoryPartition part = itr->second;
    int pos = itr->first;
    itr++;
    
    // loop through all partitions
    while (itr != partitions.end()) {
        // if we have empty space
        if (part.getID() == '.') {
            partitions[pos] = itr->second;
            time += (t_memmove * itr->second.getSize());
            MemoryPartition tmp('.', itr->second.getSize());
            partitions[itr->first] = tmp;
        }
        part = itr->second;
        pos = itr->first;
        itr++;

        // now combine the empty partitions if necessary
        if (itr->second.getID() == '.' && part.getID() == '.') {
            MemoryPartition tmp('.', part.getSize() + itr->second.getSize());
            partitions[pos] = tmp;
            std::map<int, MemoryPartition>::iterator tmp_itr = itr;
            tmp_itr++;
            partitions.erase(itr);
            itr = tmp_itr;
        }
    }

    std::cout << "time " << (*t + time) << "ms: Completed defragmentation (moved "
        << (time / t_memmove) << " memory units)" << std::endl;

    printMemory(partitions, *t + time);

    (*t) += time;

    return pos;
}
