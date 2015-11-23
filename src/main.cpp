#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <list>
#include <cstdio>

#include "process.h"

int readFile(const std::string &filename, std::list<Process> &processes);
int simulateFCFS(const std::list<Process> &processes, std::ofstream &outfile, int t_cs);
int simulateSRT(const std::list<Process> &processes, std::ofstream &outfile, int t_cs);
int simulatePWA(const std::list<Process> &processes, std::ofstream &outfile, int t_cs);

/**
 * Operating Systems Project 1
 * Ian Chamberlain
 *
 * Reads an input file of processes and simulates them
 */
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

/*    // get the final time in ms and display that the simulator endd
    finalTime = simulateFCFS(processes, outfile, t_cs);
    std::cout << "time " << finalTime << "ms: Simulator for FCFS ended [Q]" << std::endl;
    std::cout << std::endl << std::endl;*/

    // do the same for SRT
    finalTime = simulateSRT(processes, outfile, t_cs);
    std::cout << "time " << finalTime << "ms: Simulator for SRT ended [Q]" << std::endl;
    std::cout << std::endl << std::endl;

    // and PWA
    finalTime = simulatePWA(processes, outfile, t_cs);
    std::cout << "time " << finalTime << "ms: Simulator for PWA ended [Q]" << std::endl;

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
