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

    // read the input file and populate the vector
    readFile(argv[1], processes);

    for (int i=0; i<processes.size(); i++) {
        std::cout   << "P" << processes[i].getNum()
                    << ": " << processes[i].getBurstTime()
                    << "|" << processes[i].getNumBursts()
                    << "|" << processes[i].getIOTime() << std::endl;
    }

    return EXIT_SUCCESS;
}

/**
 * readFile: use the provided input filename to populate the provided vector
 * with Process objects
 */
void readFile(const std::string &filename, std::vector<Process> &processes) {
    // read the input file and populate the vector
    std::ifstream infile;
    infile.open(filename);
    while (infile.peek()) {
        if (infile.peek() == '#') {
            // throw away the line
            std::getline(infile, "");
        }
        else if (infile.peek() == '\n') {
            infile.get(); // throw away a character
        }
        else {
            // get each number up until '|' or newline, depending on the number
            std::string tmp;
            std::getline(infile, tmp, '|');
            int p_num = std::stoi(tmp);
            std::getline(infile, tmp, '|');
            int burst_time = std::stoi(tmp);
            std::getline(infile, tmp, '|');
            int num_bursts = std::stoi(tmp);
            std::getline(infile, tmp);
            int io_time = std::stoi(tmp);

            // add the process to the list of processes
            processes.push_back(Process(p_num, burst_time, num_bursts, io_time));
        }
    }
}
