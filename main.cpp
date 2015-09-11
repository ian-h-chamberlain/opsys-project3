#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <deque>
#include <cstdio>

#include "process.h"

void readFile(const std::string &filename, std::deque<Process> &processes);
int simulate(const std::deque<Process> &processes);

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

    std::deque<Process> processes;

    // read the input file and populate the deque
    readFile(argv[1], processes);

    int finalTime = simulate(processes);
    std::cout << "time " << finalTime << "ms: Simulator ended" << std::endl;

    return EXIT_SUCCESS;
}

/**
 * readFile: use the provided input filename to populate the provided deque
 * with Process objects
 */
void readFile(const std::string &filename, std::deque<Process> &processes) {
    // read the input file and populate the deque
    std::ifstream infile(filename.c_str());
    while (infile.peek() != EOF) {
        if (infile.peek() == '#' || infile.peek() == '\n') {
            // throw away the line
            std::string tmp;
            std::getline(infile, tmp);
        }
        else {
            // get each number up until '|' or newline, depending on the number
            std::string tmp;
            std::getline(infile, tmp, '|');
            int p_num = atoi(tmp.c_str());
            std::getline(infile, tmp, '|');
            int burst_time = atoi(tmp.c_str());
            std::getline(infile, tmp, '|');
            int num_bursts = atoi(tmp.c_str());
            std::getline(infile, tmp);
            int io_time = atoi(tmp.c_str());

            // add the process to the list of processes
            processes.push_back(Process(p_num, burst_time, num_bursts, io_time));
        }
    }
}
