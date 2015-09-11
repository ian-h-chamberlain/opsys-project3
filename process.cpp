#include "process.h"
#include <iostream>

Process::Process(int num, int b_time, int bursts, int io_time) {
    this->proc_num = num;
    this->burst_time = b_time;
    this->num_bursts = bursts;
    this->io_time = io_time;
    cur_burst = 0;
}

void Process::runBurst() {
    if (cur_burst < num_bursts)
        cur_burst++;
    else {
        std::cerr << "Attempt to run burst after process is already complete!" << std::endl;
    }
}
