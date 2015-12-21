/**
 * Operating Systems Project 3
 * Ian Chamberlain & Brian Kovacik
 *
 * Reads an input file of processes and simulates them
 */

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <list>
#include <cstdio>

#include "process.h"

int readFile(const std::string &filename, std::list<Process> &processes);
int simulateSRT(const std::list<Process> &processes, std::ofstream &outfile, int t_cs, int mem_type);
int simulateRR(const std::list<Process> &processes, std::ofstream &outfile, int t_cs, int t_sl, int mem_type);

int main (int argc, char* argv[]) {

    if (argc != 1 && argc != 2) {
        std::cerr << "Usage: " << argv[0] << " [processes.txt]" << std::endl;
        return EXIT_FAILURE;
    }

    std::list<Process> processes;

    // read the input file and populate the list
    if (argc == 2) {
        if(readFile(argv[1], processes) != 0)
            return EXIT_FAILURE;
    }
    else {
        if (readFile("processes.txt", processes) != 0)
            return EXIT_FAILURE;
    }

    std::ofstream outfile("simout.txt");
    if (!outfile) {
        std::cerr << "Problem opening file simout.txt" << std::endl;
        return EXIT_FAILURE;
    }

    int t_cs = 13;
    int finalTime;

    // simulate RR with first-fit
    finalTime = simulateRR(processes, outfile, t_cs, 20, 0);
    std::cout << "time " << finalTime << "ms: Simulator for SRT ended [Q]" << std::endl;
    std::cout << std::endl << std::endl;

    // do the same for next-fit
    finalTime = simulateRR(processes, outfile, t_cs, 20, 1);
    std::cout << "time " << finalTime << "ms: Simulator for SRT ended [Q]" << std::endl;
    std::cout << std::endl << std::endl;

    // and best-fit
    finalTime = simulateRR(processes, outfile, t_cs, 20, 2);
    std::cout << "time " << finalTime << "ms: Simulator for SRT ended [Q]" << std::endl;

    // simulate SRT with first-fit
    finalTime = simulateSRT(processes, outfile, t_cs, 0);
    std::cout << "time " << finalTime << "ms: Simulator for SRT ended [Q]" << std::endl;
    std::cout << std::endl << std::endl;

    // do the same for next-fit
    finalTime = simulateSRT(processes, outfile, t_cs, 1);
    std::cout << "time " << finalTime << "ms: Simulator for SRT ended [Q]" << std::endl;
    std::cout << std::endl << std::endl;

    // and best-fit
    finalTime = simulateSRT(processes, outfile, t_cs, 2);
    std::cout << "time " << finalTime << "ms: Simulator for SRT ended [Q]" << std::endl;
    std::cout << std::endl << std::endl;

    outfile.close();

    return EXIT_SUCCESS;
}

/**
 * readFile: use the provided input filename to populate the provided list
 * with Process objects
 */
int readFile(const std::string &filename, std::list<Process> &processes) {
    // read the input file and populate the list
    std::ifstream infile(filename.c_str());
    // check the file's existence
    if (!infile) {
        std::cerr << "Problem opening file " << filename << std::endl;
        return 1;
    }

    // read the file until we reach EOF
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
            char p_name = tmp[0];
            std::getline(infile, tmp, '|');
            int arr_time = atoi(tmp.c_str());
            std::getline(infile, tmp, '|');
            int burst_time = atoi(tmp.c_str());
            std::getline(infile, tmp, '|');
            int num_bursts = atoi(tmp.c_str());
            std::getline(infile, tmp, '|');
            int io_time = atoi(tmp.c_str());
            std::getline(infile, tmp);
            int priority = atoi(tmp.c_str());

            // add the process to the list of processes
            processes.push_back(Process(p_name, arr_time, burst_time, num_bursts, io_time, priority));
        }
    }

    infile.close();

    // return success
    return 0;
}
