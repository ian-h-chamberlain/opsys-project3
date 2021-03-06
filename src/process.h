/**
 * Operating Systems Project 3
 * Ian Chamberlain & Brian Kovacik
 *
 * Reads an input file of processes and simulates them
 */
#ifndef PROCESS_H
#define PROCESS_H

/**
 * Process - a data structure to hold information about the processes
 */
class Process {
    char proc_name;
    int arr_time;
    int burst_time;
    int done_time;
    int num_bursts;
    int cur_burst;
    int io_time;
    int priority;
    int remain_time;
    int wait_time;
    int add_time;

public:
    Process(char name, int arrive, int b_time, int bursts, int io_time, int priority);
    Process();
    char getNum() const { return proc_name; }
    int getArriveTime() const { return arr_time; }
    int getBurstTime() const { return burst_time; }
    int getNumBursts() const { return num_bursts; }
    int getCurBurst() const { return cur_burst; }
    int getIOTime() const { return io_time; }
    int getDoneTime() const { return done_time; }
    int getPriority() const { return priority; }
    int getRemainingTime() const { return remain_time; }
    int getWaitTime() const { return wait_time; }
    int getAddTime() const { return add_time; }
    void runForMs(int t) { remain_time -= t; }
    void setPriority(int p) { priority = p; }
    void runBurst(int cur_time);
    void runIO();
    void wait(int t) { wait_time += t; }
    bool isComplete() const  { return cur_burst >= num_bursts; }
};

class CompareProcess {
public:
    bool operator() (Process p1, Process p2) {
        return p1.getBurstTime() < p2.getBurstTime();
    }
};

class ComparePriority {
public:
    bool operator() (Process p1, Process p2) {
        if (p1.getPriority() < p2.getPriority()) {
            return true;
        }
        else if (p1.getPriority() == p2.getPriority()) {
            return p1.getNum() < p2.getNum();
        }
        else {
            return false;
        }
    }
};

class CompareAddTime {
public:
    bool operator() (Process p1, Process p2) {
        return p1.getAddTime() < p2.getAddTime();
    }
};

#endif
