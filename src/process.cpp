#include "process.h"
#include <iostream>

/**
 * Process(int, int, int, int) - constructs a process using the input
 */
Process::Process(int num, int b_time, int bursts, int io_time, int priority) {
    this->proc_num = num;
    this->burst_time = b_time;
    this->num_bursts = bursts;
    this->io_time = io_time;
    this->priority = priority;
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
    }
    else {
        std::cerr << "Attempt to run burst after process is already complete!" << std::endl;
    }
}
/**
 * runIO() - resets done_time to indicate that the process is no longer in the IO queue
 */
void Process::runIO() {
    done_time = -1;
}
