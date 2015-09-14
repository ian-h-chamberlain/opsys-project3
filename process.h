#ifndef PROCESS_H
#define PROCESS_H

// Process - a data structure to hold information about the processes
class Process {
    int proc_num;
    int burst_time;
    int done_time;
    int num_bursts;
    int cur_burst;
    int io_time;
    bool in_io;

public:
    Process(int num, int b_time, int bursts, int io_time);
    Process();
    int getNum() const { return proc_num; }
    int getBurstTime() const { return burst_time; }
    int getNumBursts() const { return num_bursts; }
    int getIOTime() const { return io_time; }
    int getDoneTime() const { return done_time; }
    void runBurst(int cur_time);
    void startIO(int cur_time);
    void toggleIO();
    bool isComplete() const  { return cur_burst >= num_bursts; }
    bool isIO() const { return in_io; }
};

#endif
