#ifndef PROCESS_H
#define PROCESS_H

/**
 * Process - a data structure to hold information about the processes
 */
class Process {
    int proc_num;
    int burst_time;
    int done_time;
    int num_bursts;
    int cur_burst;
    int io_time;
    int priority;
    int remain_time;

public:
    Process(int num, int b_time, int bursts, int io_time, int priority);
    Process();
    int getNum() const { return proc_num; }
    int getBurstTime() const { return burst_time; }
    int getNumBursts() const { return num_bursts; }
    int getCurBurst() const { return cur_burst; }
    int getIOTime() const { return io_time; }
    int getDoneTime() const { return done_time; }
    int getPriority() const { return priority; }
    int getRemainingTime() const { return remain_time; }
    void runForMs(int t) { remain_time -= t; }
    void setPriority(int p) { priority = p; }
    void runBurst(int cur_time);
    void runIO();
    bool isComplete() const  { return cur_burst >= num_bursts; }
};

class CompareProcess {
public:
    bool operator() (Process p1, Process p2) {
        return p1.getBurstTime() < p2.getBurstTime();
    }
};

#endif
