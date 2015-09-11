#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <string>
#include <cstdio>

#include "process.h"

void readFile(const std::string &filename, std::vector<Process> &processes);
int simulate(const std::vector<Process> &processes);
void printQueue(const std::vector<Process> &queueToPrint);

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

    // read the input file and populate the vector
    readFile(argv[1], processes);

    std::cout << "time " << simulate(processes) << "ms: Simulator ended" << std::endl;

    return EXIT_SUCCESS;
}

/**
 * simulate: Simulate processes based on the queue passed in
 */
int simulate(const std::vector<Process> &processes) {
    std::vector<Process> execQueue(processes);
    std::vector<Process> ioQueue;

    // initial cpu time is zero
    int t = 0;
    std::cout << "time " << t << "ms: Simulator started ";
    printQueue(execQueue);

    // run process until we run out of processes in either queue
    /*
    while (execQueue.size() != 0 && ioQueue.size() != 0) {
        std::cout << "time " << t << "ms: ";
    }
    */

    return t;
}

/**
 *  printQueue: a helper function to print the process queue
 */
void printQueue(const std::vector<Process> &queueToPrint) {
    // print [Q 1 2 ... ]
    std::cout << "[Q ";
    for (unsigned int i=0; i<queueToPrint.size() - 1; i++) {
        std::cout << queueToPrint[i].getNum() << " ";
    }
    std::cout << queueToPrint[queueToPrint.size() - 1].getNum() << "]" << std::endl;
}

/**
 * readFile: use the provided input filename to populate the provided vector
 * with Process objects
 */
void readFile(const std::string &filename, std::vector<Process> &processes) {
    // read the input file and populate the vector
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
