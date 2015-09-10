#include <iostream>
#include <vector>

#include "process.h"

void readFile(std::vector<Process> & processes);

/**
 * Operating Systems Project 1
 * Ian Chamberlain
 *
 * Reads an input file of processes and simulates them
 */
int main (int argc, char* argv[]) {

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " processes.txt" << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<Process> processes;

    readFile(argv[1], processes);

    return EXIT_SUCCESS;
}

/**
 * readFile: use the provided input filename to populate the provided vector
 * with Process objects
 */
void readFile(const std::string &infile, std::vector<Process> &processes) {
    // TODO: read the input file and populate the vector
}
