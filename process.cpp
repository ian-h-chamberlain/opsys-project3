#include "process.h"
#include <iostream>

// Process - constructs a process using the input

Process::Process(int num, int b_time, int bursts, int io_time) {
    this->proc_num = num;
    this->burst_time = b_time;
    this->num_bursts = bursts;
    this->io_time = io_time;
    done_time = -1;
    cur_burst = 0;
    in_io = false;
}

Process::Process() {
    // no need to do anything here
}

// runBurst - checks first, then adds one to the number of bursts
void Process::runBurst(int cur_time) {
    if (cur_burst < num_bursts) {
        cur_burst++;
        done_time = cur_time + burst_time;
    }
    else {
        std::cerr << "Attempt to run burst after process is already complete!" << std::endl;
    }
}

// finishIO - changes the process's status to cpu time again
void Process::startIO(int cur_time) {
    done_time = cur_time + io_time;
}

void Process::toggleIO() {
    in_io = !in_io;
    done_time = -1;
}

// comparator for Processes
bool compareProcesses (const Process &p1, const Process &p2) {
    // account for ties using proc num
    if (p1.getDoneTime() == p2.getDoneTime()) {
        if (p1.getDoneTime() > 0) {
            return p1.getNum() < p2.getNum();
        }
    }
    return p1.getDoneTime() < p2.getDoneTime();
}
