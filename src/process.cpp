/**
 * Operating Systems Project 3
 * Ian Chamberlain & Brian Kovacik
 *
 * Reads an input file of processes and simulates them
 */
#include "process.h"
#include <iostream>

/**
 * Process(int, int, int, int) - constructs a process using the input
 */
Process::Process(char name, int arrive,  int b_time, int bursts, int io_time, int priority) {
    this->proc_name = name;
    this->arr_time = arrive;
    this->burst_time = b_time;
    this->num_bursts = bursts;
    this->io_time = io_time;
    this->priority = priority;
    remain_time = burst_time;
    wait_time = 0;
    done_time = -1;
    cur_burst = 0;
}

/** 
 * Process() - An empty constructor so we can create empty objects
 */
Process::Process() {
    // no need to do anything here
}

/**
 * runBurst(int) - checks first, then adds one to the number of bursts
 */
void Process::runBurst(int cur_time) {
    // ensure that the process isn't already complete
    if (cur_burst < num_bursts) {
        cur_burst++;
        done_time = cur_time + io_time;
        remain_time = 0;
    }
    else {
        std::cerr << std::endl << "Attempt to run burst after process is already complete!" << std::endl;
    }
}

/**
 * runIO() - resets done_time to indicate that the process is no longer in the IO queue
 */
void Process::runIO() {
    done_time = -1;
    remain_time = burst_time;
}
