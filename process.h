#IFNDEF PROCESS_H
#DEFINE PROCESS_H

class Process {
    int proc_num;
    int burst_time;
    int num_bursts;
    int cur_burst;
    int io_time;

public:
    Process(num, b_time, bursts, io_time);
    int getNum() const { return proc_num; }
    int getBurstTime() const { return burst_time; }
    int getNumBursts() const { return num_bursts; }
    int getIOTime() const { return io_time; }
    void runBurst();
    bool isComplete() { return cur_burst < num_bursts; }
}

#ENDIF
